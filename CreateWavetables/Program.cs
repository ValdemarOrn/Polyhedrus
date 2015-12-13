using OxyPlot;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using LowProfile.Visuals;
using LowProfile.Core.Extensions;
using LowProfile.Fourier.Double;
using System.IO;
using System.Threading;
using System.Globalization;
using System.Drawing;

namespace CreateWavetables
{
	class Program
	{
		private static int? CurrentWaveSize;

		[STAThread]
		static void Main(string[] args)
		{
            Thread.CurrentThread.CurrentCulture = CultureInfo.InvariantCulture;
			
			var dir = args.FirstOrDefault(x => Directory.Exists(x));
			var file = args.FirstOrDefault(x => File.Exists(x));
			var fileList = new List<string>();

			if (dir != null)
			{
				Console.WriteLine($"Processing all .wt and .wav files in directory {dir}");
				fileList.AddRange(Directory.GetFiles(dir, "*.wt", SearchOption.AllDirectories));
				fileList.AddRange(Directory.GetFiles(dir, "*.wav", SearchOption.AllDirectories));
			}
			else if (file != null)
			{
				Console.WriteLine($"Processing single file {file}");
				fileList.Add(file);
			}
			else
			{
				Console.WriteLine("You must specify a directory or a file to process");
				Console.WriteLine("Press enter to exit...");
				Console.ReadLine();
				return;
			}

			Console.WriteLine("---------------");
			Console.WriteLine($"Starting process of {fileList.Count} files");
			Console.WriteLine("---------------\n");

			foreach (var filePath in fileList)
			{
				try
				{
					Console.WriteLine($"Processing file {filePath}");

					if (Path.GetExtension(filePath) == ".wt")
					{
						var data = File.ReadAllBytes(filePath);

						if (IsSurgeWt(data))
						{
							Console.WriteLine("Reading Vember Surge Wavetable");
							ConvertSurgeFile(data, filePath.Substring(0, filePath.Length - 3) + ".wav");
						}
						else
						{
							Console.WriteLine("Reading Terratec Komplexer Wavetable");
							ConvertKomplexerFile(data, filePath.Substring(0, filePath.Length - 3) + ".wav", true);
						}
					}
					else if (Path.GetExtension(filePath) == ".wav")
					{
						Console.WriteLine("Converting .wav file to new format");

						var size = GetWaveSizeForWav(filePath);
						var newPath = Path.Combine(
							Path.GetDirectoryName(filePath),
							"Polyhedrus-Converted",
							Path.GetFileName(filePath));

                        ConvertWaveFile(size, filePath, newPath);
					}
				}
				catch (Exception ex)
				{
					Console.WriteLine($"Failed to convert file {file}, error:\n{ex.Message}\n\n");
				}
			}

			Console.WriteLine("Press enter to exit...");
			Console.ReadLine();
		}

		private static int GetWaveSizeForWav(string filePath)
		{
			var chunks = new List<byte[]>();
			AudioLib.WaveFiles.ReadWaveFile(filePath, ref chunks);
			var clmChunk = chunks.FirstOrDefault(x => Encoding.ASCII.GetString(x, 0, 4) == "clm ");

			int size = -1;
			// Check if it's a serum wavetable, then read the metadata
			if (clmChunk != null && Encoding.ASCII.GetString(clmChunk).Contains("xferrecords.com"))
			{
				var str = Encoding.ASCII.GetString(clmChunk, 11, 6);
				str = str.Split(' ').First().Trim();
				size = int.Parse(str);
			}
			else
			{
				string current = "";
				if (CurrentWaveSize.HasValue)
					current = $"({CurrentWaveSize})";

                Console.Write($"Specify the size of each wave in the table {current}: ");
				var sizeString = Console.ReadLine();

				if (string.IsNullOrWhiteSpace(sizeString) && CurrentWaveSize.HasValue)
				{
					size = CurrentWaveSize.Value;
				}
				else
				{
					size = int.Parse(sizeString);
					CurrentWaveSize = size;
				}
			}

			return size;
		}

		private static bool IsSurgeWt(byte[] data)
		{
			return data[0] == 'v' && data[1] == 'a' && data[2] == 'w' && data[3] == 't';
		}

