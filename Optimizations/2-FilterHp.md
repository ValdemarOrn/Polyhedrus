Base case:

	PerftFilterHp
	Seconds,TimeMillis
	500,422
	500,426
	500,412
	500,416
	500,420
	500,420
	500,417
	500,421

Lower bound (only update, not processing loop):

	PerftFilterHp
	Seconds,TimeMillis
	500,91
	500,90
	500,90
	500,90
	500,91
	500,90
	500,89
	500,91

After consolidating the loop and using local register variables:

	PerftFilterHp
	Seconds,TimeMillis
	500,373
	500,372
	500,365
	500,374
	500,372
	500,371
	500,370
	500,364

Not a huge improvement, but something measurable.