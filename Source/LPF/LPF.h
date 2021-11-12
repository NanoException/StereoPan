/*
  ==============================================================================

    LPF.h
    Created: 12 Nov 2021 12:14:48pm
    Author:  liquid1224

  ==============================================================================
*/

#pragma once

class LPF {
public:
    LPF();
    ~LPF();
    void SetParameter(float samplerate, float frequency, float Q, float gain);
    void DoProcess(float* bufferPtr, int bufferSize);


private:
    float a0, a1, a2, b0, b1, b2;
    float in1, in2;
    float out1, out2;

};