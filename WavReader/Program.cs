namespace WavProcessor
{
    using NAudio.Wave;

    public class Program
    {
        public static void Main(string[] args)
        {
            using (var reader = new WaveFileReader("testSound.wav"))
            {
                using (var writer = new WaveFileWriter("newTestSound.wav", reader.WaveFormat))
                {
                    var count = reader.SampleCount;
                    var processor = new Processor(reader.WaveFormat.SampleRate);
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
        }
    }
}