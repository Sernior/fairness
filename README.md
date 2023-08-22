# Priority Mutex

**A collection of mutexes which are able to prioritize threads based on their priority, granting higher-priority threads access to protected resources ahead of lower-priority ones.**

**Designed to enhance throughput in pipelines by prioritizing slower threads.**

<a name="readme-top"></a>

<div align="center">
  <p align="center">
    A C++ lib containing a collection of advanced mutexes.
  </p>
</div>

## About The Project

While the standard library offers various tools for managing concurrency, it lacks effective methods for enforcing fairness policies among threads.<br>
Tweaking such behaviors can be challenging, and mistakes might lead to issues like thread starvation. These mutexes, if misused, have the potential to cause harm rather than benefit. Careful implementation and understanding are crucial to harness their benefits effectively.<br>
The priority mutexes in this library do not autonomously adjust priorities, which means there is a risk of thread starvation if new threads are continually created and given high-priority locks; the primary intention behind creating these mutexes is for their utilization within completed pipelines. In this context, the risk of thread starvation is non-existent due to the established pipeline structure.

The assumptions are:
  - The pipeline operates in a multi-threaded manner.
  - the pipeline is finished.
## Getting Started

### Prerequisites

I build and tested this lib with C++17.
If for some reason, on some compilers, it doesn`t work on C++17+ please email me or open an issue.

### Setup

This is an header only library but you can build the tests using:

Generate ninja build files and build

   ```sh
   $ cmake . -B build -G Ninja -DLIBFSM_COMPILE_TESTS=ON
   $ cmake --build build
   ```

### Installation
Using cmake you can include this lib using:

```
include(FetchContent)

FetchContent_Declare(
  fsm
  GIT_REPOSITORY https://github.com/Sernior/priority-mutex.git
  GIT_TAG [TAG] #change with the tag you want to use
)

FetchContent_MakeAvailable(fsm)
```
At this point you should be able to link the library simply using:
```
target_link_libraries(your_stuff.cpp fsm)
```

## Snippet
This is a snippet ready-to-use.
```cpp
#include <iostream>
#include <chrono>
#include <vector>
#include <priority_mutex.h>

static PrioSync::priority_mutex<4> m;


void threadFunction(PrioSync::Priority_t prio) {
    m.lock(prio);
    std::cout << "Thread with priority : " << int(prio) << " is running." << std::endl;
    m.unlock();
}



int main() {
    m.lock();
    std::vector<std::thread> tp;
    for (int i = 0; i < 4; i++){
        tp.push_back(std::thread(&threadFunction, i));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));//make sure the threads lock themselves
    m.unlock();
    for (auto& t : tp)
        t.join();

    return 0;
}
```

The output of the above will be:

```
Thread with priority : 0 is running.
Thread with priority : 1 is running.
Thread with priority : 2 is running.
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
* Paolo Di Giglio (Contributor) - https://github.com/pdigiglio
* Salvatore Martorana (Contributor) - https://github.com/SMartorana

