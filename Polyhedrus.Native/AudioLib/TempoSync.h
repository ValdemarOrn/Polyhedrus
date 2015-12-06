#ifndef AUDIOLIB_TEMPOSYNC
#define AUDIOLIB_TEMPOSYNC

#include <string>

namespace AudioLib
{
	enum class Quantization
	{
		_1_64t = 0,
		_1_64 = 1,
		_1_64d,

		_1_32t,
		_1_32,
		_1_32d,

		_1_16t,
		_1_16,
		_1_16d,

		_1_8t,
		_1_8,
		_1_8d,

		_1_4t,
		_1_4,
		_1_4d,

		_1_2t,
		_1_2,
		_1_2d,

		_1t,
		_1,
		_1d,

		_2t,
		_2,
		_2d,

		_4t,
		_4,
		_4d,
	};

	class TempoSync
	{
	public:
		inline static double GetSamplesPerNote(Quantization quantization, double bpm, double samplerate)
		{
			double quarterNotesPerSecond = bpm / 60.0;
			double notesPerSecond = 1;

			switch (quantization)
			{
			case Quantization::_1_64t: notesPerSecond = quarterNotesPerSecond * 16 * 3.0 / 2.0; break;
			case Quantization::_1_64: notesPerSecond = quarterNotesPerSecond * 16; break;
			case Quantization::_1_64d: notesPerSecond = quarterNotesPerSecond * 16 * 2.0 / 3.0; break;

			case Quantization::_1_32t: notesPerSecond = quarterNotesPerSecond * 8 * 3.0 / 2.0; break;
			case Quantization::_1_32: notesPerSecond = quarterNotesPerSecond * 8; break;
			case Quantization::_1_32d: notesPerSecond = quarterNotesPerSecond * 8 * 2.0 / 3.0; break;

			case Quantization::_1_16t: notesPerSecond = quarterNotesPerSecond * 4 * 3.0 / 2.0; break;
			case Quantization::_1_16: notesPerSecond = quarterNotesPerSecond * 4; break;
			case Quantization::_1_16d: notesPerSecond = quarterNotesPerSecond * 4 * 2.0 / 3.0; break;

			case Quantization::_1_8t: notesPerSecond = quarterNotesPerSecond * 2 * 3.0 / 2.0; break;
			case Quantization::_1_8: notesPerSecond = quarterNotesPerSecond * 2; break;
			case Quantization::_1_8d: notesPerSecond = quarterNotesPerSecond * 2 * 2.0 / 3.0; break;

			case Quantization::_1_4t: notesPerSecond = quarterNotesPerSecond * 1 * 3.0 / 2.0; break;
			case Quantization::_1_4: notesPerSecond = quarterNotesPerSecond * 1; break;
			case Quantization::_1_4d: notesPerSecond = quarterNotesPerSecond * 1 * 2.0 / 3.0; break;

			case Quantization::_1_2t: notesPerSecond = quarterNotesPerSecond * 0.5 * 3.0 / 2.0; break;
			case Quantization::_1_2: notesPerSecond = quarterNotesPerSecond * 0.5; break;
			case Quantization::_1_2d: notesPerSecond = quarterNotesPerSecond * 0.5 * 2.0 / 3.0; break;

			case Quantization::_1t: notesPerSecond = quarterNotesPerSecond * 0.25 * 3.0 / 2.0; break;
			case Quantization::_1: notesPerSecond = quarterNotesPerSecond * 0.25; break;
			case Quantization::_1d: notesPerSecond = quarterNotesPerSecond * 0.25 * 2.0 / 3.0; break;

			case Quantization::_2t: notesPerSecond = quarterNotesPerSecond * 0.0125 * 3.0 / 2.0; break;
			case Quantization::_2: notesPerSecond = quarterNotesPerSecond * 0.0125; break;
			case Quantization::_2d: notesPerSecond = quarterNotesPerSecond * 0.0125 * 2.0 / 3.0; break;

			case Quantization::_4t: notesPerSecond = quarterNotesPerSecond * 0.0625 * 3.0 / 2.0; break;
			case Quantization::_4: notesPerSecond = quarterNotesPerSecond * 0.0625; break;
			case Quantization::_4d: notesPerSecond = quarterNotesPerSecond * 0.0625 * 2.0 / 3.0; break;
			}

			return samplerate / notesPerSecond;
		}

		inline static std::string ToString(Quantization quantization)
		{
			switch (quantization)
			{
			case Quantization::_1_64t: return "1/64 Triplet";
			case Quantization::_1_64: return "1/64";
			case Quantization::_1_64d: return "1/64 Dotted";

			case Quantization::_1_32t: return "1/32 Triplet";
			case Quantization::_1_32: return "1/32";
			case Quantization::_1_32d: return "1/32 Dotted";

			case Quantization::_1_16t: return "1/16 Triplet";
			case Quantization::_1_16: return "1/16";
			case Quantization::_1_16d: return "1/16 Dotted";

			case Quantization::_1_8t: return "1/8 Triplet";
			case Quantization::_1_8: return "1/8";
			case Quantization::_1_8d: return "1/8 Dotted";

			case Quantization::_1_4t: return "1/4 Triplet";
			case Quantization::_1_4: return "1/4";
			case Quantization::_1_4d: return "1/4 Dotted";

			case Quantization::_1_2t: return "1/2 Triplet";
			case Quantization::_1_2: return "1/2";
			case Quantization::_1_2d: return "1/2 Dotted";

			case Quantization::_1t: return "1/1 Triplet";
			case Quantization::_1: return "1/1";
			case Quantization::_1d: return "1/1 Dotted";

			case Quantization::_2t: return "2/1 Triplet";
			case Quantization::_2: return "2/1";
			case Quantization::_2d: return "2/1 Dotted";

			case Quantization::_4t: return "4/1 Triplet";
			case Quantization::_4: return "4/1";
			case Quantization::_4d: return "4/1 Dotted";
			}

			return "---";
		}
	};
}

#endif

