#include <iostream>
#include <mutex>
#include <condition_variable>
#include <set>

/*
Pro and cons of having 2 condition_variable

Pro:
- when there is an unlock or the readers count reach 0 we can selectively notify all readers or one writer so not to unlock threads for no reason.
- would most likely simplify the lock and shared lock as we don`t have to take into account the case "I got notified for no reason and I need to reenter the lock".
- would allow us more control.

Con:
- we need to decide which get selected first creating an implicit fairness policy (if we have two cv we need to choose which we notify first instead of delegating this decision to the scheduler).
- more memory wasted as we are creating 1 more cv for each threadPriority.
*/

#define DEBUGGIN_FSM
namespace fsm{
    struct threadPriority{
        uint32_t prio{};
        mutable uint32_t writers_waiting{};
        mutable uint32_t readers_waiting{};
        mutable std::condition_variable writer_queue;
        mutable std::condition_variable reader_queue;
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

    enum class ReaderWriterPriority{
        reader,
        writer
    };

    template<ReaderWriterPriority P = ReaderWriterPriority::writer>
    class fair_shared_mutex{
        public:

        void lock(uint32_t priority = 0){
            #ifdef DEBUGGIN_FSM
            auto threadID = std::this_thread::get_id();
            #endif

            std::unique_lock<std::mutex> lock(_internalMtx);
            auto& myPriority = *(_priorities.emplace(priority).first);
            while (_lockOwned){ 
                myPriority.writers_waiting++;
                myPriority.writer_queue.wait(lock);
                myPriority.writers_waiting--;
            }
            _lockOwned = true;
        }

        void unlock(){
            #ifdef DEBUGGIN_FSM
            auto threadID = std::this_thread::get_id();
            #endif

            {
            std::lock_guard<std::mutex> lock(_internalMtx);
            _lockOwned = false;
            }
            for (auto& p : _priorities){
                if constexpr (P == ReaderWriterPriority::writer){
                    if (p.writers_waiting > 0){
                        p.writer_queue.notify_one();
                        break;
                    }
                    else if (p.readers_waiting > 0){
                        p.reader_queue.notify_all();
                        break;
                    }
                }
                else {
                    if (p.readers_waiting > 0){
                        p.reader_queue.notify_all();
                        break;
                    }
                    else if (p.writers_waiting > 0){
                        p.writer_queue.notify_one();
                        break;
                    }
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

        //this variable might be usefull in the future if we want to make the mutex also unlock lower priority readers
        //uint32_t _totalCurrentReaders{};

    };
}

