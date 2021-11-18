/*
  ==============================================================================

    LPF.cpp
    Created: 18 Nov 2021 12:06:54am
    Author:  liquid1224

  ==============================================================================
*/

#include "LPF.h"
#include "corecrt_math_defines.h"
#include "cmath"

LPF::LPF()
    : a0(1.0), a1(0.0), a2(0.0),
    b0(1.0), b1(0.0), b2(0.0),
    in1(0.0), in2(0.0),
    out1(0.0), out2(0.0)
{}

LPF::~LPF() {}

void LPF::SetParameter(double samplerate, double frequency, double Q)
{
    double omega = 2.0 * M_PI * frequency / samplerate;
    double sinw = sin(omega);
    double cosw = cos(omega);
    double alpha = sinw / (2 * Q);

    a0 = 1 + alpha;
    a1 = -2 * cosw;
    a2 = 1 - alpha;
    b0 = (1 - cosw) / 2;
    b1 = 1 - cosw;
    b2 = (1 - cosw) / 2;
}

void LPF::DoProcess(double* bufferptr, int buffersize)
{
    for (int i = 0; i < buffersize; i++)
    {
        double out0 = (b0 / a0) * bufferptr[i] + (b1 / a0) * in1 + (b2 / a0) * in2 - (a1 / a0) * out1 - (a2 / a0) * out2;
        in2 = in1;
        in1 = bufferptr[i];

        out2 = out1;
        out1 = out0;
        bufferptr[i] = out0;
    }
}