# Priority Mutex

**#TODO**

**#TODO**

<a name="readme-top"></a>

<div align="center">
  <p align="center">
    #TODO
  </p>
</div>

## About The Project

#TODO

The assumptions are:
  - #TODO 
  - #TODO
## Getting Started

### Prerequisites

I build and tested this lib with C++17.
If for some reason, on some compilers, it doesn`t work on C++17+ please email me or open an issue.

### Setup

Generate ninja build files and build

   ```sh
   $ cmake . -B build -G Ninja -DLIBFSM_COMPILE_TESTS=ON
   $ cmake --build build
   ```

### Installation

#TODO

## Snippet
This is a snippet ready-to-use. #TODO
```cpp
#include <iostream>
#include <syncstream>
#include <thread>
#include <priority_mutex.h>
#include <shared_priority_mutex.h>
#include <chrono>
#include <vector>
#include <algorithm>
 
int main()
{
    PrioSync::shared_priority_mutex<9> m1;

    auto startTime = std::chrono::system_clock::now();
    std::vector<std::thread::id> res;
    auto acquireLockAndPrint = [&m1, &startTime,&res](uint32_t threadPriority = 0, uint32_t preLockTimer = 0, uint32_t criticalSectionTimer = 0, uint32_t postUnlockTimer = 0){
        auto threadID = std::this_thread::get_id();
        std::this_thread::sleep_for(std::chrono::microseconds(preLockTimer));
        m1.lock(threadPriority);
        auto afterLockTime = std::chrono::system_clock::now();
        std::osyncstream(std::cout) << "Thread: "
                << threadID << " entered critical section with priority: " << threadPriority
                << " (" << (afterLockTime-startTime).count() << " ns)" << '\n';
        std::this_thread::sleep_for(std::chrono::microseconds(criticalSectionTimer));
        auto afterComputationTime = std::chrono::system_clock::now();
        std::osyncstream(std::cout) << "Thread: "
                << threadID << " finished its computations with priority: " << threadPriority
                << " (" << (afterComputationTime-startTime).count() << " ns)" << '\n';
        res.push_back(threadID);
        m1.unlock();
        std::this_thread::sleep_for(std::chrono::microseconds(postUnlockTimer));
    };

    std::thread thread1([&]{acquireLockAndPrint(0,0,2000);});
    std::thread thread2([&]{acquireLockAndPrint(1,700,400,200);});
    std::thread thread3([&]{acquireLockAndPrint(2,300,400,300);});
    std::thread thread4([&]{acquireLockAndPrint(3,100,200,100);});
    std::thread thread5([&]{acquireLockAndPrint(5,1200,300,200);});
    std::thread thread6([&]{acquireLockAndPrint(6,300,300,300);});
    std::thread thread7([&]{acquireLockAndPrint(7,100,100,100);});
    std::thread thread8([&]{acquireLockAndPrint(8,200,200,200);});
    thread1.join();
    thread2.join();
    thread3.join();
    thread4.join();
    thread5.join();
    thread6.join();
    thread7.join();
    thread8.join();
    bool is_sorted = std::is_sorted(res.begin()+1, res.end());
    if (is_sorted){
        std::cout << "Correct";
        return 0;
    }
    for (auto& r : res){
        std::cout << r << "  ";
    }
}
```

The output of the above will be:

```
#TODO
```

## Contributing

If you encounter any issues or would like to suggest new features, please don't hesitate to open an issue or get in touch with me at federignoli@hotmail.it.<br />Contributions are also welcome! Feel free to open pull requests to the main repository and assign me as a reviewer – I'll be sure to review them. Your help is greatly appreciated!

## License

Distributed under the MIT License. See LICENSE.txt for more information.

## Documentation
The documentation is available at the following link: https://sernior.github.io/priority-mutex/

## Contacts

Federico Abrignani - federignoli@hotmail.it

## Authors and Contributors

* Federico Abrignani (Author) - https://github.com/Sernior
* Paolo Di Giglio (Contributor) - https://github.com/pdigiglio
* Salvatore Martorana (Contributor) - https://github.com/SMartorana

