// syscv.cpp : Defines the entry point for the application.
//

#include <iostream>
#include "riverlib/river_top.h"

using namespace std;
using namespace sysvc;

int main()
{
    TopObject *top = new TopObject();

    AttributeType gencfg(Attr_Dict);
    gencfg["FileMaxLength"].make_int64(1024*1024);
    gencfg["OutputDir"].make_string("e:/Projects/delit/generate");

    SCV_generate_systemc(&gencfg, top);

	cout << "Hello CMake." << endl;
	return 0;
}
