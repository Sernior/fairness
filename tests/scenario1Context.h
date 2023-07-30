#include <fair_shared_mutex.h>
#include <vector>

const int THREAD_COUNT = 5; // Number of threads in the array
static std::mutex AcquireMtx; // Mutex to synchronize access to the acquireMutexVector
static std::mutex ReleaseMtx; // Mutex to synchronize access to the releaseMutexVector
static fsm::fair_shared_mutex m;

static std::vector<int> acquireMutexVector; // The vector of int, used to log the acquire of the threads
static std::vector<int> releaseMutexVector; // The vector of int, used to log the release of the threads

static std::vector<int> expectedAcquireMutexVector1{
    0, 4, 2, 1, 3
}; // The vector of int, used to compare the acquireMutexVector vector
static std::vector<int> expectedAcquireMutexVector2{
    0, 2, 4, 1, 3
}; // The vector of int, used to compare the acquireMutexVector vector


static std::vector<int> expectedReleaseMutexVector1{
    0, 4, 2, 1, 3
}; // The vector of int, used to compare the releaseMutexVector vector
static std::vector<int> expectedReleaseMutexVector2{
    0, 2, 4, 1, 3
}; // The vector of int, used to compare the releaseMutexVector vector

static std::vector<int> priorityVector =        {0,     1,    0,    2,    0};
static std::vector<int> preLockTimerVector =    {0,     200,  300,  100,  200};
static std::vector<int> criticalSectionVector = {2000,  400,  400,  200,  300};
static std::vector<int> postLockTimerVector =   {0,     200,  300,  100,  200};

enum class thread_state_t {
    ERROR,
    INACTIVE,
    WAITING,
    RUNNING_NOT_CRITICAL,
    RUNNING_CRITICAL
};
struct ThreadState {
    uint32_t id;
    thread_state_t state = thread_state_t::INACTIVE;

    // Add any other data you want to associate with each thread
    // For example: int progress, std::string name, etc.
};

// Create an array of thread states
static std::vector<ThreadState> threadStates(THREAD_COUNT);

// Function to be executed by each thread
void threadFunction(int i) {
    // Modify the state of the thread (this is a simple example)
    threadStates[i].state = thread_state_t::RUNNING_NOT_CRITICAL;
    std::this_thread::sleep_for(std::chrono::microseconds(preLockTimerVector[i]));

    m.lock(priorityVector[i]);

    std::unique_lock<std::mutex> AcquireLock(AcquireMtx);
    threadStates[i].state = thread_state_t::RUNNING_CRITICAL;
    acquireMutexVector.push_back(threadStates[i].id);
    AcquireLock.unlock();

    std::this_thread::sleep_for(std::chrono::microseconds(criticalSectionVector[i]));

    m.unlock();
    
    std::unique_lock<std::mutex> ReleaseLock(ReleaseMtx);
    threadStates[i].state = thread_state_t::RUNNING_NOT_CRITICAL;
    releaseMutexVector.push_back(threadStates[i].id);
    ReleaseLock.unlock();

    std::this_thread::sleep_for(std::chrono::microseconds(postLockTimerVector[i]));

    threadStates[i].state = thread_state_t::INACTIVE;
}