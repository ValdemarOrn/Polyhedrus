#ifndef AUDIOLIB_DECIMATOR
#define AUDIOLIB_DECIMATOR


namespace AudioLib
{
	class Decimator
	{
	private:
		float values[19];

	public:
		static float Coeff[19];
		
		inline Decimator()
		{
			for (int i = 0; i < 19; i++)
				values[i] = 0;
		}

		inline float Process(float x0, float x1)
		{
			values[0] = values[2];
			values[1] = values[3];
			values[2] = values[4];
			values[3] = values[5];
			values[4] = values[6];
			values[5] = values[7];
			values[6] = values[8];
			values[7] = values[9];
			values[8] = values[10];
			values[9] = values[11];
			values[10] = values[12];
			values[11] = values[13];
			values[12] = values[14];
			values[13] = values[15];
			values[14] = values[16];
			values[15] = values[17];
			values[16] = values[18];
			values[17] = x0;
			values[18] = x1;

			// This is faster than the vectorized version
			float output = 
				(
					(
						Coeff[0] * values[0] +
						Coeff[1] * values[1] +
						Coeff[2] * values[2] +
						Coeff[3] * values[3] +
						Coeff[4] * values[4]
					) +
					(
						Coeff[5] * values[5] +
						Coeff[6] * values[6] +
						Coeff[7] * values[7] +
						Coeff[8] * values[8]
					)
				) +
				(
					(
						Coeff[9] * values[9] +
						Coeff[10] * values[10] +
						Coeff[11] * values[11] +
						Coeff[12] * values[12] +
						Coeff[13] * values[13]
					) +
					(
						Coeff[14] * values[14] +
						Coeff[15] * values[15] +
						Coeff[16] * values[16] +
						Coeff[17] * values[17] +
						Coeff[18] * values[18]
					)
				);

			return output;
		}
	};
}

#endif
