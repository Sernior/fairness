# boost.fairness

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

## About The Project

While the standard library offers various tools for managing concurrency, it lacks effective methods for enforcing fairness policies among threads.<br>
Tweaking such behaviors can be challenging, and mistakes might lead to issues like thread starvation. These tools, if misused, have the potential to cause harm rather than benefit. Careful implementation and understanding are crucial to harness their benefits effectively.<br>
The advanced syncronization mechanisms in this library do not autonomously adjust priorities, which means there is a risk of thread starvation if new threads are continually created and given high-priority locks; the primary intention behind creating these syncronization mechanisms is for their utilization within completed pipelines. In this context, the risk of thread starvation is non-existent due to the established pipeline structure.

The assumptions are:
  - The pipeline operates in a multi-threaded manner;
  - the pipeline is finite.
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

This is an header only library but you can build the tests using:

Generate ninja build files and build:

   ```sh
   $ cmake . -B build -G Ninja -DLIBFSM_COMPILE_TESTS=ON -DLIBFSM_COMPILE_BENCHMARKS=ON
   $ cmake --build build
   ```

### Installation
Using cmake you can include this library using:

```cmake
include(FetchContent)

FetchContent_Declare(
  fsm
  GIT_REPOSITORY https://github.com/Sernior/priority-mutex.git
  GIT_TAG [TAG] #change with the tag you want to use
)

FetchContent_MakeAvailable(fsm)
```
At this point you should be able to link the library simply using:
```cmake
target_link_libraries(your_stuff.cpp fsm)
```

## Snippet
This is a snippet ready-to-use.
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
    ms.lock(p);
    std::cout << "Thread with priority : " << p << " is running."<<std::endl;
    busy_wait_nano(criticalTime);
    ms.unlock();
    busy_wait_nano(postCriticalTime);
}

int main()
{
    std::array<int, 8> prios {0, 2, 2, 1, 1, 3, 3, 0};
    std::array<int, 8> preCT {2000, 1500, 2000, 3000, 1000, 500, 500, 2000};
    std::array<int, 8> postCT {5000, 3000, 2000, 2500, 1000, 1500, 1500, 4500};
    int criticalTime = 1000;

    BS::thread_pool pool(8);

    for (int i = 0; i < 8; i++){
        pool.push_task(thread_function_nano, prios[i], preCT[i], postCT[i], criticalTime);
    }
    pool.wait_for_tasks();

    return 0;
}
```

The output of the above will be:

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

## Contributing

If you encounter any issues or would like to suggest new features, please don't hesitate to open an issue or get in touch with me at federignoli@hotmail.it.<br />Contributions are also welcome! Feel free to open pull requests to the main repository and assign me as a reviewer – I'll be sure to review them. Your help is greatly appreciated!

## License

Distributed under the Boost Software License - Version 1.0. See LICENSE.txt for more information.

## Documentation
The documentation is available at the following link: https://sernior.github.io/priority-mutex/

## Contacts

Federico Abrignani - federignoli@hotmail.it

## Authors and Contributors

* Federico Abrignani (Author) - https://github.com/Sernior
* Salvatore Martorana (Contributor) - https://github.com/SMartorana
* Paolo Di Giglio (Contributor) - https://github.com/pdigiglio

