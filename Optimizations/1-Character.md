### Character Section

Initial Results from test:

	PerftCharacter
	Seconds,TimeMillis
	500,693
	500,675
	500,681
	500,679
	500,679
	500,683
	500,683

with audio processing loop commented out (only update runs, this is lower bound on performance)

	PerftCharacter
	Seconds,TimeMillis
	500,208
	500,221
	500,211
	500,212
	500,204

after (implicit) vectorization of Reduce loops:

	PerftCharacter
	Seconds,TimeMillis
	500,596
	500,595
	500,602
	500,608
	500,610
	500,609
	500,605

after (explicit) vectorization of Clip loops:

	PerftCharacter
	Seconds,TimeMillis
	500,547
	500,549
	500,526
	500,530
	500,539
	500,528
	500,537

after improving Out of order execution in Biquad:

	litte/no change

So far, about a 30% speed improvement in the processing loop.

Profiler shows the if() statement in the decimation loop as being fairly costly. My previous attempts to eliminate it proved more expensive.

Unfortunately that's as low as I seem to be able to get it for now.