#include "idct.h"

void idct :: read_data() {
    for (int u = 0; u < 8; u++) {
        for (int v = 0; v < 8; v++) {
            input_data[u][v] = in64[u][v].read();
        }
    }
}

//inverse discrete cosine transform
void idct :: calculate_idct(){
 	for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            double sum_val = 0.0;
            for (int u = 0; u < 8; u++) {
                for (int v = 0; v < 8; v++) {
                    double coeff = input_data[u][v];
                    if ((u == 0) && (v == 0)) {
                        coeff /= 8.0;
                    }
                    else if ((u == 0) || (v == 0)) {
                        coeff /= (4.0 * sqrt(2.0));
                    }
                    else {
                        coeff /= 4.0;
                    }
                    sum_val += coeff * fcosine[x][u].read() * fcosine[y][v].read();
                }
            }
            out64[x][y].write(sum_val + 128.0);
        }
    }

}
