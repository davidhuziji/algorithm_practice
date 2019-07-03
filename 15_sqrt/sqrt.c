/*
 * Copyright (c) 2019 Hu Ziji. All rights reserved.
 * Auther: Hu Ziji <davidhuzj@gmail.com>
 */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define DATA_LIMIT		0x100
#define LEN_DIGITS		0x6

/*
 * XXX
 * Try to implement the sqrt search by using the integer. However, each time
 * to search the next digit, the sqrt has to be multipled with 10 and then the
 * data has to be multipled with 100.
 * Thus after searching 4 digits behind dot, the data will exceed the limitation
 * of uint32_t.
 * Thus without an optimization, it is unlikely to use integer to implement the
 * sqrt search.
 * As a result, implement the sqrt search by using float type.
 */

static void binary_sqrt(uint32_t data)
{
	float start = 0, end = (float)data, mid, step = 1;
	float square, large_square;
	int i;

	for (i = 0; i < LEN_DIGITS; i++)
		step /= 10;

	while (start <= end) {
		mid = start + (end - start) / 2;
		square = mid * mid;
		large_square = mid * mid + 2 * step * mid;

		if ((square <= (float)data) && (large_square > (float)data))
			break;

		if (square < (float)data)
			start = mid + step;
		else
			end = mid - step;
	}

	printf("The sqrt is %f.\n", mid);
}

int main(void)
{
	uint32_t data;
	int i, round = 0x5;

	srand(time(NULL));

	for (i = 0; i < round; i++) {
		data = rand() % DATA_LIMIT;
		printf("Try to get the sqrt value of %d\n", data);

		binary_sqrt(data);
	}

	return 0;
}