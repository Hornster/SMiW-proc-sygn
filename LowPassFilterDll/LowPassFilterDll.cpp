// LowPassFilterDll.cpp : Defines the exported functions for the DLL application.
//

#include "LowPassFilterDll.h"
#include "stdafx.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <numeric>

//FOR OVERDRIVE
const float cutValue = 0.8;

//FOR NOT WORKING FURIER TRANSFORM
const int filterFreqBorder = 2000; //in herz
int filterInterval = 100;	//in herz
//Size of the window in samples.
const int SAMPLES_AMOUNT = 220;		//lowest hearable sound is 20Hz. 44000/20 = 2200
const int MAX_FREQ = 2000; //in herz. 10000 is quite high already,some people cannot hear above 16000.
const float PI2_BY_SAMPLES_AMOUNT = (M_PI * 2) / (float)SAMPLES_AMOUNT;

const int sampleFreq = 44100;

float sourceSamples[SAMPLES_AMOUNT];	//the source samples (int)
float modifiedSamples[SAMPLES_AMOUNT];  //samples after filtration (int)
int arrayPtr = 0;
int influenceArrayPtr = 0;				//Used for modification of data in the modifiedSamples table
int currentFreq = filterFreqBorder;		//Currently analyzed frequency

int ReadSampleFromSource()
{
	return 0;
}
int SpitOutModifiedSample()
{
	return 0;
}
//Calculates value of single point for given frequency.
inline float CalcSinglePoint(float point, int pointIndex)//floats shall be int when used in the chip
{
	float time = pointIndex / currentFreq;
	return point * exp(-currentFreq*time*PI2_BY_SAMPLES_AMOUNT);
}
//Sums all points for single frequency.
float SumPoints()//floats shall be int when used in the chip
{
	float sum = 0;
	for (int i = 0; i < SAMPLES_AMOUNT; i++)
	{
		sum += CalcSinglePoint(sourceSamples[i], i);
	}

	return sum;
}
//Subtracts value of sine of given freq from current sample.
void InfluenceSamples(float modifiedSampleValue)//floats shall be int when used in the chip
{
	modifiedSamples[influenceArrayPtr] = modifiedSamples[influenceArrayPtr] - modifiedSampleValue;
}
//Iterates the index through the modifiedSamples array. When the indexer is brought back to 0
//(whole array iterated through) returns FALSE. Returns TRUE if iteration is still happening.
bool IterateArrayModifyingPtr()
{
	influenceArrayPtr++;

	if (influenceArrayPtr >= SAMPLES_AMOUNT)
	{
		influenceArrayPtr = 0;
		return false;
	}
	else
	{
		return true;
	}
}
//Increases the savePtr and readPtr by 1. If these reach out of the range - brings them back to 0.
void IterateArrayPtrs()
{
	arrayPtr++;

	if (arrayPtr >= SAMPLES_AMOUNT)
		arrayPtr = 0;
}
//Returns TRUE when desired range has been checked already. FALSE otherwise.
//Upon returning TRUE, resets the currentFreq to filterFreqBorder
bool SwitchFreq()
{
	if (currentFreq <= MAX_FREQ)
	{
		currentFreq += filterInterval;
		return false;
	}
	else
	{
		currentFreq = filterFreqBorder;
		return true;
	}
}
//Loads new sample into the arrays
void LoadRawSample(float newSample)//floats shall be int when used in the chip
{
	modifiedSamples[arrayPtr] = sourceSamples[arrayPtr] = newSample;
}

int main()
{
	while (true)
	{
		//Read new sample
		float newSample = ReadSampleFromSource();
		//Load newly read sample to both arrays
		LoadRawSample(newSample);
		//Begin Furier's transform
		do
		{
			//Modify curren sample by subtracting frequencies above specified border (filterFreqBorder)
			InfluenceSamples(SumPoints());
			//As long as the currentFrequency did not reach the MAX_FREQ, begin another iteration of loop.
		} while (!SwitchFreq());

		//Output the ready sample.
		SpitOutModifiedSample();

		//at the end, iterate the pointers.
		IterateArrayPtrs();
	}

}

extern "C" __declspec(dllexport) float __stdcall ApplyFilter(float sample)
{
	//Read new sample
	float newSample = sample;
	//Load newly read sample to both arrays
	LoadRawSample(newSample);
	//Begin Furier's transform
	do
	{
		do
		{
			//Modify curren sample by subtracting frequencies above specified border (filterFreqBorder)
			InfluenceSamples(SumPoints());
			
		} while (IterateArrayModifyingPtr());
		//As long as the currentFrequency did not reach the MAX_FREQ, begin another iteration of loop.
	} while (!SwitchFreq());

	//Output the ready sample.
	//SpitOutModifiedSample(); 

	newSample = modifiedSamples[arrayPtr];
	//at the end, iterate the pointers.
	IterateArrayPtrs();

	return newSample;
}



//OVERDRIVE
extern "C" __declspec(dllexport) float __stdcall ApplyOverdriveFilter(float sample, float cutValue)
{
	
	if (abs(sample) > cutValue)
	{
		sample = sample > 0 ? cutValue : -cutValue;
	}

	return sample;
}

extern "C" __declspec(dllexport) float __stdcall ApplyMachineSound(float sample, float step)
{
	int stepsAmount = ceil(1.0f / step);
	float newSample = 0.0f;

	for (int i = 1; i <= stepsAmount; i++)
	{
		if (abs(sample) < i*step)
		{
			newSample = sample > 0? (i-1) * step : -(i-1) * step;
			break;
		}
	}

	if (newSample > 1.0f)
		newSample = 1.0f;
	else if (newSample < -1.0f)
		newSample = -1.0f;

	return newSample;
}

extern "C" __declspec(dllexport) float __stdcall ApplyMachineSoundSine(float sample, float sampleIndex, float samplingFreq, float sineFreq)
{
	float sampleTime = sampleIndex / samplingFreq;

	float sineValue = sin(sampleTime * sineFreq);

	float result = sample * sineValue;//sample * sineValue;


	return result;
}
extern "C" __declspec(dllexport) float __stdcall ApplyMachineSound2Sine(float sample, float sampleIndex, float samplingFreq, float sine1Freq, float sine2Freq)
{
	float sampleTime = sampleIndex / samplingFreq;

	float sine1Value = sin(sampleTime * sine1Freq);
	float sine2Value = sin(sampleTime * sine2Freq);

	float result = sample * sine1Value*sine2Value;//sample * sineValue;


	return result;
}