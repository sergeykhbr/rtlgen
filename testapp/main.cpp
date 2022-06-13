// syscv.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <api.h>

using namespace std;


int main()
{
    sysvc::vc_module cls("cls");
    cls.register_input("i_a", 8);
    cls.register_output("o_b", 8);




	cout << "Hello CMake." << endl;
	return 0;
}
