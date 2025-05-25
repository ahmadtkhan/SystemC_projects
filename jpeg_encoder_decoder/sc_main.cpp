#include "systemc.h"
#include "functions.h"
#include "fdct.h"
#include "idct.h"
//#include inverse module

int sc_main(int argc, char* argv[]) {
    if (argc < 4) {
        fprintf(stderr, 
            "usage: ./sc_jpeg.x [input file] [output file] [(c)ompress|(d)ecompress]\n");
        return 1;
    }

    FILE *input  = fopen(argv[1], "rb");
    FILE *output = fopen(argv[2], "wb");

    char choice = argv[3][0];

    // Read/write header
    write_read_header(input, output);

    // Create the cosine table
    double cos_tbl[8][8];
    make_cosine_tbl(cos_tbl);

    // Create signals for FDCT
    sc_signal<double> dct_data[8][8]; // signal to the dc transformed data
    sc_signal<double> cosine_tbl[8][8]; // signal for the cosine table values
    sc_signal<bool>   clk1("clk1"); // clock signal for FDCT (also need a 2nd clock for IDCT)
	sc_signal<bool>   clk2("clk2");
	//Decompression signals and variables
    sc_signal<double> idct_in[8][8];
    sc_signal<double> idct_out[8][8];

    // Copy the cos table into the signals
    for(int i=0; i<8; i++) {
        for(int j=0; j<8; j++) {
            cosine_tbl[i][j].write(cos_tbl[i][j]);
        }
    }

    // Instantiate FDCT with file pointer in the constructor
    fdct FDCT1("FDCT1", input);
    FDCT1.clk(clk1);
	
	// make the cosine table
    for(int i=0; i<8; i++) {
        for(int j=0; j<8; j++) {
            FDCT1.fcosine[i][j](cosine_tbl[i][j]);
            FDCT1.out64[i][j](dct_data[i][j]);
        }
    }
	//binds ports - idct
    idct IDCT1("IDCT1");
    IDCT1.clk(clk2);

    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            IDCT1.fcosine[i][j](cosine_tbl[i][j]);
            IDCT1.in64[i][j](idct_in[i][j]);
            IDCT1.out64[i][j](idct_out[i][j]);
        }
    }

	// because compression and decompression are two different processes, we must use
	// two different clocks, to make sure that when we want to compress, we only compress
	// and dont decompress by mistake
    sc_start(SC_ZERO_TIME);
    if (choice == 'c' || choice == 'C') { // for compression
        while(true) {
            
			clk1.write(1);		// convert the clock to high
			sc_start(10, SC_NS);	// cycle the high for 10 nanoseconds
			clk1.write(0);		// start the clock as low
			sc_start(10, SC_NS);	// cycle the low for 10 nanoseconds

            // We need to check if we hit EOF in fdct::read_data
            // E.g. we can check feof(input) or track partial read in FDCT
            if (feof(input)) {
                break;
            }

            // read back signals from module
            double block[8][8];
            for (int u=0; u<8; u++){
                for(int v=0; v<8; v++){
                    block[u][v] = dct_data[u][v].read();
                }
            }
			// zigzag and quantize the outputted data - will write out to file (see functions.h)
            zigzag_quant(block, output);
        }
    }
    else if (choice == 'd' || choice == 'D') {
        while(!feof(input)) {
            double block[8][8];
			//unzigzag and inverse quatize input file and result will be placed in data
            unzigzag_iquant(block, input);
            if (feof(input)) break;

            //write unzigzag data to ports
            for (int u=0; u<8; u++){
                for(int v=0; v<8; v++){
                    idct_in[u][v].write(block[u][v]);
                }
            }

			clk2.write(1);		// convert the clock to high
			sc_start(10, SC_NS);	// cycle the high for 10 nanoseconds
			clk2.write(0);		// start the clock as low
			sc_start(10, SC_NS);	// cycle the low for 10 nanoseconds

            //read idct data from ports & write to output file
            for (int x=0; x<8; x++){
                for(int y=0; y<8; y++){
                    double val = idct_out[x][y].read();
                    if(val<0) val=0;
                    if(val>255) val=255;
                    // write to file
                    fputc((unsigned char)(val+0.5), output);
                }
            }
        }
    }

    fclose(input);
    fclose(output);

    return 0;
}

