#include <fair_shared_mutex.h>
#include <vector>
#include <thread>
namespace scenario2{
    /*
    
    Tests like this one do not work. We can`t rely on the scheduler and we need an artificial abstracion of "Time" to properly test the mutex.
    We should use something to guarantee that the computation of a thread reach our expected state and using time in its raw form like "microseconds"
    simply does not provide us any guarantee.
    threadFunction should have systems to sync the threads... and chrono::sleep_for is not valid.
    
    */

    const int THREAD_COUNT = 8; // Number of threads in the array
    static std::mutex AcquireMtx; // Mutex to synchronize access to the acquireMutexVector
    static std::mutex ReleaseMtx; // Mutex to synchronize access to the releaseMutexVector
    static fsm::fair_shared_mutex m;

    static std::vector<std::thread::id> acquireMutexVector; // The vector of int, used to log the acquire of the threads
    static std::vector<std::thread::id> releaseMutexVector; // The vector of int, used to log the release of the threads

    static std::vector<int> expectedAcquireMutexVector1{
        0, 1, 2, 3, 4, 5, 6, 7
    }; // The vector of int, used to compare the acquireMutexVector vector
    static std::vector<int> expectedAcquireMutexVector2{
        1, 0, 2, 3, 4, 5, 6, 7
    }; // The vector of int, used to compare the acquireMutexVector vector


    static std::vector<int> expectedReleaseMutexVector1{
        0, 1, 2, 3, 4, 5, 6, 7
    }; // The vector of int, used to compare the releaseMutexVector vector
    static std::vector<int> expectedReleaseMutexVector2{
        1, 0, 2, 3, 4, 5, 6, 7
    }; // The vector of int, used to compare the releaseMutexVector vector

    static std::vector<int> priorityVector =        {0,     1,    2,    3,    5,    6,    7,    8};
    static std::vector<int> preLockTimerVector =    {0,     70,  30,  10,  120,  30,  10,  20};
    static std::vector<int> criticalSectionVector = {200,  40,  40,  20,  30,  30,  10,  20};
    static std::vector<int> postLockTimerVector =   {0,     20,  30,  10,  20,  30,  10,  20};

    enum class thread_state_t {
        ERROR,
        INACTIVE,
        WAITING,
        RUNNING_NOT_CRITICAL,
        ABOUT_TO_AQUIRE_LOCK,
        RUNNING_CRITICAL
    };
    struct ThreadState {
        thread_state_t state = thread_state_t::INACTIVE;

        // Add any other data you want to associate with each thread
        // For example: int progress, std::string name, etc.
    };

    // Create an array of thread states
    static std::vector<ThreadState> threadStates(THREAD_COUNT);

    // Function to be executed by each thread
    void threadFunction(const int i) {
        // Modify the state of the thread (this is a simple example)
        threadStates[i].state = thread_state_t::RUNNING_NOT_CRITICAL;

        //while ( i != 0 && threadStates[i-1].state == thread_state_t::INACTIVE)continue;// sync the threads

        //for (int j = i; j != 0; j--){
        //    while ( threadStates[j-1].state == thread_state_t::INACTIVE)continue;// sync the threads
        //}

        std::this_thread::sleep_for(std::chrono::microseconds(preLockTimerVector[i]));


        while ( i != 0 && threadStates[i-1].state != thread_state_t::ABOUT_TO_AQUIRE_LOCK)continue;// sync the threads


        threadStates[i].state = thread_state_t::ABOUT_TO_AQUIRE_LOCK;

        m.lock(priorityVector[i]);

        std::unique_lock<std::mutex> AcquireLock(AcquireMtx);
        threadStates[i].state = thread_state_t::RUNNING_CRITICAL;
        acquireMutexVector.push_back(std::this_thread::get_id());
        AcquireLock.unlock();

        std::this_thread::sleep_for(std::chrono::microseconds(criticalSectionVector[i]));
        
        std::unique_lock<std::mutex> ReleaseLock(ReleaseMtx);
        threadStates[i].state = thread_state_t::RUNNING_NOT_CRITICAL;
        releaseMutexVector.push_back(std::this_thread::get_id());
        ReleaseLock.unlock();

        m.unlock();

        std::this_thread::sleep_for(std::chrono::microseconds(postLockTimerVector[i]));

        threadStates[i].state = thread_state_t::INACTIVE;
    }
}