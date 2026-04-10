#include "dsp_bytecode_vm.h"
#include "dsp_functions.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

DspBytecodeInstruction dsp_bytecode_instruction_halt(void){
    return (DspBytecodeInstruction){.code = BYTECODE_HALT, .data = {.none = 0}};
}

DspBytecodeInstruction dsp_bytecode_instruction_hardclip( size_t dst, size_t src, float threshold){
    return (DspBytecodeInstruction){.code = BYTECODE_HARDCLIP, .data = {.hard_clip = {.src = src, .dst = dst, .threshold = threshold}}};
}

DspBytecodeInstruction dsp_bytecode_instruction_softclip( size_t dst, size_t src){
    return (DspBytecodeInstruction){.code = BYTECODE_SOFTCLIP, .data = {.soft_clip = {.src = src, .dst = dst}}};
}

DspBytecodeInstruction dsp_bytecode_instruction_gain( size_t dst, size_t src, float gain){
    return (DspBytecodeInstruction){.code = BYTECODE_GAIN, .data = {.gain = {.src = src, .dst = dst, .gain = gain}}};
}

DspBytecodeInstruction dsp_bytecode_instruction_load(VmRegister dst, size_t memory_addr){
    return (DspBytecodeInstruction){.code = BYTECODE_LOAD, .data = {.load = {.dst = dst, .memory_addr = memory_addr}}};
}

DspBytecodeInstruction dsp_bytecode_instruction_save(size_t memory_addr, VmRegister dst){
    return (DspBytecodeInstruction){.code = BYTECODE_SAVE, .data = {.save = {.memory_addr = memory_addr, .dst = dst}}};
}

DspBytecodeInstruction dsp_bytecode_instruction_set(VmRegister dst, float value){
   return (DspBytecodeInstruction){.code = BYTECODE_SET, .data.set = {.dst = dst, .value = value}};
}

typedef enum VmContinuation{
    VM_CONTINUATION_STOP,
    VM_CONTINUATION_CONT
}VmContinuation;

typedef VmContinuation (*InstructionHandler)(DspBytecodeVM *vm, DspBytecodeInstruction instruction);

VmContinuation instruction_handler_hardclip(DspBytecodeVM *vm, DspBytecodeInstruction instruction){
    vm->rf[instruction.data.hard_clip.dst] = 
        dsp_hard_clip(vm->rf[instruction.data.hard_clip.src],
            instruction.data.hard_clip.threshold);
    return VM_CONTINUATION_CONT;
}

VmContinuation instruction_handler_softclip(DspBytecodeVM *vm, DspBytecodeInstruction instruction){
    vm->rf[instruction.data.soft_clip.dst]
        = dsp_soft_clip(vm->rf[instruction.data.soft_clip.src]);
    return VM_CONTINUATION_CONT;
}

VmContinuation instruction_handler_gain(DspBytecodeVM *vm, DspBytecodeInstruction instruction){
    vm->rf[instruction.data.gain.dst] = 
        dsp_gain(vm->rf[instruction.data.gain.src], instruction.data.gain.gain);
    return VM_CONTINUATION_CONT;
}

VmContinuation instruction_handler_halt(DspBytecodeVM *vm, DspBytecodeInstruction instruction){
    (void) vm;
    (void) instruction;
    return VM_CONTINUATION_STOP;
}

VmContinuation instruction_handler_load(DspBytecodeVM *vm, DspBytecodeInstruction instruction){
    switch (instruction.data.load.dst.bank) {
    case RF:
        memcpy(
            &vm->rf[instruction.data.load.dst.id] ,
            &vm->memory[instruction.data.load.memory_addr] ,
            sizeof(vm->rf[0])
        );
    break;
    case RUI:
        memcpy(
            &vm->rui[instruction.data.load.dst.id], 
            &vm->memory[instruction.data.load.memory_addr],
            sizeof(vm->rui[0])
        );
    break;
    }
    return VM_CONTINUATION_CONT;
}

VmContinuation instruction_handler_save(DspBytecodeVM *vm, DspBytecodeInstruction instruction){
    switch (instruction.data.save.dst.bank) {
        case RF:
        vm->memory[instruction.data.save.memory_addr] = vm->rf[instruction.data.save.memory_addr];
        break;
        case RUI:
        vm->memory[instruction.data.save.memory_addr] = (float)vm->rui[instruction.data.save.memory_addr];
        break;
    }
    return VM_CONTINUATION_CONT;
}

VmContinuation instruction_handler_set(DspBytecodeVM *vm, DspBytecodeInstruction instruction){
    switch(instruction.data.set.dst.bank) {
        case RF:
        vm->rf[instruction.data.set.dst.id] = instruction.data.set.value;
        break;
        case RUI:
        vm->rui[instruction.data.set.dst.id] = (uint32_t)instruction.data.set.value;
        break;
    }
    return VM_CONTINUATION_CONT;
}

static InstructionHandler instruction_handler_jump_table[] = {
  instruction_handler_halt,
  instruction_handler_hardclip,
  instruction_handler_softclip,
  instruction_handler_gain,
  instruction_handler_load,
  instruction_handler_save,
  instruction_handler_set
  };

void dsp_bytecode_vm_init(DspBytecodeVM *self, const DspBytecodeInstruction program[INSTRUCTION_SIZE]){
    memset(self->rf, 0.0f, sizeof(self->rf));
    memset(self->rui, 0, sizeof(self->rui));
    memcpy(self->program, program, sizeof(self->program));
    self->program_ptr = 0; 
    memset(self->memory, 0.0f, sizeof(self->memory));
}

float dsp_bytecode_vm_play(
    DspBytecodeVM *self
){
    for(self->program_ptr = 0; self->program_ptr < INSTRUCTION_SIZE; self->program_ptr++){
        DspBytecodeInstruction current_instruction = self->program[self->program_ptr];
        if(instruction_handler_jump_table[current_instruction.code](self, current_instruction) == VM_CONTINUATION_STOP){
            break;
        }
    }
    
    // clip for safety
   return dsp_hard_clip(self->rf[0], 1.0f);  
}
