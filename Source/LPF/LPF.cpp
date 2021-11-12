/*
  ==============================================================================

    LPF.cpp
    Created: 12 Nov 2021 12:14:48pm
    Author:  liquid1224

  ==============================================================================
*/

#include "LPF.h"
#include <cmath>
#include "corecrt_math_defines.h"

LPF::LPF():
    a0(1.0f), a1(0.0f), a2(0.0f),
    b0(1.0f), b1(0.0f), b2(0.0f),
    in1(0.0f), in2(0.0f),
    out1(0.0f), out2(0.0f)
{}

LPF::~LPF() {}

void LPF::SetParameter(float sampletate, float frequency, float Q, float gain)
{
    float omega0 = 2.0f*M_PI*frequency/sampletate;
    float sinw = sin(omega0);
    float cosw = cos(omega0);
    float alpha = sinw / (2*Q);

    a0 = 1 + alpha;
    a1 = -2 * cosw;
    a2 = 1 - alpha;
    b0 = (1 - cosw) / 2;
    b1 = 1 - cosw;
    b2 = (1 - cosw) / 2;

}

void LPF::DoProcess(float* bufferPtr, int bufferSize)
{
    for (int i = 0; i < bufferSize; i++)
    {
        float out0 = (b0/a0)*bufferPtr[i] + (b1/a0)*in1 + (b2/a0)*in2 - (a1/a0)*out1 - (a2/a0)*out2;
        in2 = in1;
        in1 - bufferPtr[i];
        out2 = out1;
        out1 = out0;

        bufferPtr[i] = out0;
    }
}