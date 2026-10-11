/// Small helper for handling the list (source)

#ifdef _WIN32
#	define strtok_r strtok_s
#	define strdup _strdup
#else
#	define _POSIX_C_SOURCE 200809L
#endif

#include "slu/list.h"

#include "array.c"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


enum SluErrors slu_write(const struct SluList* restrict list, FILE* restrict file, const char* restrict delim) {
	size_t catListSize = 0;
	for (size_t i = 0; i < list->len; ++i) {
		catListSize += (size_t)(strlen(list->list[i]) + 1);
		catListSize += (size_t)(strlen(delim) + 1);
	}
	char* catList = calloc(catListSize, sizeof(char));
	if (!catList) {
		return SLU_ERROR_ALLOC;
	}

	for (size_t i = 0; i < list->len; ++i) {
		size_t size = strlen(catList);
		strncat(catList, list->list[i], catListSize - size - 1);
		size = strlen(catList);
		strncat(catList, delim, catListSize - size - 1);
	}

	size_t size = strlen(catList);
	size_t written = fwrite(catList, size, 1, file);
	if (!written) {
		free(catList);
		return SLU_ERROR_RW;
	}
	free(catList);
	return SLU_OK;
}
enum SluErrors slu_read(struct SluList* restrict* restrict out, FILE* restrict file, const char* restrict delim) {
	struct SluList* list = calloc(1, sizeof(struct SluList));
	char* items = NULL;
	char* itemsCopy = NULL;
	char** arrayList = NULL;
	enum SluErrors err = SLU_OK;
	if (!list) {
		err = SLU_ERROR_ALLOC;
		goto cleanup;
	}

	// Get file length
	long startPos = ftell(file);
	if (startPos == -1L) {
		err = SLU_ERROR_META;
		goto cleanup;
	}
	if (fseek(file, 0L, SEEK_END)) {
		err = SLU_ERROR_META;
		goto cleanup;
	}
	long endPos = ftell(file);
	if (endPos == -1L) {
		err = SLU_ERROR_META;
		goto cleanup;
	}
	if (fseek(file, startPos, SEEK_SET)) {
		err = SLU_ERROR_META;
		goto cleanup;
	}
	if (endPos < startPos) {
		err = SLU_ERROR_META;
		goto cleanup;
	}
	size_t fileLength = (size_t)(endPos - startPos);

	// Read
	items = calloc(fileLength + 1, sizeof(char));
	if (!items) {
		err = SLU_ERROR_ALLOC;
		goto cleanup;
	}
	fread(items, sizeof(items[0]), fileLength, file);
	if (ferror(file) || feof(file)) {
		err = SLU_ERROR_RW;
		goto cleanup;
	}

	// Get array size
	size_t i = 0;
	itemsCopy = strdup(items);
	if (!itemsCopy) {
		err = SLU_ERROR_ALLOC; // You can't tell me this isn't allocation
		goto cleanup;
	}
	char *saveptr = NULL;
	for (char* item = strtok_r(itemsCopy, delim, &saveptr); item; item = strtok_r(NULL, delim, &saveptr)) {
		++i;
	}
	free(itemsCopy);
	itemsCopy = NULL;
	size_t itemAmount = i;

	// Turn into array
	arrayList = calloc(itemAmount, sizeof(char*));
	if (!arrayList) {
		err = SLU_ERROR_ALLOC;
		goto cleanup;
	}
	// For slu_has()
	for (i = 0; i < itemAmount; ++i) {
		arrayList[i] = "";
	}

	i = 0;
	for (char* item = strtok_r(items, delim, &saveptr); item; item = strtok_r(NULL, delim, &saveptr)) {
		arrayList[i++] = item;
	}
	list->list = arrayList;
	list->len = i;
	list->raw_space = i;
	list->raw_file_content = items;
	*out = list;
	return SLU_OK;

	cleanup: {
		free(list);
		free(items);
		free(itemsCopy);
		free(arrayList);
		return err;
	}
}
void slu_close(struct SluList* list) {
	if (!list) {
		return;
	}
	free(list->list);
	free(list->raw_file_content);
	free(list);
}

int slu_has(const struct SluList* restrict list, const char* restrict target) {
	if (findIndexOfString((const char* const*)list->list, list->len, target) != -1) {
		return 1; // Found
	}
	return 0; // Not found
}
enum SluErrors slu_add(struct SluList* restrict list, char* restrict item) {
	char** tempList = NULL;
	if (list->len + 1 > list->raw_space) {
		tempList = realloc(list->list, (size_t)(list->len + 1) * sizeof(char*));
		if (!tempList) {
			return SLU_ERROR_ALLOC;
		}
		list->list = tempList;
		++list->raw_space;
	}
	list->list[list->len] = item;
	++list->len;
	return SLU_OK;
}
void slu_remove(struct SluList* restrict list, const char* restrict item) {
	size_t found = (size_t)findIndexOfString((const char* const*)list->list, list->len, item);
	for (size_t i = found; i < list->len; ++i) {
		if (list->len != i + 1) {
			list->list[i] = list->list[i + 1];
		}
	}
	--list->len;
}
void slu_sort(struct SluList* list, int (*comp)(const void *, const void *)) {
	qsort(list->list, list->len, sizeof(char*), comp);
}

