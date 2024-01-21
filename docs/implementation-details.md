# Implementation Details

[TOC]

A technical overview of the priority mutexes in this library.

## Introduction {#extension-page-navigation}

First I want to explain for the context of this document, the difference within this library of *mutex* and *spinlock*. A mutex is different from a spinlock in the sense that they use two different ways of performing the wait;

### spinlock

A spinlock will *spinwait* which can be visualized as:

```cpp

    static const std::function<void()> relaxOrYield[] = {pause ,std::this_thread::yield};

    template<typename T ,typename K>
    inline void spin_wait(T& mem, K const expected) noexcept {
        for(int i = 0; i < BOOST_FAIRNESS_SPINWAIT_SPINS; ++i){
            if (mem.load(std::memory_order_relaxed) != expected)
                return;

            relaxOrYield[i >= BOOST_FAIRNESS_SPINWAIT_SPINS_RELAXED]();
        }
    }

```

So it will stop  the thread execution until the value in memory you are currently looking at will change (or the scheduler takes away the context from the thread).

@note To understand what [pause](https://developer.arm.com/documentation/dui0801/f/A64-General-Instructions/YIELD) does. Each architecture has an intruction like this one.

### mutex


While instead a mutex`s wait:

```cpp

    template<typename T, typename K>
    inline void wait(T& mem, K const expected) noexcept {
        auto memEqualsExpected = [&mem, expected]{
            return mem.load(std::memory_order_relaxed) == expected;
        };

        do {
            for(int i = 0; i < BOOST_FAIRNESS_WAIT_SPINS; ++i){
                if (!memEqualsExpected())
                    return;

                relaxOrYield[i >= BOOST_FAIRNESS_WAIT_SPINS_RELAXED]();
            }
            wait_(mem, expected);
        } while (memEqualsExpected());
    }

```

Not only will spin wait for a bit, but after each spin wait will use a syscall ([FUTEX op WAIT](https://man7.org/linux/man-pages/man2/futex.2.html) on linux or [waitonaddress](https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitonaddress) on windows) to stop the thread until the memory in that region is different from what we expect.
This is also very similar to how the std lib implementators have implemented the [atomic::wait](https://en.cppreference.com/w/cpp/atomic/atomic/wait).

So from now on when I mention a boost::fairness::spinlock_priority_mutex and a boost::fairness::priority_mutex the only difference between the two is how they implement the wait operation.

## False Sharing

The main problem I faced when making this library is finding ways to avoid [false sharing](https://www.cs.rochester.edu/u/scott/papers/1993_SEDMS_false_sharing.pdf).

Let`s imagine of writing a simple spinlock:

```cpp

    class spinlock{

        void lock(){
            while(locked.test_and_set())
                continue;
            return;
        };
        
        void unlock(){
            locked.clear();
        };

        std::atomic_flag locked{};
    };

```
We can easily see that this solution would suffer from the aforementioned false sharing.

Multiple cores will fetch the memory contained in the locked variable but each time one of them will change its state through [test_and_set](https://en.cppreference.com/w/cpp/atomic/atomic_flag/test_and_set) or clear all the other cores will have their local caches invalidated.

### TATAS

A simple but not scalable improvement to this approach would be to prefetch the data contained in locked and test that before performing the test and set:

```cpp

    class spinlock_tatas{

        void lock(){
            while(locked.test() && locked.test_and_set())
                continue;
            return;
        };
        
        void unlock(){
            locked.clear();
        };

        std::atomic_flag locked{};
    };

```
Now we will at least test the memory which is a simple read operation (and wont invalidate caches) before doing the test and set (which instead would invalidate caches).

This approach is known as test and test and set (or TATAS).

Basing a priority mutex on this approach is quite simple and by defining the macro BOOST_FAIRNESS_USE_TATAS_SPINLOCK users of my lib can infact use this approach.

This approach is very fast under low contention as the algorithm itself is quite simple but as stated it does not scale.

A more effective approach to address this problem is to implement a list-based locking mechanism, where each node has its own 'locked' variable. When a thread, currently holding the lock, performs an unlock operation, it sets the 'locked' variable of the next node instead of releasing a global 'locked' variable.

To completely avoid false sharing, nodes should be aligned to 128 bytes. While the majority of modern architectures have cache lines with a size of 64 bytes, the way L2 updates modified cache lines in most modern architectures occurs two at a time, potentially causing delays even among adjacent cache lines.

For instance this is from the optimization manual of my cpu (AMD zen4 microarchitecture):

<img class="readme-img" src="https://sernior.github.io/fairness/l2fetch.png" style= "object-fit: cover; object-position: 100% 0; width: 100%;"/>

## MCS Lock and Craig Algorithm

A well-known approach in the realm of list-based locks is the [MCS lock](https://www.cs.rochester.edu/u/scott/papers/1991_TOCS_synch.pdf). While the MCS lock is notable for providing mostly FIFO fairness, it doesn't align with the specific requirements for the context of this library.

I aimed for a mutex that operates like to a priority queue rather than a simple queue so that users could define their own fairness policies depending on their needs.
Luckly for me the algorithm I was looking for already existed, created in 1993 from Travis S. Craig.

Unfortunately this came with a new set of problems.

[The Craig algorithm](https://dada.cs.washington.edu/research/tr/1993/02/UW-CSE-93-02-02.pdf) relies on two essential structures: "Thread" and "Request." Efficient allocation, usage, and deallocation of these structures are crucial for the algorithm's performance.

"Thread" structures, representing threads, are relatively straightforward to handle. A simple thread-local pool of "Thread" structures is employed, eliminating the need for concurrency management.

"Request" structures require more careful consideration. To balance efficiency and resource usage, a limited pool of "Request" structures is maintained, and users can control the size using the macro BOOST_FAIRNESS_MAX_PQNODES; 

"Request" structures contain the memory used by the lock where to perform wait or spinwait (depending if you are using a priority_mutex or a priority_spinlock) so they are aligned at 128 bytes.

When a thread requests a lock, it must acquire a "Request" to enqueue. Each "Request" includes an additional atomic flag (inUse_) to indicate whether the current request is in use or available.

```cpp

    Request* getRequest(){
        for (uint32_t i = 0; i < N; ++i){
            if (!reqs_[i].inUse_.test(std::memory_order_relaxed) && !reqs_[i].inUse_.test_and_set(std::memory_order_acquire))
                return &reqs_[i];
            spin_wait();
        }
        return nullptr;
    }

```        
The spin policy, defined in the spin_wait() function, helps manage the waiting period during the acquisition of a "Request."
```cpp
    for(;;){
        req = reqs_.getRequest();
        if (req != nullptr)
            break;
        std::this_thread::yield(); // there are no free Requests now just yield
    }
```

Setting up the spin policies, such as adjusting macros like BOOST_FAIRNESS_GETREQUEST_SPINS and BOOST_FAIRNESS_GETREQUEST_SPINS_RELAXED, is crucial for achieving optimal performance. The default values for these macros have been carefully chosen and are known to perform well across different architectures. However, users are encouraged to run benchmarks and fine-tune these values based on their specific system requirements to further optimize performance.

## Shared and Recursive Mutex

Complex mutexes, such as shared and recursive mutexes, can be implemented using a priority_spinlock to ensure the atomicity of their methods. This approach not only enables the library to provide maximum fairness but also allows for seamless adaptability to changes in the priority_spinlock implementation. For instance, switching to BOOST_FAIRNESS_USE_TATAS affects all mutexes uniformly as it alters the underlying implementation.

## Other Components and Future Plans

As of January 21, 2024, this library includes RAII wrappers boost::fairness::unique_lock and boost::fairness::shared_lock, mirroring the functionality of their std counterparts. Notably, these wrappers accept a priority parameter. Currently, the library lacks priority_convars and lock_guard (low priority). Additionally, more experimentation is required for the requestPool, as there is an ongoing pursuit of improvements in that area.
