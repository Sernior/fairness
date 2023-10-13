# boost.fairness
  
> <span style="color: red;">This is not currently part of the Boost library collection.<br> However, my intention is to submit it for review once it's fully completed.</span>

<em>F.Abrignani, S. Martorana (salvatoremartorana@hotmail.com) - federignoli@hotmail.it - Version 0.1, 17.08.2023</em>

<table style="border:none;border-collapse:collapse;">
    <tr>
        <td style="border:none;border-left:none;border-top:none;border-bottom:none;width: 80px;"> <img src="warning.png" style= "width:70px; height:70px; object-fit: cover; object-position: 100% 0;"/>
        </td>
        <td style="border-right:none;border-top: none;border-bottom: none;"> 
        A C++ library containing a collection of advanced syncronization mechanisms.<br>
        </td>
    </tr>
</table>

## Overview
While the standard library offers various tools for managing concurrency, it lacks effective methods for enforcing fairness policies among threads.<br>
Tweaking such behaviors can be challenging, and mistakes might lead to issues like thread starvation. These tools, if misused, have the potential to cause harm rather than benefit. Careful implementation and understanding are crucial to harness their benefits effectively.<br>
The advanced syncronization mechanisms in this library do not autonomously adjust priorities, which means there is a risk of thread starvation if new threads are continually created and given high-priority locks; the primary intention behind creating these syncronization mechanisms is for their utilization within completed pipelines. In this context, the risk of thread starvation is non-existent due to the established pipeline structure.

The assumptions are:
  - The pipeline operates in a multi-threaded manner;
  - the pipeline is finite.

<img src="stdpipeline.png" style= "object-fit: cover; object-position: 100% 0;"/>

<img src="prioritypipeline.png" style= "object-fit: cover; object-position: 50% 0;"/>

## Motivation
#TODO

## priority_mutex

### lock()

That's an ```example``` of lock().

```cpp
void my_function(boost::fairness::priority_mutex pm, uint8_t priority) {
    //...do something
    pm.lock(priority);
    //...do something
};
```

### unlock()

That's an ```example``` of unlock().

```cpp
void my_function(boost::fairness::priority_mutex pm, uint8_t priority) {
    //...do something
    pm.unlock();
    //...do something
};
```

## shared_priority_mutex

### lock_shared

That's an ```example``` of lock_shared().

```cpp
void my_function(boost::fairness::shared_priority_mutex pm, uint8_t priority) {
    //...do something
    pm.lock_shared(priority);
    //...do something
};
```