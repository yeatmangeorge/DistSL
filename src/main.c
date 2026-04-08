#include "dsp_functions.h"

#include <stdio.h>
#include <stdlib.h>

#define NUM_OF_INPUTS 2
#define ERR(MSG) \
    fprintf(stderr, "ERROR: " MSG "\n");\
    exit(1);

int main(int argc, char *argv[]){
   if(argc!=NUM_OF_INPUTS){
       ERR("Invalid args");
   } 
   float input = (float)atof(argv[1]);
   float output = dsp_hard_clip(input, 0.8f);
   fprintf(stdout, "%f\n", (float)output);
   return 0; 
}