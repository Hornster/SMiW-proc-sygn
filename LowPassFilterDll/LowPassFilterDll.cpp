// LowPassFilterDll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include "LowPassFilterDll.h"
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
//GOOD SOUNDING
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


////////////////////////////////////
//DALEK VOICE MODULATOR (ROBOT VOICE)
////////////////////////////////////
//Compresses the values that exceed the 1.0f.
//Divides the stuff by 2 as long as it exceeds the 1.0f value.
float CompressSample(float sample)
{
	while(abs(sample) > 0.8f)
	{
		sample /= 2;
	}
	return sample;
}
//Multiplies signal sample by provided value.
inline float MultiplySample(float sampleValue, float multiplyValue)
{
	return sampleValue * multiplyValue;
}
//Performs signal addition (multiplication) and returns the resulting signal.
inline float AddSignals(float signal1, float signal2)
{
	return (signal1 + signal2)/2; //Divide by two to prevent increasing of the samples over 1.0f.
}
//Simulates single diode in the modulation ring.
//Outputs modified by designed function input sample.
float DiodeNode(float inputSample)
{
	float h = 3.0f;		//Parameter specifying the slope of the linear section (beyond vl value).
	//Values below are borders which, compared to the inputSample, are used to determine what
	//formula should be used by the modulator.
	float vb = 0.2f;	//Lower threshold of voltage in the ring modulator (0.25f is value of the sine wave only)
						//(voltage beyond, but lower than vl, is described with a polynominal).
	float vl = 0.4f;	//Upper threshold of voltage in the ring modulator diode 
						//(voltage beyond is increasing linearly).


	inputSample = abs(inputSample);

	if (inputSample <= vb)
	{
		return 0;	//Values below vb are close to 0.
	}
	else if (inputSample <= vl)
	{
		//Used for more readable formula. 
		float diffInputSampleVb = inputSample - vb;	
		//The formula for when inputSample is lesser than vl, but greater than vb.
		//The 2 number is part of the formula.

		float result = h * ((diffInputSampleVb*diffInputSampleVb) / (2 * (vl - vb)));		//Square the difference between input sample and vb. Then scales the result with slope factor.
		//result /= 2 * (vl - vb);									//Multiplies difference of vl and vb values with two, then divides the above part of the formula with it.
		return result;
	}
	else
	{
		float diffVlVb = vl - vb;	//Used for more readable formula. Difference between vl and vb values.
		float result = h * inputSample;		//Scale the sample voltage with slope factor
		result -= h * vl;					//Scale the vl value with slope factor
		result += h * ((diffVlVb * diffVlVb) / (2 * diffVlVb));		// square diffVlVb value and divide it by diffVlVb multiplied by two. Then, scale the result with slope factor.
		return result;
	}
}
//Called from the outside. Returns modulated sample (made to sound like a robot).
extern "C" __declspec(dllexport) float __stdcall ModulateVoiceRobotic(float sample, float sampleIndex, float samplingFreq, float sineFreq)
{
	//Calculate the time of the current sample.
	float sampleTime = sampleIndex / samplingFreq;
	//Calculate the value of the sine wave.
	float sineValue = sin(sampleTime * sineFreq);
	sample = sample * 4.0f;
	//First part of modulator
	float preScaledSine = MultiplySample(sineValue, 0.5f);	//According to the described loop - the sine value shall be scaled by 0.5f
	float preScaledSample = AddSignals(preScaledSine, sample);//Add the scaled sine to the sample.
	preScaledSine = MultiplySample(preScaledSine, -1.0f);	//Next, multiply the scaled sine wave by -1.
	preScaledSine = AddSignals(preScaledSine, sample);		//Before the crossroad, mix the preScaledSine and sample.

	//Second part of the modulator - Input side
	float nonInvertedInputProbe = DiodeNode(preScaledSample);							//Part of the input that has not been inverted and got to a diode.
	float invertedInputProbe = MultiplySample(preScaledSample, -1.0f);					//Invert the input sample
	invertedInputProbe = DiodeNode(invertedInputProbe);									//Process the inverted sample with a diode.
	float preparedInputSample = AddSignals(nonInvertedInputProbe, invertedInputProbe);	//At the end, mix the probes in this part.

	//Second part of the modulator - Sine side
	float nonInvertedSineProbe = DiodeNode(preScaledSine);								//Part of the sine that is not inverted and gets processed by a 
																						//diode instantly.
	float invertedSineProbe = MultiplySample(preScaledSine, -1.0f);						//Invert the second sine sample...
	invertedSineProbe = DiodeNode(invertedSineProbe);									//...and then process it within the diode.
	float preparedSineSample = AddSignals(nonInvertedSineProbe, invertedSineProbe);		//Join the two probes together.

	preparedSineSample = MultiplySample(preparedSineSample, -1.0f);
	//Final mixing
	float result = AddSignals(preparedSineSample, preparedInputSample);	//Join the two processed signals.
	/*if (abs(result) > 1.0f)
	{
		result > 0 ? result = 1.0f : result = -1.0f;	//If the sound value is too big - cut it down to max value.
	}*/
	result = CompressSample(result);
	return result;	//Return the modulated sample.


}
//Calculates the half capacity of an unsigned sample. Uses provided sample capacity (maximal value that can be stored in it).
inline int CalcSampleHalfCapacity(int maxSampleCapacity)
{
	return maxSampleCapacity / 2;
}
//Subtracts halfCapacity from sampleValue, positioning it accordingly to the 0 value.
inline int OffsetSample(int sampleHalfCapacity, int sampleValue)
{
	return sampleValue - sampleHalfCapacity;
}
//Adds the half sample value to the sample, returning it to it's previous, relative position towards 0.
inline int DeOffsetSample(int sampleHalfCapacity, int sampleValue)
{
	return sampleValue + sampleHalfCapacity;
}
//Normalizes the provided sample (ranges from -1.0f to 1.0f), using max (unsigned) capacity of the sample.
extern "C" __declspec(dllexport) float __stdcall NormalizeSample(int sample, int maxSampleCapacity)
{
	int halfCapacity = CalcSampleHalfCapacity(maxSampleCapacity);
	sample = OffsetSample(halfCapacity, sample);
	return (float)sample / (float)halfCapacity;
}
extern "C" __declspec(dllexport) int __stdcall StrangifySample(float normalizedSample, int maxSampleCapacity)
{
	int halfCapacity = CalcSampleHalfCapacity(maxSampleCapacity);
	int strangifiedSample = normalizedSample * halfCapacity;
	return DeOffsetSample(halfCapacity, strangifiedSample);
}

extern "C" __declspec(dllexport) int __stdcall ProcessData(int firstSample, int secondSample, int samplingFreq, int sineFreq)
{
	static int index = 0;
	float sample = (((firstSample + secondSample) / 2) - 0x007fffff) / static_cast<float>(0x007fffff);
	float modulatedSample = ModulateVoiceRobotic(sample, index, samplingFreq, sineFreq);
	index++;
	return 0x007fffff + static_cast<int>(modulatedSample * 0x007fffff);
}