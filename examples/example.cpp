#include <boost/fairness.hpp>
#include <iostream>

static boost::fairness::priority_mutex<4> ms;

void threadFunction(int p) {
    {
        boost::fairness::unique_lock ul(ms, p);
        std::cout << p << std::endl;
    }
}
int main()
{
    threadFunction(2);
    return 0;
}