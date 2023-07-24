#include <iostream>
#include <mutex>
#include <condition_variable>
#include <set>

struct threadPriority{
    uint32_t prio{};
    mutable uint32_t waiting{};
    mutable std::condition_variable cv;
    threadPriority(uint32_t priority){
        prio = priority;
    }
    bool operator<(threadPriority const& other) const{
        return prio < other.prio;
    }
    bool operator<(uint32_t priority) const{
        return prio < priority;
    }
};
class fair_shared_mutex{
    public:

    void lock(uint32_t priority = 0){
        //debug
        auto threadID = std::this_thread::get_id();
        //
        std::unique_lock<std::mutex> lock(_internalMtx);
        auto& myPriority = *(_priorities.emplace(priority).first);
        if (_lockOwned){
            myPriority.waiting++;
            myPriority.cv.wait(lock);
        }
        while (_lockOwned){ 
            // this while may be useless depending if we decide to use 2 cv one for readers and one for writers
            // but if we do it like that then we also need to decide which of the 2 get the notify first which I want to avoid.
           myPriority.cv.wait(lock);
        }
        _lockOwned = true;
    }

    void unlock(){
        //debug
        auto threadID = std::this_thread::get_id();
        //
        {
		std::lock_guard<std::mutex> lock(_internalMtx);
		_lockOwned = false;
		}
        for (auto& p : _priorities){
            if (p.waiting > 0){
                p.waiting--;
                p.cv.notify_all();
                //p.cv.notify_one();
                break;
            }
        }
    }
    //void lock_shared(uint32_t priority = 0){

    //}
    //void unlock_shared(){

    //}

    private:
    std::mutex _internalMtx;
    std::set<threadPriority> _priorities;
    bool _lockOwned{};
    //bool _writing{};
    //uint32_t _readers{};
        //

};
