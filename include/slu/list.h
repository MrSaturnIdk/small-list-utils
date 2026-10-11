/**
 * @file list.h
 * @brief The entire library
 * @author MrSaturnIdk
 */

/**
 * @mainpage Small List Utils
 *
 * An extremely tiny C99 project for parsing a file with items separated by a delimiter
 *
 * Meant to be linked by CMake's `FetchContent`, also note the target name is \c slu
 *
 * Thread safe, no worries
 *
 * **Not** safe agianst \c '\0' being in the file,
 * though why would it be there in an overly simple data format like this
 *
 * Uses a tokenizer, will ignore empty items (sorry)
 */

#ifndef SLU_LIST_H
#define SLU_LIST_H

#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Return code fancy names
enum SluErrors {
	SLU_OK = 0, ///< Didn't fail at all
	SLU_ERROR_RW = -1, ///< Failed to read/write to the file
	SLU_ERROR_META = -2, ///< Failed to get file metadata, like length
	SLU_ERROR_ALLOC = -3 ///< Failed to allocate memory
};
/**
 * @brief The list type
 * @note Anything prefixed with \c raw_ is not meant for public use
 */
struct SluList {
	char** list; ///< The list (allocated), note every single value is a valid string and some are literals
	char* raw_file_content; ///< An allocated string which the entire file is loaded into
	size_t len; ///< The list length
	size_t raw_space; ///< The amount of space the list actually takes up
};

/**
 * @brief Literally writes \p list to \p file
 *
 * @param file The file to write to, respects already set position
 * @pre \p file is a valid file
 * @note \p file is not cleared or reverted on error (since if it already failed, it may as well fail again)
 * @param delim The thing splitting items
 * @return Either \c SLU_ERROR_RW or \c SLU_OK
 */
enum SluErrors slu_write(const struct SluList* restrict list, FILE* restrict file, const char* restrict delim);
/**
 * @brief Get a list
 *
 * @param out What to set the list to, should be a pointer to a null pointer (or a pointer to a stack pointer).
 * Since it is a plain reassignment, passing a pointer to a heap pointer is a leak
 * Will retain its original value if error, otherwise a valid list
 * @param file The file to read, respects already set position and treats it like the beginning in length calculations
 * @pre \p file is a valid file
 * @note \p file is not cleared or reverted on error (since if it already failed, it may as well fail again)
 * @param delim The thing splitting items
 * @return Either `SLU_OK`, `SLU_ERROR_RW`, `SLU_ERROR_META`, or \c SLU_ERROR_ALLOC
 */
enum SluErrors slu_read(struct SluList* restrict* restrict out, FILE* restrict file, const char* restrict delim);
/**
 * @brief Frees the heap objects
 *
 * @param list The list to be closed, can be null pointer for noop
 */
void slu_close(struct SluList* list);

/**
 * @brief Checks if \p list has \p target
 *
 * @return \c 1 if it does include it, \c 0 if otherwise
 */
int slu_has(const struct SluList* restrict list, const char* restrict target);
/**
 * @brief Appends \p item to \p list and increments length
 *
 * @return either \c SLU_OK or `SLU_ERROR_ALLOC`, list remains untouched on fail
 */
enum SluErrors slu_add(struct SluList* restrict list, char* restrict item);
/// @brief Removes \p item, sets every item in \p list back 1 (no empty item), and deincrements the legnth
void slu_remove(struct SluList* restrict list, const char* restrict item);
/// @brief Sorts \p list using \p sorter (passed to `qsort()`)
void slu_sort(struct SluList* list, int (*sorter)(const void *, const void *));

#ifdef __cplusplus
}
#endif

#endif
