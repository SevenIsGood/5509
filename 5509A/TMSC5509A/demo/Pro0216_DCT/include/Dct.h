#ifndef _DCT_H_
#define _DCT_H_

void IMG_fdct_8x8(short *dct_data, unsigned num_fdcts);
void IMG_idct_8x8_12q4(short *idct_data, unsigned num_idcts);

#endif