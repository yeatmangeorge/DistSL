#include "dsp_bytecode_vm.h"

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
   
   DspBytecodeVM vm;
   const DspBytecodeInstruction program[INSTRUCTION_SIZE] = {
       dsp_bytecode_instruction_gain(0.5f),
       dsp_bytecode_instruction_hardclip(0.25f),
       dsp_bytecode_instruction_halt()
   };
   float output = dsp_bytecode_vm_play(&vm, input, program);
   
   fprintf(stdout, "%f\n", (float)output);
   return 0; 
}