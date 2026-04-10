#pragma once

#include <stdint.h>
#include <string.h>

#define REGISTER_SIZE 3 
#define INSTRUCTION_SIZE UINT8_MAX 
#define MEMORY_SIZE UINT8_MAX

typedef enum DspBytecode{
  BYTECODE_HALT,
  BYTECODE_HARDCLIP,
  BYTECODE_SOFTCLIP,
  BYTECODE_GAIN,
  BYTECODE_LOAD,
  BYTECODE_SET
} DspBytecode;

typedef enum VmRegisterBank{
    RF,
    RUI,
}VmRegisterBank;

typedef struct VmRegister{
   size_t id; 
   VmRegisterBank bank;
}VmRegister;

typedef union DspBytecodeInstructionData{
    struct{size_t dst; size_t src; float threshold;} hard_clip;
    struct{size_t dst; size_t src;} soft_clip;
    struct{size_t dst; size_t src; float gain;} gain;
    struct{VmRegister dst; size_t memory_addr;}load;
    //TODO float is a hack as all types are 32bit
    struct{VmRegister dst; float value;}set;
    struct{char _;} none;
} DspBytecodeInstructionData;

typedef struct DspBytecodeInstruction{
   DspBytecode code;
   DspBytecodeInstructionData data;
}DspBytecodeInstruction;

typedef struct DspBytecodeVM{
    float rf[REGISTER_SIZE];
    uint32_t rui[REGISTER_SIZE]; 
    size_t stack_ptr; 
    DspBytecodeInstruction program[INSTRUCTION_SIZE];
    size_t program_ptr;
    float memory[MEMORY_SIZE];
}DspBytecodeVM;

void dsp_bytecode_vm_init(DspBytecodeVM *self, const DspBytecodeInstruction program[INSTRUCTION_SIZE]);

float dsp_bytecode_vm_play(
    DspBytecodeVM *self
    // const float input_sample 
);

DspBytecodeInstruction dsp_bytecode_instruction_halt(void);
DspBytecodeInstruction dsp_bytecode_instruction_hardclip( size_t dst,size_t src, float threshold);
DspBytecodeInstruction dsp_bytecode_instruction_softclip( size_t dst,size_t src);
DspBytecodeInstruction dsp_bytecode_instruction_gain( size_t dst,size_t src, float gain);
DspBytecodeInstruction dsp_bytecode_instruction_load(VmRegister dst, size_t memory_addr);
DspBytecodeInstruction dsp_bytecode_instruction_set(VmRegister dst, float value);
