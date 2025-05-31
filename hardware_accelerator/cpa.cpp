#include "cpa.h"

void cpa::cpa_method()
{
	bool a_val = a.read();
	bool b_val = b.read();
	bool c_val = c_in.read();

	bool so_val = a_val ^ b_val ^ c_val;
	bool co_val = (a_val & b_val) | (a_val & c_val) | (b_val & c_val);
	so.write(so_val);
	co.write(co_val);
}
