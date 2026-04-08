#include "dsp_functions.h"
#include <math.h>

float dsp_soft_clip(const float input){
    return tanhf(input);
}

static float clamp_lowest(const float a, const float b){
    return (a<b)?a:b;
}

static float clamp_highest(const float a, const float b){
    return (a>b)?a:b;
}

float dsp_hard_clip(const float input, const float threshold) {
    return (input >= 0) 
        ? clamp_lowest(input, threshold)
        : clamp_highest(input, threshold);
}

float dsp_gain(const float input, const float gain){
   return input * gain; 
}
