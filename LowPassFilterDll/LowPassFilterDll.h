#pragma once
extern "C" __declspec(dllexport) float __stdcall ApplyFilter(float sample);
extern "C" __declspec(dllexport) float __stdcall ApplyOverdriveFilter(float sample, float cutValue);
extern "C" __declspec(dllexport) float __stdcall ApplyMachineSound(float sample, float step);
extern "C" __declspec(dllexport) float __stdcall ApplyMachineSoundSine(float sample, float sampleIndex, float samplingFreq, float sineFreq);
extern "C" __declspec(dllexport) float __stdcall ApplyMachineSound2Sine(float sample, float sampleIndex, float samplingFreq, float sine1Freq, float sine2Freq);