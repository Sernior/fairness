
#include <mutex>
#include <condition_variable>
#include <set>
#include <stdexcept>

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

    typedef uint32_t _Priority_t;
    typedef uint16_t _Read_cnt_t;
    typedef uint16_t _Write_cnt_t;
    typedef uint32_t _TotRead_cnt_t;
    static constexpr _TotRead_cnt_t _Max_readers = _TotRead_cnt_t(-1);

    struct threadPriority{
        _Priority_t prio{};
        mutable _Write_cnt_t writers_waiting{};
        mutable _Read_cnt_t readers_waiting{};
        mutable std::condition_variable writer_queue;
        mutable std::condition_variable reader_queue;
        threadPriority(_Priority_t priority){
            prio = priority;
        }
        bool operator<(threadPriority const& other) const{
            return prio < other.prio;
        }
        bool operator<(_Priority_t priority) const{
            return prio < priority;
        }
        bool operator==(threadPriority const& other) const{
            return prio == other.priority;
        }
        bool operator==(_Priority_t priority) const{
            return prio == priority;
        }
    };

    enum class ReaderWriterPriority{
        reader,
        writer
    };

    template<ReaderWriterPriority P = ReaderWriterPriority::writer>
    class fair_shared_mutex{
        public:

        void lock(_Priority_t priority = 0){
            #ifdef DEBUGGIN_FSM
            auto threadID = std::this_thread::get_id();
            #endif

            std::unique_lock<std::mutex> lock(_internalMtx);
            auto& myPriority = *(_priorities.emplace(priority).first);

            while (_lockOwned || _totalCurrentReaders > 0){ 
                myPriority.writers_waiting++;
                myPriority.writer_queue.wait(lock);
                myPriority.writers_waiting--;
            }

            _lockOwned = true;

            //while (_totalCurrentReaders > 0) // can we do this? is it good? (P0)
            //    myPriority.writer_queue.wait(lock);
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
        void lock_shared(_Priority_t priority = 0){
            #ifdef DEBUGGIN_FSM
            auto threadID = std::this_thread::get_id();
            #endif

            std::unique_lock<std::mutex> lock(_internalMtx);
            auto& myPriority = *(_priorities.emplace(priority).first);

            #ifdef DEBUGGIN_FSM
            if (_lockOwned && _totalCurrentReaders > 0){// this could actually be good if we put another check in the lock() for readers > 0 (P0)
            //I ultimatly think this condition should be kept as an invalid state as there is no guarantee that by fishing in the writer_queue
            //we would catch again the right thread (the one who got to the second while) on the first try... thus causing many writers to wake up
            //for no reason.
                throw std::exception("The lock is being owned uniquely while there are readers.");
            }
            if (_totalCurrentReaders == _Max_readers){
                throw std::exception("Unsupported number of readers.");
            }
            #endif

            // do we starve writers? maybe we need to check the currently served priority or maybe we could just accept that if
            // a thread A comes and tries to acquire the shared_lock while others are reading... even if A has lower priority then the current ones
            // we could just let him pass anyways.
            while (_lockOwned || _totalCurrentReaders == _Max_readers){
                myPriority.readers_waiting++;
                myPriority.writer_queue.wait(lock);
                myPriority.readers_waiting--;
            }

            _totalCurrentReaders++;

        }
        //void unlock_shared(){

        //}

        private:
        std::mutex _internalMtx;
        std::set<threadPriority> _priorities;
        bool _lockOwned{};
        _TotRead_cnt_t _totalCurrentReaders{};

    };
}

