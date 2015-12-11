First I had to rewrite the noise module somewhat, as it was fairly shit and was missing the ability to mix between different types of noise.

Initial results:

	PerfNoise
	Seconds,TimeMillis
	500,431
	500,447
	500,430
	500,428
	500,425
	500,424
	500,427
	500,429

My initial attempt was going to be pre-computing 1 second buffers of noise, but the cycle hum was too noticeable, so I had to continue to process everything in the main audio loop.

I also experimented with adding a more efficient GetFloats() method to the RNG, but this did not seem to improve performance at all; in fact it was slightly slower, even when using SSE instructions to cast the random integers to floats in a vectorized manner.

One optimization I was able to do was to use 31 bit precision in the NextFloat() method in the RNG. This meant I could use signed ints, and as one of the optimization points stated, unsigned ints take longer to convert. This seems to hold true:

	PerfNoise
	Seconds,TimeMillis
	500,402
	500,416
	500,402
	500,395
	500,400
	500,400
	500,402
	500,400
	500,392
