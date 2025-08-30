#include <csignal>
#include <cstdlib>
#include <iostream>

class Tester
{
public:
    Tester()  { std::cout << "Tester ctor\n"; }
    ~Tester() { std::cout << "Tester dtor\n"; }
};

static Tester static_tester; // Destructor not called
 
static void signal_handler(int signal) 
{
    if (signal == SIGABRT)
        std::cerr << "SIGABRT received\n";
    else
        std::cerr << "Unexpected signal " << signal << " received\n";
    std::_Exit(EXIT_FAILURE);
}
 
int main(void)
{
    Tester automatic_tester; // Destructor not called
 
    // Setup handler
    auto previous_handler = std::signal(SIGABRT, signal_handler);
    if (previous_handler == SIG_ERR)
    {
        std::cerr << "Setup failed\n";
        return EXIT_FAILURE;
    }
 
    std::abort(); // Raise SIGABRT
}
