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

static Tester<1> static_tester; // Destructor will be called
 
int main(void)
{
    Tester<2> automatic_tester; // Destructor not called
 
	std::cerr << "testing...\n";
	
    struct R { 
		~R() { 
			std::cerr << "R destructor\n"; 
		} 
	} resource;
 
    /*...*/

	std::exit(0);
}
