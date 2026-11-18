/** One helper for arrays */

#ifndef UTILS_ARRAY_C
#define UTILS_ARRAY_C

#include <stddef.h>
#include <string.h>

static int findIndexOfString(const char* const array[], size_t len, const char* item) {
	size_t i = 0;

	for (i = 0; i < len; ++i) {
		if (!strcmp(array[i], item)) {
			return (int)i;
		}
	}
	return -1;
}

#endif