		private static void ConvertSurgeFile(byte[] data, string outputFile)
		{
			/*
			Vember were kind enough to provide the data format as part of the synth... but it's all hogwash and completely wrong!
			
			Original txt:

			byte
			0-3	'wavt'		as text/big-endian-
			the rest of the data is in little-endian types
			4-7	wave_size 	size of each wave, max: 1024 (v1.2)
			8-9	wave_count 	number of waves, max: 512 (v1.2)
			10-11	flags
			12-end	wave data in float32 format. 4*wave_size*wave_count bytes

			Corrected info:

			All ittle endian
			0-3	'vawt'
			4-7	wave_size 	size of each wave, max: 1024 (v1.2)
			8-9	wave_count 	number of waves, max: 512 (v1.2)
			10-11 flags
			12-end wave data in signed 16-bit int format. 2 * wave_size * wave_count bytes

			Yeah, I don't know WTF was up with that...
			*/

			var size = BitConverter.ToInt32(data, 4);
			var count = BitConverter.ToInt16(data, 8);
			var totalBytes = 2 * size * count;
			var dataCount = data.Length - 12;

			if (totalBytes != dataCount)
			{
				Console.WriteLine($"Vember Surge wavetable does not seem to be valid. Got size: {size} and count: {count}, but that does not match size of actual data");

				if (dataCount % (size * 2) == 0)
				{
					Console.WriteLine("data size is a modulo of the wave size. Attempting to convert data, but it may not be correct!");
				}
				else
				{
					Console.WriteLine($"Cannot process this wavetable at all");
					return;
				}
			}

			var sampleData = data.Skip(12).Chunk(2).Select(x => BitConverter.ToInt16(x.ToArray(), 0)).ToArray();
			var wavetable = new List<double>();

			foreach (var waveData in sampleData.Chunk(size))
			{
				var wave = waveData.Select(x => (double)x).ToArray();
				var dft = AudioLib.SimpleDFT.DFT(wave);
				var newWave = AudioLib.SimpleDFT.IDFT(dft, 2048);
				var max = newWave.Max(x => Math.Abs(x));
				newWave = newWave.Select(x => x / max).ToArray();
				wavetable.AddRange(newWave);
			}

			WriteWavetable(wavetable, outputFile);
		}

		private static void ConvertWaveFile(int size, string path, string outputFile)
		{
			var sampleData = AudioLib.WaveFiles.ReadWaveFile(path).First();
			var wavetable = new List<double>();
			var chunks = sampleData.Chunk(size).Where(x => x.Count == size).ToArray();

			chunks
				.AsParallel()
				.AsOrdered()
				.Select(waveData =>
				{
					var wave = waveData.Select(x => (double)x).ToArray();
					var dft = AudioLib.SimpleDFT.DFT(wave);
					var newWave = AudioLib.SimpleDFT.IDFT(dft, 2048);
					var max = newWave.Max(x => Math.Abs(x));
					newWave = newWave.Select(x => x / max).ToArray();
					Console.Write(".");
					return newWave;
				})
				.ToList()
				.Foreach(x => wavetable.AddRange(x));

			Console.WriteLine("");
			WriteWavetable(wavetable, outputFile);
		}

		/// <summary>
		/// Reads Terratex Komplexer wavetables
		/// </summary>
		/// <param name="data">the raw byte data from the .wt file</param>
		/// <param name="outputFile">the path where to save the output wavetable</param>
		/// <param name="reduceNoise">when set to true, will treat very low volume partials as silent (some WTs have been generated from DFT of audio signals, and have noise)</param>
		private static void ConvertKomplexerFile(byte[] data, string outputFile, bool reduceNoise)
        {
		    var output = new List<double>();
		    foreach (var bytes in data.Chunk(64 * 4))
		    {
			    var partials = AudioLib.BufferConverter.ToFloat(bytes.ToArray()).Select(x => (double)(x)).ToArray();
			    var maxPart = partials.Max();
			    partials = partials.Select(x => x / maxPart).Select(x => reduceNoise && (Math.Abs(x) < 0.013) ? 0.0 : x).ToArray();
			    var wave1 = MakeWave(partials);
			    output.AddRange(wave1);
		    }

			WriteWavetable(output, outputFile);
        }
		
		private static double[] MakeWave(double[] partials)
        {
            var wave = new double[2048];
            for (int n = 0; n < partials.Length; n++)
            {
                var g = partials[n];

                for (int i = 0; i < wave.Length; i++)
                {
                    wave[i] += Math.Sin((n + 1) * (i / (double)wave.Length * 2 * Math.PI) + Math.PI) * g;
                }
            }

	        var max = wave.Max(x => Math.Abs(x));
	        wave = wave.Select(x => x / max).ToArray();
            return wave;
        }

		private static void WriteWavetable(IEnumerable<double> wavetable, string outputFile)
		{
			Directory.CreateDirectory(Path.GetDirectoryName(outputFile));
			var stereoData = new double[1][];
			stereoData[0] = wavetable.ToArray();
			AudioLib.WaveFiles.WriteWaveFile(stereoData, AudioLib.WaveFiles.WaveFormat.PCM24Bit, 48000, outputFile);
		}
	}
}
