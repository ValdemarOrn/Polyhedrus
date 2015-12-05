using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Polyhedrus.Plugin
{
    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    unsafe struct WavetableNative
    {
		public int Count;
		public int WavetableDataSize;
		public float* WavetableData;
    }

	public struct Wavetable
	{
		public int Count;
		public int WavetableDataSize;
		public float[] WavetableData;
	}

	public unsafe class PolyhedrusNative
	{
		[DllImport(@"Polyhedrus.Native.dll", CallingConvention = CallingConvention.Cdecl, SetLastError = false, ThrowOnUnmappableChar = false)]
		extern static IntPtr Create();

		[DllImport(@"Polyhedrus.Native.dll", CallingConvention = CallingConvention.Cdecl, SetLastError = false, ThrowOnUnmappableChar = false)]
		extern static void Initialize(IntPtr instance, int samplerate, int udpListenPort, int udpSendPort);

		[DllImport(@"Polyhedrus.Native.dll", CallingConvention = CallingConvention.Cdecl, SetLastError = false, ThrowOnUnmappableChar = false)]
		extern static void SetParameter(IntPtr instance, int parameter, double value);

		[DllImport(@"Polyhedrus.Native.dll", CallingConvention = CallingConvention.Cdecl, SetLastError = false, ThrowOnUnmappableChar = false)]
		extern static void ProcessMidi(IntPtr instance, byte* message);

		[DllImport(@"Polyhedrus.Native.dll", CallingConvention = CallingConvention.Cdecl, SetLastError = false, ThrowOnUnmappableChar = false)]
		extern static void ProcessAudio(IntPtr instance, IntPtr buffer, int bufferSize);

		[DllImport(@"Polyhedrus.Native.dll", CallingConvention = CallingConvention.Cdecl, SetLastError = false, ThrowOnUnmappableChar = false)]
		extern static void Delete(IntPtr instance);

        [DllImport(@"Polyhedrus.Native.dll", CallingConvention = CallingConvention.Cdecl, SetLastError = false, ThrowOnUnmappableChar = false)]
        extern static WavetableNative* GetWavetable(IntPtr instance, int tableIndex);

		[DllImport(@"Polyhedrus.Native.dll", CallingConvention = CallingConvention.Cdecl, SetLastError = false, ThrowOnUnmappableChar = false)]
		public extern static void RunTests();
		
		private static object createLock = new object();
		private IntPtr instance;

		public PolyhedrusNative(int samplerate, int udpPort, int udpSendPort)
		{
			lock (createLock)
			{
				instance = Create();
				Initialize(instance, samplerate, udpPort, udpSendPort);
			}
		}

		~PolyhedrusNative()
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
	        managed.WavetableDataSize = data->WavetableDataSize;
			managed.WavetableData = new float[managed.WavetableDataSize];

			for (int i = 0; i < managed.WavetableDataSize; i++)
			{
				managed.WavetableData[i] = data->WavetableData[i];
			}

			return managed;
        }
    }
}
