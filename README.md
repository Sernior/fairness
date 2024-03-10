<!-- 

//          Copyright © Federico Abrignani 2023 - 2024.
//          Copyright © Salvatore Martorana 2023 - 2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt) 

-->

# Boost.Fairness

[![GitHub release (latest by date)](https://img.shields.io/github/v/release/Sernior/fairness)](https://github.com/Sernior/fairness/releases/latest)
[![GitHub](https://img.shields.io/github/license/Sernior/fairness)](https://github.com/Sernior/fairness/blob/main/LICENSE)
![GitHub Repo stars](https://img.shields.io/github/stars/Sernior/fairness)

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

The reason I started Boost.Fairness is that the operating system makes decisions regarding scheduling policies without knowledge of the application layer. Sometimes, a programmer optimizing an application may want to prioritize certain threads for various reasons.

When I started Boost.Fairness, I believed it would be relatively easy, but it turned out not to be. Throughout my study and development of Boost.Fairness, I realized that in this field, there are no "silver bullets" and each solution has its own set of pros and cons.
Unable to find a universal solution for every architecture, I opted to allow users to customize certain critical aspects of the library. I recommend users of Boost.Fairness to read [this page](https://sernior.github.io/fairness/md_docs_2implementation-details.html).

## About The Project

The standard library provides tools for handling concurrency but lacks effective ways to allow programmers to choose fairness policies.<br>
Modifying these behaviors can be tricky, and mistakes may lead to issues such as thread starvation. If not used carefully, these tools can do more harm than good, emphasizing the importance of cautious usage.<br>
Furthermore, the advanced synchronization mechanisms in Boost.Fairness do not automatically adjust priorities. This poses a risk of thread starvation when new threads are consistently created with high-priority locks.

## Use Cases

Although priority-based locks are primarily designed to mitigate the issue of starvation and prevent it, they can also serve as a valuable tool in scenarios where traditional, non-priority-based synchronization mechanisms fall short. Here are some scenarios where a priority-based synchronization mechanism may be the preferred choice:

**Real-time Systems**:
	In real-time systems, tasks have strict timing requirements. Priority-based synchronization could be used that high-priority tasks are executed without delay, allowing critical functions like safety-critical control systems or healthcare devices to meet their deadlines.

**Resource Allocation**:
	In resource allocation scenarios where different processes or threads compete for resources (e.g., memory, CPU time), a priority-based mechanism can allocate resources to high-priority tasks, could be used to ensure that get the resources they need promptly.

**Multi-Threaded Gaming**:
	In multi-threaded gaming engines, priority-based synchronization can prioritize slower threads in order to increase throughput of the graphical pipeline so to increase FPS.

**Scientific Simulations**:
	Scientific simulations involve complex pipelines where maximizing throughput is crucial. Concurrent execution of multiple tasks in simulations makes priority-based synchronization effective for fine-tuning fairness policies and optimizing throughput.

In these use-cases, priority-based synchronization mechanisms enhance resource management, responsiveness, and prioritization of critical tasks, thereby improving system efficiency and performance in situations where fairness and timing are critical.<br>
Additionally, managing fairness policies helps optimize around issues like priority inversion.

## Pipelines
The primary intention behind creating these syncronization mechanisms is for their utilization within completed pipelines. In this context, the risk of thread starvation is non-existent due to the established pipeline structure.

The assumptions are:
  - The pipeline operates in a multi-threaded manner;
  - the pipeline is finite.

<img class="readme-img" src="https://sernior.github.io/fairness/stdpipeline.png" style= "object-fit: cover; object-position: 100% 0; width: 100%;"/>

<img class="readme-img" src="https://sernior.github.io/fairness/prioritypipeline.png" style= "object-fit: cover; object-position: 100% 0; width: 100%;"/>

**These two images depict the same pipeline. The first image illustrates the execution of the pipeline with a mostly FIFO lock, while the second image demonstrates an improved execution using a priority lock. The priority lock could be employed to reduce the overall execution time of the pipeline.**

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
$ cmake . -B build -G Ninja -DLIB_FAIRNESS_COMPILE_TESTS=ON -DLIB_FAIRNESS_COMPILE_BENCHMARKS=ON
$ cmake --build build
```

### Installation
Using cmake you can include Boost.Fairness as follows:

```cmake
include(FetchContent)

FetchContent_Declare(
	fairness
  	GIT_REPOSITORY https://github.com/Sernior/fairness.git
  	GIT_TAG [TAG] #change with the tag you want to use
)

FetchContent_MakeAvailable(fairness)
```
At this point you should be able to link the library simply using:
```cmake
target_link_libraries(your_stuff.cpp fairness)
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

| MACRO                                        | Default     | Description                                                                                                                                                                                              |
| -------------------------------------------- | ----------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| BOOST_FAIRNESS_WAIT_SPINS                    | 16          | The total number of spins before a syscall to the OS to yield the cpu through futex on linux or waitOnAdress on windows (this is used only if BOOST_FAIRNESS_USE_EXPERIMENTAL_WAIT_NOTIFY is defined).   |
| BOOST_FAIRNESS_WAIT_SPINS_RELAXED            | 12          | The number of paused spins before a syscall to the OS to yield the cpu through futex on linux or waitOnAdress on windows (this is used only if BOOST_FAIRNESS_USE_EXPERIMENTAL_WAIT_NOTIFY is defined).  |
| BOOST_FAIRNESS_USE_TATAS                     | not defined | If defined contention will be solved with a simple tatas-based algorithm witch is faster but doesn't scale.                                                                                              |
| BOOST_FAIRNESS_SPINWAIT_SPINS                | 16          | The total number of spins performed while spin waiting.                                                                                                                                                  |
| BOOST_FAIRNESS_SPINWAIT_SPINS_RELAXED        | 12          | The number of paused spins performed while spin waiting.                                                                                                                                                 |
| BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE     | 128         | Hardware destructive size used by Boost.Fairness.                                                                                                                                                        |
| BOOST_FAIRNESS_MAX_PQNODES                   | 4           | The number of requests each priority mutex has available (this is used only if BOOST_FAIRNESS_USE_TATAS is not defined).                                                                                 |
| BOOST_FAIRNESS_MAX_THREADS                   | 4           | The total number of thread structs each thread has available (this is used only if BOOST_FAIRNESS_USE_TATAS is not defined).                                                                             |
| BOOST_FAIRNESS_USE_EXPERIMENTAL_WAIT_NOTIFY  | not defined | Use a custom implementation to perform atomic::wait and atomic::notify instead of the standard ones.                                                                                                     |
| BOOST_FAIRNESS_GETREQUEST_SPINS              | 1           | The total number of spins performed while performing a getRequest (this is used only if BOOST_FAIRNESS_USE_TATAS is not defined).                                                                        |
| BOOST_FAIRNESS_GETREQUEST_SPINS_RELAXED      | 1           | The number of paused spins performed while performing a getRequest (this is used only if BOOST_FAIRNESS_USE_TATAS is not defined).                                                                       |

## Contributing

If you encounter any issues or would like to suggest new features, please don't hesitate to open an issue or get in touch at federignoli@hotmail.it.<br/>
Contributions are also welcome! Feel free to open pull requests to the main repository and assign me as a reviewer – I'll be sure to review them. Your help is greatly appreciated!

## License

Distributed under the Boost Software License - Version 1.0. See [LICENSE](https://github.com/Sernior/fairness/blob/main/LICENSE) for more information.

## Documentation
The documentation is available at the following link: https://sernior.github.io/fairness/

## Contacts

Federico Abrignani - federignoli@hotmail.it<br/>
Salvatore Martorana - salvatoremartorana@hotmail.com

## Authors and Contributors

<a href="https://github.com/Sernior/fairness/graphs/contributors">
    <img src="https://contrib.rocks/image?repo=Sernior/fairness" />
</a>

