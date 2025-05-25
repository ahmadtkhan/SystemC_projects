#ifndef FDCT_H
#define FDCT_H

#include "systemc.h"
#include <math.h>

struct fdct : sc_module {
    FILE *infile;  

    sc_out<double> out64[8][8];    // the DCT-transformed 8x8 block
    sc_in<double>  fcosine[8][8];  // cosine table input
    sc_in<bool>    clk;            // clock signal

    char input_data[8][8]; // the data read from the input file

    SC_HAS_PROCESS(fdct);
    fdct(sc_module_name name, FILE *fp): sc_module(name), infile(fp){
		// make read_data method sensitive to the positive clock edge, and
		// the calculate_dct method sensitive to the negative clock edge
		// this way, the entire read and performing dct takes only one clock cycle
		// as apposed to two
        SC_METHOD(read_data); // define read_data as a method
        dont_initialize();
        sensitive << clk.pos();

        SC_METHOD(calculate_dct); // define calculate_dct as a method
        dont_initialize();
        sensitive << clk.neg();
    }

    void read_data( void );
    void calculate_dct( void );
};

#endif

