#include <boost/fairness.hpp>

static boost::fairness::priority_mutex<4> ms;

void threadFunction(int i) {
    if (ms.try_lock(i)){
        ms.unlock();
        std::cout << i << std::endl;
        return;
    }
    std::cout << -1 << std::endl;

}
int main()
{
    threadFunction(2);
    return 0;
}