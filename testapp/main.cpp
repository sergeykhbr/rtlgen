// syscv.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <api.h>

using namespace std;
using namespace sysvc;

int main()
{
    ModuleObject *top = SCV_new_module(0, "top");
    IoObject *i_a = SCV_new_module_io(top, IO_DIR_INPUT, "i_a", 8, " signal A");
    IoObject *i_b = SCV_new_module_io(top, IO_DIR_INPUT, "i_b", 1, " signal B");
    IoObject *i_c = SCV_new_module_io(top, IO_DIR_OUTPUT, "o_c", 64, " signal C");



    AttributeType gencfg(Attr_Dict);
    gencfg["FileMaxLength"].make_int64(1024*1024);
    gencfg["OutputDir"].make_string("e:/Projects/delit/generate");

    SCV_generate_systemc(&gencfg, top);

	cout << "Hello CMake." << endl;
	return 0;
}
