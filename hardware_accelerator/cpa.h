#ifndef CPA_H
#define CPA_H

#include <systemc.h>

SC_MODULE(cpa){
	sc_in<bool> clk; 
	sc_in<bool> a;
	sc_in<bool> b;
	sc_in<bool> c_in;
	sc_out<bool> co;
	sc_out<bool> so;
	
	void cpa_method();
	
	SC_CTOR(cpa){
		SC_METHOD(cpa_method);
		dont_initialize();
		sensitive << clk.pos();
	}
};

#endif
