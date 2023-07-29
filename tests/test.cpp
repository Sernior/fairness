#include <gtest/gtest.h>
#include <thread>
#include <fair_shared_mutex.h>

const int THREAD_COUNT = 5; // Number of threads in the array
std::mutex WaitingMtx; // Mutex to synchronize access to the waitingMutexVector
std::mutex AcquireMtx; // Mutex to synchronize access to the acquireMutexVector
std::mutex ReleaseMtx; // Mutex to synchronize access to the releaseMutexVector

std::vector<std::string> waitingMutexVector; // The vector of strings, used to log the wait of the threads
std::vector<std::string> acquireMutexVector; // The vector of strings, used to log the acquire of the threads
std::vector<std::string> releaseMutexVector; // The vector of strings, used to log the release of the threads


/** Define a custom state struct
 *  uint32_t    id: is the id of the thread;
 *  uint32_t    state: is the state of the thread ( 0: error, 
 *                                                  1: inactive, 
 *                                                  2: waiting, 
 *                                                  3: not critical section processing,
 *                                                  4: critical section processing
 *                                                );
*/
struct ThreadState {
    uint32_t id;
    uint32_t state = 1;

    // Add any other data you want to associate with each thread
    // For example: int progress, std::string name, etc.
};

// Function to be executed by each thread
void threadFunction(ThreadState& state, uint32_t threadPriority = 0, uint32_t preLockTimer = 0, uint32_t criticalSectionTimer = 0, uint32_t postUnlockTimer = 0) {
    // Modify the state of the thread (this is a simple example)
    state.state = 3;
    std::this_thread::sleep_for(std::chrono::microseconds(preLockTimer));
    
    std::unique_lock<std::mutex> waitingLock(WaitingMtx);
    state.state = 2;
    myVector.push_back("Thread " + std::to_string(state.id) + " is in waiting.");
    waitingLock.unlock();

    m.lock(threadPriority);

    std::unique_lock<std::mutex> AcquireLock(AcquireMtx);
    state.state = 4;
    myVector.push_back("Thread " + std::to_string(state.id) + " is in acquire.");
    AcquireLock.unlock();

    std::this_thread::sleep_for(std::chrono::microseconds(criticalSectionTimer));

    m.unlock();
    std::unique_lock<std::mutex> ReleaseLock(ReleaseMtx);
    state.state = 3;
    myVector.push_back("Thread " + std::to_string(state.id) + " is in release.");
    ReleaseLock.unlock();

    std::this_thread::sleep_for(std::chrono::microseconds(postUnlockTimer));

    state.state = 1;
}

TEST(FairSharedMutexTest, Scenario_1) {
    // EXPECT_EQ(2, 5);
    // EXPECT_EQ(5, 0);
}

int main(int argc, char* argv[]) {
    ///////////////////////
    
    // Create an array of thread states
    ThreadState threadStates[THREAD_COUNT];

    // Create an array of threads
    std::thread threads[THREAD_COUNT];

    // Start the threads
    for (int i = 0; i < THREAD_COUNT; ++i) {
        threadStates[i].id = i;
        threadStates[i].isRunning = false;

        threads[i] = std::thread(threadFunction, std::ref(threadStates[i]));
    }

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
