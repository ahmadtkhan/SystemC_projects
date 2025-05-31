#ifndef CSA_H
#define CSA_H

#include <systemc.h>

SC_MODULE(csa){
	sc_in<bool> clk; 
	sc_in<bool> a;
	sc_in<bool> b;
	sc_in<bool> c_in;
	sc_in<bool> s;
	sc_out<bool> co;
	sc_out<bool> so;

	void csa_method();
	
	SC_CTOR(csa){
		SC_METHOD(csa_method);
		dont_initialize();
		sensitive << clk.pos();
	}
};

#endif
