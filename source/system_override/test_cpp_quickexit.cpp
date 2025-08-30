#include <cstdlib>
#include <iostream>
#include <process.h>

template<int N>
class Tester
{
public:
    Tester()  { std::cerr << "Tester ctor #" << N << std::endl; }
    ~Tester() { std::cerr << "Tester dtor #" << N << std::endl; }
};

static Tester<1> static_tester; // Destructor not called

template<int N>
static void quick_exit_handler(void)
{
    std::cerr << "quick_exit handler #" << N << std::endl; // flush is intended
}
 
static void at_exit_handler(void)
{
    std::cerr << "at_exit handler\n";
}
 
int main(void)
{
    Tester<2> automatic_tester; // Destructor not called
	
    if (std::at_quick_exit(quick_exit_handler<1>) ||
        std::at_quick_exit(quick_exit_handler<2>))
    {
        std::cerr << "Registration failed\n";
        return EXIT_FAILURE;
    }
 
    std::atexit(at_exit_handler); // the handler will not be called
 
	std::cerr << "testing...\n";
	
    struct R { 
		~R() { 
			std::cerr << "R destructor\n"; 
		} 
	} resource;
 
    /*...*/
 
    std::quick_exit(EXIT_SUCCESS);
}
