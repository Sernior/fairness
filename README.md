# boost.fairness

[![GitHub release (latest by date)](https://img.shields.io/github/v/release/Sernior/fairness)](https://github.com/Sernior/fairness/releases/latest)
[![GitHub](https://img.shields.io/github/license/Sernior/fairness)](https://github.com/Sernior/fairness/blob/main/LICENSE)
![GitHub Repo stars](https://img.shields.io/github/stars/Sernior/fairness)

> [!NOTE]  
> <span style="color: red;">This is not currently part of the Boost library collection.<br> However, my intention is to submit it for review once it's fully completed.</span>

**A collection of advanced syncronization mechanisms which are able to prioritize threads based on their priority, granting higher-priority threads access to protected resources ahead of lower-priority ones.**

**Designed to enhance throughput in pipelines by prioritizing slower threads.**

<a name="readme-top"></a>

<div align="center">
  <p align="center">
    <em>A C++ library containing a collection of advanced syncronization mechanisms.</em>
  </p>
</div>

## Motivation

### Crafting Fairness in Synchronization: A C++ Library Journey

In the realm of concurrent programming, where multiple threads traverse intricate pathways of data and computation, ensuring a level playing field becomes paramount. The art of synchronization, through mechanisms like mutexes, has long been our trusty companion in this multifaceted arena. But as the complexity of software systems continues to grow, so does the need for synchronization that offers more than just correctness—it demands fairness.

### The Call for Fairness

Picture a bustling metropolis of threads, each vying for their moment in the CPU spotlight. Traditional synchronization mechanisms, while essential, can inadvertently breed inequality. Threads may wait in obscurity, while others monopolize resources, leaving the rest yearning for their fair share of execution time. This inequality can result in contention, suboptimal resource utilization, and performance bottlenecks.

### Enter the Quest for Fair Synchronization

In this landscape of shared resources and high concurrency, arises the motivation to embark on a journey to craft a C++ library that centers on fairness. We aspire to level the playing field and ensure that every thread, regardless of its origin or intent, is given a fair chance to progress.

### Our Vision

Our vision is to develop a C++ library that provides synchronization mechanisms with fairness at its core. These mechanisms will ensure that threads are granted access to resources in a manner that minimizes waiting times and promotes an equitable distribution of CPU cycles. We envision a world where contention is reduced, where latency is minimized, and where the system hums harmoniously as threads collaborate in synchronized elegance.

### The Journey Ahead

The path we tread is not without challenges. Fair synchronization can be complex, and striking the right balance between fairness and performance is an art in itself. We must navigate through the intricate design of data structures, algorithms, and software patterns that underpin synchronization mechanisms.
Our journey involves harnessing the power of mutexes, semaphores, and other synchronization primitives, fine-tuning them to serve the cause of fairness. It requires comprehensive testing, profiling, and the wisdom to refine our design iteratively.

### Join Us in This Odyssey

As we set forth on this odyssey to create a C++ library with fairness as its banner, we invite you to join us in this quest. Together, we can bring a sense of equilibrium to concurrent programming, ensuring that the digital world operates not just efficiently but also equitably.
In our pursuit of fairness, we hope to empower developers to create software systems that are both high-performing and just, where every thread has an equal chance to shine in the grand tapestry of computation.
Join us as we embark on the journey to create a C++ library that champions the cause of synchronization fairness and resonates with the spirit of collaboration and equity. Together, we can craft a more just and efficient digital world.

## About The Project

While the standard library offers various tools for managing concurrency, it lacks effective methods for enforcing fairness policies among threads.<br>
Tweaking such behaviors can be challenging, and mistakes might lead to issues like thread starvation. These tools, if misused, have the potential to cause harm rather than benefit. Careful implementation and understanding are crucial to harness their benefits effectively.<br>
The advanced syncronization mechanisms in this library do not autonomously adjust priorities, which means there is a risk of thread starvation if new threads are continually created and given high-priority locks.

## Use Cases

Priority-based synchronization mechanisms provide a valuable tool for scenarios where traditional, non-priority-based synchronization mechanisms may not be sufficient. Here are some use-cases where a priority-based synchronization mechanism is preferred:

**Real-time Systems**:
	In real-time systems, tasks have strict timing requirements. Priority-based synchronization ensures that high-priority tasks are executed without delay, allowing critical functions like safety-critical control systems or healthcare devices to meet their deadlines.

**Resource Allocation**:
	In resource allocation scenarios where different processes or threads compete for resources (e.g., memory, CPU time), a priority-based mechanism can allocate resources to high-priority tasks, ensuring that critical processes get the resources they need promptly.

**Multimedia Processing**:
	In multimedia processing applications (e.g., video streaming), ensuring a seamless user experience is essential. Priority-based synchronization can prioritize processing tasks for the currently displayed frame over background tasks, minimizing frame drops or lags.

**Multi-User Environments**:
	In multi-user environments, such as database systems, a priority-based mechanism can give high-priority to queries or transactions that are more time-sensitive, ensuring responsiveness for users' immediate needs.

**Energy Management**:
	In battery-powered devices like smartphones, laptops, or IoT devices, priority-based synchronization can help in efficiently managing power by giving priority to background tasks when resources are scarce, and conserving energy during low-priority tasks.

**Quality of Service (QoS)**:
	In networking and communication systems, ensuring Quality of Service is essential. Priority-based synchronization mechanisms can prioritize network traffic, giving preference to critical data like VoIP calls or video conferencing, guaranteeing a smooth user experience.

**Emergency Handling**:
	Emergency handling systems (e.g., fire alarm systems, emergency response software) require immediate action. Priority-based synchronization ensures that emergency signals or events are given the highest priority, allowing rapid response.

**Fault Tolerance**:
	In systems where fault tolerance is crucial (e.g., aviation systems), priority-based synchronization can be used to ensure that redundant components or processes are always up and running, even during critical failures.

**Multi-Threaded Gaming**:
	In multi-threaded gaming engines, priority-based synchronization can prioritize rendering and user input processing, ensuring that the game responds quickly to player actions, even when background tasks are ongoing.

**Scientific Simulations**:
	Scientific simulations often have multiple tasks or calculations running concurrently. Priority-based synchronization can be used to allocate more resources to the most important or time-sensitive simulations, speeding up scientific discoveries.

In these use-cases, priority-based synchronization mechanisms allow for better resource management, responsiveness, and prioritization of critical tasks over less critical ones, enhancing system efficiency and performance in scenarios where fairness and timing are of utmost importance.

## Pipelines
The primary intention behind creating these syncronization mechanisms is for their utilization within completed pipelines. In this context, the risk of thread starvation is non-existent due to the established pipeline structure.

The assumptions are:
  - The pipeline operates in a multi-threaded manner;
  - the pipeline is finite.

<img class="readme-img" src="https://sernior.github.io/fairness/stdpipeline.png" style= "object-fit: cover; object-position: 100% 0; width: 100%;"/>

<img class="readme-img" src="https://sernior.github.io/fairness/prioritypipeline.png" style= "object-fit: cover; object-position: 100% 0; width: 100%;"/>

## Getting Started

### Prerequisites

I build and tested this lib with C++20. <br>
If for some reason, on some compilers (not MinGW), it doesn`t work on C++20+ please email me or open an issue.

### Setup boost library

Download https://www.boost.org/users/history/version_1_82_0.html

In the directory where you want to put the Boost installation, execute:

    ```sh
    $ tar --bzip2 -xf /path/to/boost_1_82_0.tar.bz2
    $ ./bootstrap.sh --prefix=/usr/local
    $ ./b2 install
    ```

### Setup

Generate ninja build files and build:

   ```sh
   $ cmake . -B build -G Ninja -DLIBFSM_COMPILE_TESTS=ON -DLIBFSM_COMPILE_BENCHMARKS=ON
   $ cmake --build build
   ```

### Installation
Using cmake you can include this library as follows:

```cmake
include(FetchContent)

FetchContent_Declare(
	fsm
  	GIT_REPOSITORY https://github.com/Sernior/fairness.git
  	GIT_TAG [TAG] #change with the tag you want to use
)

FetchContent_MakeAvailable(fsm)
```
At this point you should be able to link the library simply using:
```cmake
target_link_libraries(your_stuff.cpp fsm)
```

## Snippet
To run this code snippet, you should integrate the 'BS_thread_pool' library into your CMakeLists.txt by including it as follows:

```cmake
add_library(tp INTERFACE)# testing to remove in the end
target_include_directories(tp INTERFACE thread-pool)
target_compile_features(tp INTERFACE cxx_std_20)
```

Organize your directory structure as follows:

```
├── snippet.cpp
└── thread-pool
    └── BS_thread_pool.hpp
```

Now this snippet is ready-to-use.

```cpp
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>
#include <BS_thread_pool.hpp>
#include <boost/fairness.hpp>

static boost::fairness::slim_priority_mutex<4> ms;

static void busy_wait_nano(uint64_t nanoseconds){
    auto begin = std::chrono::steady_clock::now();
    for(;std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - begin).count() < nanoseconds;)
        continue;
}

static void thread_function_nano(int p, int preCriticalTime, int postCriticalTime, int criticalTime){
    busy_wait_nano(preCriticalTime);
    {
        boost::fairness::unique_lock l1(ms, p);
    	std::cout << "Thread with priority : " << p << " is running." << std::endl;
        busy_wait_nano(criticalTime);
    }
    busy_wait_nano(postCriticalTime);
}

int main()
{
    std::array<int, 8> prios {0, 1, 2, 1, 3, 2, 2, 0};
    std::array<int, 8> preCT {2000, 1500, 2000, 3000, 1000, 500, 500, 2000};
    std::array<int, 8> postCT {5000, 3000, 2000, 2500, 1000, 1500, 1500, 4500};
    int criticalTime = 1000;

    BS::thread_pool pool(8);

    for (int i = 0; i < 8; ++i){
        pool.push_task(thread_function_nano, prios[i], preCT[i], postCT[i], criticalTime);
    }
    pool.wait_for_tasks();

    return 0;
}
```

The output of the above could be:

```
Thread with priority : 0 is running.
Thread with priority : 0 is running.
Thread with priority : 1 is running.
Thread with priority : 1 is running.
Thread with priority : 2 is running.
Thread with priority : 2 is running.
Thread with priority : 3 is running.
Thread with priority : 3 is running.
```

## Configurations

This is a list of macros that can be defined to configure boost::fairness:

| MACRO                               | Description                                                                                                                 |
| ----------------------------------- | --------------------------------------------------------------------------------------------------------------------------- |
| BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES | Allow the inclusion of the experimental mutexes.                                                                            |
| BOOST_FAIRNESS_WAIT_SPINS           | The number of unpaused spins before a syscall to the OS to yield the cpu through futex on linux or waitOnAdress on windows. |
| BOOST_FAIRNESS_WAIT_SPINS_RELAXED   | the number of paused spins before a syscall to the OS to yield the cpu through futex on linux or waitOnAdress on windows.   |
| BOOST_FAIRNESS_HAS_DWCAS            | if the hardware supports cmpxch16b and boost::atomic is present then allow boost::fairness to use cmpxch16b.                |
| BOOST_FAIRNESS_USE_STD_WAIT_NOTIFY  | boost::fairness will use atomic::wait/notify implemented by the standard lib instead of its own implementation.             |

## Contributing

If you encounter any issues or would like to suggest new features, please don't hesitate to open an issue or get in touch with me at federignoli@hotmail.it.<br/>
Contributions are also welcome! Feel free to open pull requests to the main repository and assign me as a reviewer – I'll be sure to review them. Your help is greatly appreciated!

## License

Distributed under the Boost Software License - Version 1.0. See LICENSE.txt for more information.

## Documentation
The documentation is available at the following link: https://sernior.github.io/fairness/

## Contacts

Federico Abrignani - federignoli@hotmail.it<br/>
Salvatore Martorana - salvatoremartorana@hotmail.com

## Authors and Contributors

<a href="https://github.com/Sernior/fairness/graphs/contributors">
    <img src="https://contrib.rocks/image?repo=Sernior/fairness" />
</a>

