#pragma once

float dsp_soft_clip(const float input);
float dsp_hard_clip(const float input, const float threshold);
float dsp_gain(const float input, const float gain);