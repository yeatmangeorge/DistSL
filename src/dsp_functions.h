#pragma once

float dsp_soft_clip(const float input);
float dsp_hard_clip(const float input, const float threshold);
float dsp_gain(const float input, const float gain);
float dsp_low_pass_filter(const float input, const float previous, const float cutoff);
float dsp_high_pass_filter(const float input, const float previous, const float cutoff);
