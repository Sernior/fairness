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

In the realm of concurrent programming, where multiple threads traverse intricate pathways of data and computation, ensuring a level playing field becomes paramount. The art of synchronization, through mechanisms like mutexes, has long been our trusty companion in this multifaceted arena. But as the complexity of software systems continues to grow, so does the need for synchronization that offers more than just correctness. It demands fairness.

In this landscape of shared resources and high concurrency, arises the motivation to embark on a journey to craft a C++ library that centers on fairness. We aspire to level the playing field and ensure that every thread, regardless of its origin or intent, is given a fair chance to progress.

### Our Vision

Our vision is to develop a C++ library that provides synchronization mechanisms with fairness at its core allowing users to express their own fairness policies.
Fair synchronization can be complex, and striking the right balance between fairness and performance is an art in itself.
We must navigate through the intricate design of data structures, algorithms, and software patterns that underpin synchronization mechanisms. Users of this library should be aware that there is no silver bullet in concurrency. These tools are supposed to be used in contextes where the target software has been profiled and bottlenecks already identified.

## About The Project

While the standard library offers various tools for managing concurrency, it lacks effective methods for enforcing fairness policies among threads.<br>
Tweaking such behaviors can be challenging, and mistakes might lead to issues like thread starvation. These tools, if misused, have the potential to cause harm rather than benefit. Careful implementation and understanding are crucial to harness their benefits effectively.<br>
The advanced syncronization mechanisms in this library do not autonomously adjust priorities, which means there is a risk of thread starvation if new threads are continually created and given high-priority locks.

## Use Cases

Although priority-based locks are primarily designed to mitigate the issue of starvation and prevent it, they can also serve as a valuable tool in scenarios where traditional, non-priority-based synchronization mechanisms fall short. Here are some scenarios where a priority-based synchronization mechanism may be the preferred choice:

**Real-time Systems**:
	In real-time systems, tasks have strict timing requirements. Priority-based synchronization could be used that high-priority tasks are executed without delay, allowing critical functions like safety-critical control systems or healthcare devices to meet their deadlines.

**Resource Allocation**:
	In resource allocation scenarios where different processes or threads compete for resources (e.g., memory, CPU time), a priority-based mechanism can allocate resources to high-priority tasks, could be used to ensure that get the resources they need promptly.

**Energy Management**:
	In battery-powered devices like smartphones, laptops, or IoT devices, priority-based synchronization can help in efficiently managing power by giving priority to background tasks when resources are scarce, and conserving energy during low-priority tasks.

**Quality of Service (QoS)**:
	In networking and communication systems, ensuring Quality of Service is essential. Priority-based synchronization mechanisms can prioritize network traffic, giving preference to critical data like VoIP calls or video conferencing, guaranteeing a smooth user experience.

**Emergency Handling**:
	Emergency handling systems (e.g., fire alarm systems, emergency response software) require immediate action. Priority-based synchronization ensures that emergency signals or events are given the highest priority, allowing rapid response.

**Fault Tolerance**:
	In systems where fault tolerance is crucial (e.g., aviation systems), priority-based synchronization can be used to ensure that redundant components or processes are always up and running, even during critical failures.

**Multi-Threaded Gaming**:
	In multi-threaded gaming engines, priority-based synchronization can prioritize slower threads in order to increase throughput of the graphical pipeline so to increase FPS.

**Scientific Simulations**:
	Scientific simulations typically involve intricate pipelines, and maximizing throughput is paramount. These simulations frequently entail the concurrent execution of multiple tasks and calculations, making them ideal candidates for developers to fine-tune fairness policies in order to optimize throughput.

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

