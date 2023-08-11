#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <set>

#define DEBUGGIN_FSM

#ifdef DEBUGGIN_FSM
#include <stdexcept>
#endif
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
            return prio == other.prio;
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

            //when we first emplace a priority that does not exist we are not checking
            //the existance of lower priorities writers_wating.
            auto* max_p = _find_first_priority();

            while (_lockOwned || _totalCurrentReaders > 0 || (max_p != nullptr) && (*max_p < myPriority) ){ 
                myPriority.writers_waiting++;//overflow risk? we could leave it unchecked I doubt anyone will ever use more than 60k threads
                myPriority.writer_queue.wait(lock);
                max_p = _find_first_priority();
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

            const threadPriority* p;

            {
            std::lock_guard<std::mutex> lock(_internalMtx);
            _lockOwned = false;
            p = _find_first_priority();
            }

            if (p == nullptr) return; // there is no one waiting

            if (P == ReaderWriterPriority::writer){
                if (p->writers_waiting > 0){
                    p->writer_queue.notify_one();
                }
                else if (p->readers_waiting > 0){
                    p->reader_queue.notify_all();
                }
            }
            else {
                if (p->readers_waiting > 0){
                    p->reader_queue.notify_all();
                }
                else if (p->writers_waiting > 0){
                    p->writer_queue.notify_one();
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
            if (_lockOwned && _totalCurrentReaders > 0){
                throw std::exception("The lock is being owned uniquely while there are readers.");
            }
            if (_totalCurrentReaders == _Max_readers){
                throw std::exception("Unsupported number of readers.");
            }
            #endif

            // do we starve writers? maybe we need to check the currently served priority or maybe we could just accept that if
            // a thread A comes and tries to acquire the shared_lock while others are reading... even if A has lower priority then the current ones
            // we could just let him pass anyways... No I think I want to check the currently served priority and not risk starving writers with higher prioirity
            while (_lockOwned || _totalCurrentReaders == _Max_readers){
                myPriority.readers_waiting++;//overflow risk here? we could leave it unchecked I doubt anyone will ever use more than 60k threads
                myPriority.reader_queue.wait(lock);
                myPriority.readers_waiting--;
            }

            _totalCurrentReaders++;

        }

        void unlock_shared(){
            #ifdef DEBUGGIN_FSM
            auto threadID = std::this_thread::get_id();
            #endif

            const threadPriority* p;

            {
            std::lock_guard<std::mutex> lock(_internalMtx);
            _totalCurrentReaders--;
            p = _find_first_priority();
            }

            if (p == nullptr) return; // there is no one waiting

            if (P == ReaderWriterPriority::writer){
                if (p->writers_waiting > 0)
                    p->writer_queue.notify_one();
                else if (p->readers_waiting > 0)
                    p->reader_queue.notify_all();
            }
            else {
                if (p->readers_waiting > 0)
                    p->reader_queue.notify_all();
                else if (p->writers_waiting > 0)
                    p->writer_queue.notify_one();
            }
        }

        private:
        std::mutex _internalMtx;
        std::set<threadPriority> _priorities;
        bool _lockOwned{};// probably this will have to become a threadID when we implement recursive locks
        _TotRead_cnt_t _totalCurrentReaders{};

        //Will find the first priority where there is at least 1 thread waiting
        const threadPriority* _find_first_priority(){
            for (auto& p : _priorities){
                if (P == ReaderWriterPriority::reader){
                    if (p.readers_waiting > 0 || p.writers_waiting > 0)
                        return &p;
                }
                else{
                    if (p.writers_waiting > 0 || p.readers_waiting > 0)
                        return &p;
                }
            }
            return nullptr;
        }
    };
}
