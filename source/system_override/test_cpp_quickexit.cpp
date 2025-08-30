#include <cstdlib>
#include <iostream>
 
template<int N>
static void quick_exit_handler(void)
{
    std::cout << "quick_exit handler #" << N << std::endl; // flush is intended
}
 
static void at_exit_handler(void)
{
    std::cout << "at_exit handler\n";
}
 
int main(void)
{
    if (std::at_quick_exit(quick_exit_handler<1>) ||
        std::at_quick_exit(quick_exit_handler<2>))
    {
        std::cerr << "Registration failed\n";
        return EXIT_FAILURE;
    }
 
    std::atexit(at_exit_handler); // the handler will not be called
 
    struct R { 
		~R() { 
			std::cout << "destructor\n"; 
		} 
	} resource;
 
    /*...*/
 
    std::quick_exit(EXIT_SUCCESS);
}
