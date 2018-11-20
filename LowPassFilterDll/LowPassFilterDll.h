#pragma once
extern "C" __declspec(dllexport) float __stdcall ApplyFilter(float sample);
extern "C" __declspec(dllexport) float __stdcall ApplyOverdriveFilter(float sample, float cutValue);
extern "C" __declspec(dllexport) float __stdcall ApplyMachineSound(float sample, float step);
extern "C" __declspec(dllexport) float __stdcall ApplyMachineSoundSine(float sample, float sampleIndex, float samplingFreq, float sineFreq);
extern "C" __declspec(dllexport) float __stdcall ApplyMachineSound2Sine(float sample, float sampleIndex, float samplingFreq, float sine1Freq, float sine2Freq);

extern "C" __declspec(dllexport) float __stdcall ModulateVoiceRobotic(float sample, float sampleIndex, float samplingFreq, float sineFreq);
extern "C" __declspec(dllexport) float __stdcall NormalizeSample(int sample, int maxSampleCapacity);
extern "C" __declspec(dllexport) int __stdcall StrangifySample(float normalizedSample, int maxSampleCapacity);

extern "C" __declspec(dllexport) int __stdcall ProcessData(int fistSample, int secondSample, int samplingFreq, int sineFreq);