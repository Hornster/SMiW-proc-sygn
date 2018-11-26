using System;

namespace WavProcessor
{
    using NAudio.Wave;

    public class Program
    {
        private static int modulationFreq = 920;

        public static void InputFreq()
        {
            Console.WriteLine("Podaj czestotliwosc sinusoidy modulujacej (ujemna by zakonczyc program): \n");
            while (!int.TryParse(Console.ReadLine(), out modulationFreq))
            {
                Console.WriteLine("Podano zlaczestotliwosc. Powinna byc wieksza od 0 i liczba calkowita. \n");
            }
        }

        public static void Main(string[] args)
        {
            do
            {
                InputFreq();
                using (var reader = new WaveFileReader("testSound.wav"))
                {
                    using (var writer = new WaveFileWriter("newTestSound.wav", reader.WaveFormat))
                    {
                        var count = reader.SampleCount;
                        var processor = new Processor(reader.WaveFormat.SampleRate, modulationFreq);
                        
                        for (var i = 0; i < count; ++i)
                        {
                            var sampleFrame = reader.ReadNextSampleFrame();
                            if (reader.WaveFormat.Channels == 2)
                            {
                                var samples = processor.Process((sampleFrame[0], sampleFrame[1]));
                                writer.WriteSample(samples.Item1);
                                writer.WriteSample(samples.Item2);
                            }
                            else
                            {
                                var samples = processor.Process((sampleFrame[0], sampleFrame[0]));
                                writer.WriteSample(samples.Item1);
                                //writer.WriteSample(samples.Item2);
                            }
                        }
                    }
                }
            } while (modulationFreq >= 0);
        }
    }
}