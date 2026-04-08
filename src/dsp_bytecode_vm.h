#pragma once

#include <stdint.h>
#include <string.h>

#define STACK_SIZE UINT8_MAX 
#define INSTRUCTION_SIZE UINT8_MAX 
#define MEMORY_SIZE UINT8_MAX

typedef enum DspBytecode{
  BYTECODE_HALT,
  BYTECODE_PUSH,
  BYTECODE_POP,
  BYTECODE_HARDCLIP,
  BYTECODE_SOFTCLIP,
  BYTECODE_GAIN,
} DspBytecode;

/**
* This is just to reduce confusion and allow multiparam 
* bytecodes in the future
*/
typedef union DspBytecodeInstructionData{
    struct{float value;} push;
    struct{float threshold;} hard_clip;
    struct{float gain;} gain;
    struct{char _;} none;
} DspBytecodeInstructionData;

typedef struct DspBytecodeInstruction{
   DspBytecode code;
   DspBytecodeInstructionData data;
}DspBytecodeInstruction;

typedef struct DspBytecodeVM{
    float stack[STACK_SIZE];
    uint8_t stack_ptr; 
    DspBytecodeInstruction program[INSTRUCTION_SIZE];
    uint8_t program_ptr;
    float memory[MEMORY_SIZE];
}DspBytecodeVM;

void dsp_bytecode_vm_init(DspBytecodeVM *self, const DspBytecodeInstruction program[INSTRUCTION_SIZE]);

float dsp_bytecode_vm_play(
    DspBytecodeVM *self,
    const float input_sample 
);

DspBytecodeInstruction dsp_bytecode_instruction_push(float value);
DspBytecodeInstruction dsp_bytecode_instruction_pop(void);
DspBytecodeInstruction dsp_bytecode_instruction_halt(void);
DspBytecodeInstruction dsp_bytecode_instruction_hardclip(float threshold);
DspBytecodeInstruction dsp_bytecode_instruction_softclip(void);
DspBytecodeInstruction dsp_bytecode_instruction_gain(float gain);