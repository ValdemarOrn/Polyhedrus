I opted to profile the init preset, as it's a good representation of the "average patch". It contains 1x osc, character, hp Filter and main filter enabled, amp and filter envelope and 1 modulator.

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

## Profiling

I ran an instrumentation profiler on the performance test. Annoyingly, the CPU sampling profiler is completely crippled on Windows 8, as new security features in the kernel mean you cannot increase the sampling frequency above the standar (once every 10Mhz, I think). The Instrumentation still does provide a good estimate.

Unfortunately, the profile show nothing that I didn't already expect. 

**Synth.Process**

	Voice.Process: 74.6%
	Decimator.Process: 1.7%
	ModMatrix.Process: 1.6%
	Other: 1%

**Voice.Process**

	FilterMain.Process: 26.9%
	FilterHp.Process: 12.4%
	Character.Process: 12.0%
	NoiseSimple.Process: 10.2% (wtf??)
	Other: 11.8%

I won't be focusing specifically on the hot spots, but these are still good figures to know. The next step is to start applying optimizations inside these elements, and the elements they call.

## Optimization of modules

Based on eBook found here: http://www.agner.org/optimize/

Already performed:

* No divisions in critical path.
* code operates on 8-16 samples at a time, not a per-sample basis.
* no virtual calls
* no exceptions or try/catch in critical path

Contrary to popular belief, VS does support vectorization. Use QVec flag to see the results.

1. Mark frequently used variables in tight loop as "register" variables (7.1)
1. Look out for post-increment values whose result is used in the loop (7.2) - prefix is actually often slower due to pipelining!
1. Make sure that denormal flushing is working as expected (7.3)
1. use uint8_t instead of bool to prevent overdetermined values (7.5)
1. Look out for integer to float conversion, consider using a separate float value. Make sure int is signed (7.11)
1. Where casting float to int, make sure the code is vectorized and uses SSE (7.11)
1. Optimize branched code with lookup tables, or remove branching via other means (7.12)
1. Unroll loops where it may be beneficial (7.13)
1. Make sure loop boundaries are constant (7.13)
1. Make sure any copying or setting memory is replaced by the compiler by memset/memcpy (should happen automatically unless something prevents it) (7.13)
1. Look for possibilities of improving code locality (7.14)
1. Make sure finding sign of floats is done efficiently (check last bit) (7.24, 14.9)
1. Make sure constant folding is done where possible (8.1)
1. Mark any const values as such (8.6)
1. Reorder variable layout where it may improve caching (9.4)
1. mark all memory allocation of buffers as aligned to 16 byte boundaries - prepares for vectorization (9.5-9.6)
1. look for non-sequential access of data (9.9)
1. Improve out of order execution for float values (11.0)
1. Split up loops depending on prior data whereever possible - as long as the loop cannot be vectorized with a MAC operation (11.0)
1. Analyse vectorization done by compiler, aid it where possible to improve. (12.0)
1. Use lookup tables where there are if/else, switch/case branches that cannot be simplified (14.1)
1. Use smart double bounds checking (14.2)
1. Move floating point division outside of loops, use compile constant where possible (14.6)
1. Ensure no mixing of float and double operations (14.7)
1. Replace circular buffers with base-2 values, use bitmasking instead of if(overflow) or modulo

**Final steps:**

* Code locality - improve code caching by computing the same bit of code for all voices (9.3)
* Profile Guided optimization

