/*
  ==============================================================================

    LPF.h
    Created: 18 Nov 2021 12:06:54am
    Author:  liquid1224

  ==============================================================================
*/

#pragma once

class LPF
{
public:
    LPF();
    ~LPF();
    void SetParameter(double samplerate, double frequency, double Q);
    void DoProcess(double* bufferptr, int buffersize);

private:
    double a0, a1, a2, b0, b1, b2;
    double in1, in2;
    double out1, out2;
};