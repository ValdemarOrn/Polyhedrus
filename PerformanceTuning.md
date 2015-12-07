

Initial Testcase, Debug Mode, 32 Bit, no inlining, no optimization

	Voices,Seconds,TimeMillis
	1,10,2416
	10,2,4531

Release mode, Whole program optimization, Maximize speed, favor fast code, no SSE, fp:precise

	Voices,Seconds,TimeMillis
	1,10,476
	10,2,883

Inline any suitable, fast floating point

	Voices,Seconds,TimeMillis
	1,10,325
	10,2,596

SSE2 enabled

	Voices,Seconds,TimeMillis
	1,10,258
	10,2,491

AVX enabled

	Voices,Seconds,TimeMillis
	1,10,284
	10,2,509

The AVX is a little slower than the SSE2 version. to make sure it wasn't a fluke, and that something else was interfering with the test, I turned it back down to SSE2, and got lower numbers again.

We've now got to the end of the road where the compiler can help us. But since the code is quite fast, I upped the values to 1 voice for 20 seconds and 20 voices for 2 seconds:

SSE2 Enabled

	Voices,Seconds,TimeMillis
	1,20,523
	20,2,995

The task is to perform general house cleaning. Clean up anything that looks obviously out of place, before getting into the micro-optimizations. I have not yet ran a performance analysis of the code, I am simply looking for egregious failures in efficiency.

The first obvious bit that I did was to split up the Character section into 3 separate loops.

	Voices,Seconds,TimeMillis
	1,20,516
	20,2,985

A small gain, but it was measurable consistently.

The rest of the clean-ups were not interesting, but they allow me to more aggressively optimize the rest of the code.

After this, the current performance stands at:

	Voices,Seconds,TimeMillis
	1,20,500
	20,2,958

A tiny measurable speed improvement still.
