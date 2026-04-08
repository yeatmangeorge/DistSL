#include "dsp_bytecode_vm.h"
#include "dsp_functions.h"
#include <stdint.h>

static void dsp_bytecode_vm_reset_stack(DspBytecodeVM *self){
   memset(self->stack, 0.0f, sizeof(float) * STACK_SIZE);
   self->stack_ptr = 0;
}

DspBytecodeInstruction dsp_bytecode_instruction_push(float value){
    return (DspBytecodeInstruction){.code = BYTECODE_PUSH, .data = {.push = {value}}};
}

DspBytecodeInstruction dsp_bytecode_instruction_pop(void){
    return (DspBytecodeInstruction){.code = BYTECODE_POP, .data = {.none = 0}};
}

DspBytecodeInstruction dsp_bytecode_instruction_halt(void){
    return (DspBytecodeInstruction){.code = BYTECODE_HALT, .data = {.none = 0}};
}

DspBytecodeInstruction dsp_bytecode_instruction_hardclip(float threshold){
    return (DspBytecodeInstruction){.code = BYTECODE_HARDCLIP, .data = {.hard_clip.threshold = threshold}};
}

DspBytecodeInstruction dsp_bytecode_instruction_softclip(void){
    return (DspBytecodeInstruction){.code = BYTECODE_SOFTCLIP, .data = {.none = 0}};
}

DspBytecodeInstruction dsp_bytecode_instruction_gain(float gain){
    return (DspBytecodeInstruction){.code = BYTECODE_GAIN, .data = {.gain.gain = gain}};
}


typedef enum VmContinuation{
    VM_CONTINUATION_STOP,
    VM_CONTINUATION_CONT
}VmContinuation;

typedef VmContinuation (*InstructionHandler)(DspBytecodeVM *vm, DspBytecodeInstruction instruction);

#define PUSH(VALUE) vm->stack[vm->stack_ptr++] = VALUE
#define POP() vm->stack[--vm->stack_ptr]

VmContinuation instruction_handler_push(DspBytecodeVM *vm, DspBytecodeInstruction instruction){
    if(vm->stack_ptr == STACK_SIZE){
        return VM_CONTINUATION_STOP;
    }
    PUSH( instruction.data.push.value);
    return VM_CONTINUATION_CONT;
}

VmContinuation instruction_handler_pop(DspBytecodeVM *vm, DspBytecodeInstruction instruction){
    (void) instruction;
   if(vm->stack_ptr <= 0){
      return VM_CONTINUATION_STOP; 
   } 
   --vm->stack_ptr;
   return VM_CONTINUATION_CONT;
}

VmContinuation instruction_handler_hardclip(DspBytecodeVM *vm, DspBytecodeInstruction instruction){
    if(vm->stack_ptr <= 0){
       return VM_CONTINUATION_STOP; 
    } 
    PUSH(dsp_hard_clip(POP(), instruction.data.hard_clip.threshold));
    return VM_CONTINUATION_CONT;
}

VmContinuation instruction_handler_softclip(DspBytecodeVM *vm, DspBytecodeInstruction instruction){
    (void) instruction;
    if(vm->stack_ptr<=0){
        return VM_CONTINUATION_STOP;
    }
    PUSH(dsp_soft_clip(POP()));
    return VM_CONTINUATION_CONT;
}

VmContinuation instruction_handler_gain(DspBytecodeVM *vm, DspBytecodeInstruction instruction){
    if(vm->stack_ptr<=0){
        return VM_CONTINUATION_STOP;
    }
    PUSH(dsp_gain(POP(), instruction.data.gain.gain));
    return VM_CONTINUATION_CONT;
}

VmContinuation instruction_handler_halt(DspBytecodeVM *vm, DspBytecodeInstruction instruction){
    (void) vm;
    (void) instruction;
    return VM_CONTINUATION_STOP;
}

static InstructionHandler instruction_handler_jump_table[] = {
  instruction_handler_halt,
  instruction_handler_push,
  instruction_handler_pop,
  instruction_handler_hardclip,
  instruction_handler_softclip,
  instruction_handler_gain,
  };

float dsp_bytecode_vm_play(
    DspBytecodeVM *self,
    const float input_sample, 
    const DspBytecodeInstruction program[INSTRUCTION_SIZE] 
){
    dsp_bytecode_vm_reset_stack(self);
    //We need to artificially push the input_sample onto the vm stack so it can be worked on
   DspBytecodeInstruction push_input_sample_instruction = dsp_bytecode_instruction_push(input_sample);
   instruction_handler_jump_table[push_input_sample_instruction.code](self, push_input_sample_instruction);
    
    for(uint8_t instruction_ptr = 0; instruction_ptr < INSTRUCTION_SIZE; instruction_ptr++){
        DspBytecodeInstruction current_instruction = program[instruction_ptr];
        if(instruction_handler_jump_table[current_instruction.code](self, current_instruction) == VM_CONTINUATION_STOP){
            break;
        }
    }
    
    // clip for safety
   return dsp_hard_clip(self->stack[--self->stack_ptr], 1.0f);  
}
