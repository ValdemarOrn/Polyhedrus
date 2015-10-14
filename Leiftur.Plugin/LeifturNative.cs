using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Leiftur.Plugin
{
    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    unsafe struct WavetableNative
    {
        public int Partials;
		public int Count;
		public int SampleSize;
		public float* WavetableData;
    }

	public struct Wavetable
	{
		public int Partials;
		public int Count;
		public int SampleSize;
		public float[] WavetableData;
	}

	public unsafe class LeifturNative
	{
		[DllImport(@"Leiftur.Native.dll", CallingConvention = CallingConvention.Cdecl, SetLastError = false, ThrowOnUnmappableChar = false)]
		extern static IntPtr Create();

		[DllImport(@"Leiftur.Native.dll", CallingConvention = CallingConvention.Cdecl, SetLastError = false, ThrowOnUnmappableChar = false)]
		extern static void Initialize(IntPtr instance, int samplerate, int udpListenPort, int udpSendPort);

		[DllImport(@"Leiftur.Native.dll", CallingConvention = CallingConvention.Cdecl, SetLastError = false, ThrowOnUnmappableChar = false)]
		extern static void SetParameter(IntPtr instance, int parameter, double value);

		[DllImport(@"Leiftur.Native.dll", CallingConvention = CallingConvention.Cdecl, SetLastError = false, ThrowOnUnmappableChar = false)]
		extern static void ProcessMidi(IntPtr instance, byte* message);

		[DllImport(@"Leiftur.Native.dll", CallingConvention = CallingConvention.Cdecl, SetLastError = false, ThrowOnUnmappableChar = false)]
		extern static void ProcessAudio(IntPtr instance, IntPtr buffer, int bufferSize);

		[DllImport(@"Leiftur.Native.dll", CallingConvention = CallingConvention.Cdecl, SetLastError = false, ThrowOnUnmappableChar = false)]
		extern static void Delete(IntPtr instance);

        [DllImport(@"Leiftur.Native.dll", CallingConvention = CallingConvention.Cdecl, SetLastError = false, ThrowOnUnmappableChar = false)]
        extern static WavetableNative* GetWavetable(IntPtr instance, int tableIndex);

		[DllImport(@"Leiftur.Native.dll", CallingConvention = CallingConvention.Cdecl, SetLastError = false, ThrowOnUnmappableChar = false)]
		public extern static void RunTests();
		
		private static object createLock = new object();
		private IntPtr instance;

		public LeifturNative(int samplerate, int udpPort, int udpSendPort)
		{
			lock (createLock)
			{
				instance = Create();
				Initialize(instance, samplerate, udpPort, udpSendPort);
			}
		}

		~LeifturNative()
		{
			Delete(instance);
		}

		public void Dispose()
		{
			Delete(instance);
			GC.SuppressFinalize(this);
		}

		public void Initialize(int samplerate, int udpPort, int udpSendPort)
		{
			Initialize(instance, samplerate, udpPort, udpSendPort);
		}

		public void SetParameter(int parameter, double value)
		{
			SetParameter(instance, parameter, value);
		}

		public void ProcessMidi(byte[] message)
		{
			fixed (byte* msg = message)
			{
				ProcessMidi(instance, msg);
			}
		}

		public void ProcessAudio(IntPtr buffer, int bufferSize)
		{
			ProcessAudio(instance, buffer, bufferSize);
		}

        public Wavetable GetWavetable(int tableIndex)
        {
			WavetableNative* data = GetWavetable(instance, tableIndex);

			Wavetable managed = new Wavetable();
			managed.Count = data->Count;
			managed.Partials = data->Partials;
			managed.SampleSize = data->SampleSize;
			var size = managed.Partials * managed.Count * managed.SampleSize;
			managed.WavetableData = new float[size];

			for (int i = 0; i < size; i++)
			{
				managed.WavetableData[i] = data->WavetableData[i];
			}

			return managed;
        }
    }
}
