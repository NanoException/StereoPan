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
    static void SetParameter(float samplerate, float frequency, float Q);
    static void DoProcess(float* bufferPtr, int bufferSize);


private:
    static float a0, a1, a2, b0, b1, b2;
    static float in1, in2;
    static float out1, out2;

};