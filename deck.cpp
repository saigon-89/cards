#include <iostream>

class deck
{
    
    public:
    char foo()
{
    static char *str = "Asshole!";
    return *str++;   
}

};



int main()
{
    deck a,b;
    std::cout << a.foo() << b.foo() << a.foo() << b.foo();
    return 0;
}