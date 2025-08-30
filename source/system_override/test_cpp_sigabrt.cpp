#include <csignal>
#include <cstdlib>
#include <iostream>
#include <cassert>

template<int N>
class Tester
{
public:
    Tester()  { std::cerr << "Tester ctor #" << N << std::endl; }
    ~Tester() { std::cerr << "Tester dtor #" << N << std::endl; }
};

static Tester<1> static_tester; // Destructor not called
 
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
    Tester<2> automatic_tester; // Destructor not called
 
    // Setup handler
    auto previous_handler = std::signal(SIGABRT, signal_handler);
    if (previous_handler == SIG_ERR)
    {
        std::cerr << "Setup failed\n";
        return EXIT_FAILURE;
    }

	std::cerr << "testing...\n";
	
    struct R { 
		~R() { 
			std::cerr << "R destructor\n"; 
		} 
	} resource;
 
    /*...*/

	std::raise(SIGABRT); // Raise SIGABRT
    //std::abort(); // <-- this one only raises SIGABRT in POSIX boxes, so we do it explicitly in the line above.

	assert(false && "Should never get here!");
	return 666;
}
