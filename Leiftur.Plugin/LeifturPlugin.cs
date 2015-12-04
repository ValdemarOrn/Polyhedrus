using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using SharpSoundDevice;
using System.Globalization;
using System.Threading;

namespace Leiftur.Plugin
{
	public class LeifturPlugin : IAudioDevice
	{
		// --------------- IAudioDevice Properties ---------------

		private DeviceInfo devInfo;

		private readonly LeifturNative controller;
		//private CloudSeedView view;
		//private System.Windows.Window window;
		private volatile bool isDisposing;

		public double Samplerate;
		public DeviceInfo DeviceInfo { get { return devInfo; } }
		public Parameter[] ParameterInfo { get; private set; }
		public Port[] PortInfo { get; private set; }
		public int CurrentProgram { get; private set; }
		public int DeviceId { get; set; }
		public IHostInfo HostInfo { get; set; }

		[DllImport("kernel32.dll", SetLastError = true)]
		internal static extern int AllocConsole();

		public LeifturPlugin()
		{
			AllocConsole();
			controller = new LeifturNative(48000, 12003, 12004);
			
			Samplerate = 48000;
			devInfo = new DeviceInfo();
			ParameterInfo = new Parameter[0];
			PortInfo = new Port[1];
		}

		public void InitializeDevice()
		{
#if DEBUG
			devInfo.DeviceID = "Low Profile - Leiftur - DEV";
#else
			devInfo.DeviceID = "Low Profile - Leiftur";
#endif
			devInfo.Developer = "Valdemar Erlingsson";
			devInfo.EditorWidth = 995;
			devInfo.EditorHeight = 386;
			devInfo.HasEditor = true;
			devInfo.Name = "Leiftur Synthesizer";
			devInfo.ProgramCount = 1;
			devInfo.Type = DeviceType.Generator;
			devInfo.Version = 1000;
			devInfo.UnsafeProcessing = true;
			devInfo.VstId = DeviceUtilities.GenerateIntegerId(devInfo.DeviceID);

			PortInfo[0].Direction = PortDirection.Output;
			PortInfo[0].Name = "Stereo Output";
			PortInfo[0].NumberOfChannels = 2;

			for (int i = 0; i < 0; i++)
			{
				var p = new SharpSoundDevice.Parameter();
				p.Display = "0.0";
				p.Index = (uint)i;
				p.Name = "Parameter " + i;
				p.Steps = 0;
				p.Value = 0.0;
				ParameterInfo[i] = p;
			}
		}

		public void DisposeDevice()
		{
			controller.Dispose();
		}

		public void Start()
		{
		}

		public void Stop() { }

		public bool SendEvent(Event ev)
		{
			if (ev.Type == EventType.Midi)
			{
				controller.ProcessMidi(ev.Data as byte[]);
				return true;
			}
			return false;
		}

		public unsafe void ProcessSample(double[][] input, double[][] output, uint bufferSize)
		{
        }

		public void ProcessSample(IntPtr input, IntPtr output, uint inChannelCount, uint outChannelCount, uint bufferSize)
		{
			if (bufferSize > 4096)
				throw new Exception("Buffer size is >1024 sample. CloudSeed does no support buffers this large");
			if (inChannelCount != 0)
				throw new Exception("InChannelCount for Leiftur must be 0");
			if (outChannelCount != 2)
				throw new Exception("OutChannelCount for Leiftur must be 2");

			controller.ProcessAudio(output, (int)bufferSize);
		}

		public void OpenEditor(IntPtr parentWindow)
		{
			
		}

		public void CloseEditor()
		{

		}

		#region SharpSoundDevice Programs

		public void SetProgramData(Program program, int index)
		{
			
		}

		public Program GetProgramData(int index)
		{
			var output = new Program();
			output.Data = new byte[0];
			output.Name = "Empty Program";
			return output;
		}

		#endregion
		
		public void HostChanged()
		{
			var samplerate = HostInfo.SampleRate;
			if (samplerate != Samplerate && samplerate != 44100)
			{
				Samplerate = samplerate;
				controller.Initialize((int)samplerate, 0, 0);
			}
		}
	}
}

