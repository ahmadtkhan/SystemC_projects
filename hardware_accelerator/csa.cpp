#include "csa.h"

void csa::csa_method()
{
	bool ab_val = a.read() & b.read();
	
	bool s_in  = s.read();
    bool c_val = c_in.read();

    bool so_val = ab_val ^ s_in ^ c_val;
    bool co_val = (ab_val & s_in) | (ab_val & c_val) | (s_in & c_val);

    so.write(so_val);
    co.write(co_val);

}
