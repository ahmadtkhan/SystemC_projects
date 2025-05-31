#include <stdio.h>
#include <systemc.h>
#include "csa.h"
#include "cpa.h"

void eight_multiplier();

void eight_multiplier(){

	
	sc_uint<8> a_val, b_val;

	std::cout << "Enter the value of A, and B for multiplication (0-127): " << std::endl;

	std::cin >> a_val >> b_val;

	sc_clock clk("clk",10,SC_NS,0.5); 

	static sc_signal<bool> zero_const;	

	sc_trace_file *tf;                  // Create VCD file for tracing
	sc_signal<bool> csa_a, csa_b, csa_cin, csa_s, csa_co, csa_so;      //Declare signals
	sc_signal<bool> cpa_a, cpa_b, cpa_cin, cpa_co, cpa_so;
	
	char trace_csa_a[64];
	char trace_csa_b[64];
	char trace_csa_cin[64];
	char trace_csa_s[64];
	char trace_csa_co[64];
	char trace_csa_so[64];

	char trace_cpa_a[64];
	char trace_cpa_b[64];
	char trace_cpa_cin[64];
	char trace_cpa_co[64];
	char trace_cpa_so[64];

	sc_signal<bool> A_bits[8], B_bits[8];	

	for(int i = 0; i < 8; i++){
		A_bits[i].write((a_val >> i) & 1);
		B_bits[i].write((b_val >> i) & 1);
	} 

	sc_signal<bool> sum_sig[8][8], carry_sig[8][8];
	
	csa* csa_array[7][8];
	cpa* cpa_array[8];

	tf = sc_create_vcd_trace_file("trace_file");
    tf->set_time_unit(1, SC_NS);
	sc_trace(tf, clk, "clk");

	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			if(i < 7){
				char name[50];
				sprintf(name, "csa_%d_%d", i, j);
				csa_array[i][j] = new csa(name);
				csa_array[i][j]->clk(clk);
				
				csa_array[i][j]->a(A_bits[i]);
				csa_array[i][j]->b(B_bits[j]);	
				sprintf(trace_csa_a, "csa_%d_%d_a", i, j);
				sprintf(trace_csa_b, "csa_%d_%d_b", i, j);
				sc_trace(tf, csa_array[i][j]->a, trace_csa_a);
				sc_trace(tf, csa_array[i][j]->b, trace_csa_b);			

				if(i == 0){
					zero_const.write(false);
					csa_array[i][j]->s(zero_const);
					csa_array[i][j]->c_in(zero_const);
				}else{
					if(j == 7){
						zero_const.write(false);
						csa_array[i][j]->s(zero_const);
						csa_array[i][j]->c_in(carry_sig[i-1][j]);
					}else{
						csa_array[i][j]->s(sum_sig[i-1][j+1]);
						csa_array[i][j]->c_in(carry_sig[i-1][j]);
					}
				}
				
				csa_array[i][j]->so(sum_sig[i][j]);
                csa_array[i][j]->co(carry_sig[i][j]);

				sprintf(trace_csa_cin, "csa_%d_%d_cin", i, j);
				sprintf(trace_csa_s, "csa_%d_%d_s", i, j);
				sprintf(trace_csa_co, "csa_%d_%d_co", i, j);
				sprintf(trace_csa_so, "csa_%d_%d_so", i, j);
				sc_trace(tf, csa_array[i][j]->c_in, trace_csa_cin);
				sc_trace(tf, csa_array[i][j]->s, trace_csa_s);
				sc_trace(tf, csa_array[i][j]->co, trace_csa_co);
				sc_trace(tf, csa_array[i][j]->so, trace_csa_so);			
			} else{
				char name[50];
				sprintf(name, "cpa_%d_%d", i, j);
				cpa_array[j] = new cpa(name);	
				cpa_array[j]->clk(clk);
				
				if(j == 0){
					zero_const.write(false);
					cpa_array[j]->a(sum_sig[i-1][j+1]);
					cpa_array[j]->b(carry_sig[i-1][j]);
					cpa_array[j]->c_in(zero_const);
				} else if(j == 7){
					zero_const.write(false);
					cpa_array[j]->a(carry_sig[i-1][j]);
					cpa_array[j]->b(carry_sig[i][j-1]);
					cpa_array[j]->c_in(zero_const);
				}else{
					zero_const.write(false);
					cpa_array[j]->a(sum_sig[i-1][j+1]);
					cpa_array[j]->b(carry_sig[i-1][j]);
					cpa_array[j]->c_in(carry_sig[i][j-1]);
				}
				cpa_array[j]->co(carry_sig[i][j]);
				cpa_array[j]->so(sum_sig[i][j]);
			}
		}
	}	
	sc_start(100, SC_NS);

	sc_uint<16> product_val = 0;
	
	for (int i = 0; i < 8; i++) {
		product_val[i] = sum_sig[i][0].read();
	}

	for (int i = 1; i < 8; i++) {
		product_val[i+7] = sum_sig[7][i].read();
	}
	product_val[15] = carry_sig[7][7].read();
    std::cout << "\nComputed product = " << product_val
              << " (decimal), expected = " << (a_val * b_val)
              << std::endl << std::endl;
	
	sc_close_vcd_trace_file(tf);

}

int sc_main(int argc, char* argv[]){

	eight_multiplier(); 

	return 0;                         
}
