
using System;
using System.Runtime.InteropServices;

namespace WavProcessor
{
    using NAudio.Wave;

    public class Program
    {
        [DllImport("LowPassFilterDll", CallingConvention = CallingConvention.StdCall)]
        static extern float ApplyMachineSoundSine(float sample, float sampleIndex, float samplingFreq, float sineFreq);
        [DllImport("LowPassFilterDll", CallingConvention = CallingConvention.StdCall)]
        static extern float ApplyMachineSound2Sine(float sample, float sampleIndex, float samplingFreq, float sine1Freq, float sine2Freq);

        const int lowerBorderSineFreq = 220;
        private const int upperBorderSineFreq = 1037;

       /* public static float ChangeSineFrequency(float oldFreq)
        {
            bool isRising = true;
            if (isRising && oldFreq < upperBorderSineFreq)
            {
                return oldFreq + 100;
            }
            if (isRising)
            {
                isRising = false;
            }

            if (!isRising && oldFreq > lowerBorderSineFreq)
            {
                return oldFreq - 100;
            }
            if (!isRising)
            {
                isRising = true;
            }

            return oldFreq;
        }*/

        public static void Main(string[] args)
        {
            using (var reader = new WaveFileReader("testSound.wav"))
            {
                using (var writer = new WaveFileWriter("newTestSound.wav", reader.WaveFormat))
                {
                    float samplingFreq = 44100.0f;
                    var count = reader.SampleCount;
                    var processor = new Processor(reader.WaveFormat.SampleRate);
                    for (var i = 0; i < count; ++i)
                    {
                        var sampleFrame = reader.ReadNextSampleFrame();
                        var sampleLeft = ApplyMachineSound2Sine(sampleFrame[0], i, samplingFreq, upperBorderSineFreq, lowerBorderSineFreq);
                        //var sampleRight = ApplyOverdriveFilter(sampleFrame[1]);
                        //processor.Process((sampleFrame[0], sampleFrame[1]));
                        writer.WriteSample(sampleLeft);
                        //writer.WriteSample(sampleRight);
                    }
                }
            }
        }
    }

    public class Processor
    {
        private const float SilencingFactor = 0.7f;

        private readonly int bufferSize;

        private (float, float)[] buffer;

        private int position;

        public Processor(int bufferSize)
        {
            this.bufferSize = bufferSize;
            this.buffer = new (float, float)[bufferSize];
        }

        public (float, float) Process((float, float) sample)
        {
            var result = ((sample.Item1 + (this.buffer[this.position].Item1 * SilencingFactor)) / (1 + SilencingFactor),
                (sample.Item2 + (this.buffer[this.position].Item2 * SilencingFactor)) / (1 + SilencingFactor));
            this.buffer[this.position] = result;
            this.position++;
            if (this.position == this.bufferSize)
            {
                this.position = 0;
            }

            return result;
        }
    }
}

//https://webaudio.prototyping.bbc.co.uk/ring-modulator/

    //GainNode - przemnożnik
