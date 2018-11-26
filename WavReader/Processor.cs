using System;

namespace WavProcessor
{
    using System.Runtime.InteropServices;

    public class Processor
    {
        private const int MaxInsaneSampleHalved = 0x007fffff;

        private const int LowerBorderSineFrequency = 1052;

        private int currentFrequency = LowerBorderSineFrequency;

        private int samplingFrequency;

        

        public Processor(int samplingFrequency, int modulationFreq)
        {
            this.samplingFrequency = samplingFrequency;
            this.currentFrequency = modulationFreq;
        }

        public (float, float) Process((float, float) sample)
        {
            var insaneSamples = (this.FormatSaneToInsane(sample.Item1), this.FormatSaneToInsane(sample.Item1));
            var insaneResult = ProcessData(insaneSamples.Item1, insaneSamples.Item2, this.samplingFrequency, this.currentFrequency);
            var result = this.FormatInsaneToSane(insaneResult);
            return (result, result);
        }

        [DllImport("LowPassFilterDll", CallingConvention = CallingConvention.StdCall)]
        private static extern int ProcessData(int firstSample, int secondSample, int samplingFreq, int sineFreq);

        //// [DllImport("LowPassFilterDll", CallingConvention = CallingConvention.StdCall)]
        //// private static extern float ApplyMachineSoundSine(float sample, float sampleIndex, float samplingFreq, float sineFreq);

        //// [DllImport("LowPassFilterDll", CallingConvention = CallingConvention.StdCall)]
        //// private static extern float ApplyMachineSound2Sine(float sample, float sampleIndex, float samplingFreq, float sine1Freq, float sine2Freq);

        ////[DllImport("LowPassFilterDll", CallingConvention = CallingConvention.StdCall)]
        ////private static extern float ModulateVoiceRobotic(float sample, float sampleIndex, float samplingFreq, float sineFreq);

        //// [DllImport("LowPassFilterDll", CallingConvention = CallingConvention.StdCall)]
        //// private static extern float NormalizeSample(int sample, int maxSampleCapacity);

        //// [DllImport("LowPassFilterDll", CallingConvention = CallingConvention.StdCall)]
        //// private static extern int StrangifySample(float normalizedSample, int maxSampleCapacity);

        private int FormatSaneToInsane(float sample)
        {
            return MaxInsaneSampleHalved + (int)(sample * MaxInsaneSampleHalved);
        }

        private float FormatInsaneToSane(int sample)
        {
            return (sample - MaxInsaneSampleHalved) / (float)MaxInsaneSampleHalved;
        }
}
}