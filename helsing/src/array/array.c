// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (c) 2021 Pierro Zachareas
 */

#include "configuration.h"
#include "configuration_adv.h"

#ifdef PROCESS_RESULTS
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "llnode.h"
#endif

#if defined(STORE_RESULTS) && defined(CHECKSUM_RESULTS)
#include <openssl/evp.h>
#include "hash.h"
#endif

#if defined(PROCESS_RESULTS) && SANITY_CHECK
#include <assert.h>
#endif

#if defined(STORE_RESULTS) && defined(PRINT_RESULTS)
#include <stdio.h>
#endif

#ifdef STORE_RESULTS
void array_free(struct array *ptr)
{
	if (ptr == NULL)
		return;

	free(ptr->data);
	free(ptr);
}
#endif

#ifdef PROCESS_RESULTS
static void swap(vamp_t x, vamp_t y, vamp_t *arr)
{
	vamp_t tmp = arr[x];
	arr[x] = arr[y];
	arr[y] = tmp;
}

static vamp_t partition(vamp_t lo, vamp_t hi, vamp_t *arr)
{
	vamp_t pivot = arr[hi];
	vamp_t i = lo;
	for (vamp_t j = lo; j <= hi; j++) {
		if (arr[j] < pivot) {
			swap(i, j, arr);
			i++;
		}
	}
	swap(i, hi, arr);
	return i;
}

static void quickSort(vamp_t lo, vamp_t hi, vamp_t *arr)
{
	if (hi <= lo)
		return;

	vamp_t partitionPoint = partition(lo, hi, arr);
	if (partitionPoint > 0)
		quickSort(lo, partitionPoint-1, arr);
	if (partitionPoint < vamp_max)
		quickSort(partitionPoint+1, hi, arr);
}

void array_new(struct array **ptr, struct llnode *ll, vamp_t *count_ptr)
{
#if SANITY_CHECK
	assert(ptr != NULL);
	assert(*ptr == NULL);
	assert(count_ptr != NULL);
#endif
	if (ll == NULL)
		return;

	vamp_t size = llnode_getsize(ll);
	if (size == 0)
		return;

	vamp_t *arr = malloc(sizeof(vamp_t) * size);
	if (arr == NULL)
		abort();

	// copy
	for (vamp_t i = 0; ll != NULL; ll = ll->next) {
		memcpy(&(arr[i]), ll->data, (ll->logical_size) * sizeof(vamp_t));
		i += ll->logical_size;
	}

	// sort
	quickSort(0, size - 1, arr);

	// filter fangs & resize

	vamp_t count = 0;
	for (vamp_t i = 0; i < size; i++) {
		if (arr[i] == 0)
			continue;

		vamp_t value = arr[i];
		vamp_t fang_pairs = 1;

		while (i + fang_pairs < size && arr[i + fang_pairs] == value)
			fang_pairs++;
		memset(&(arr[i]), 0, sizeof(vamp_t) * fang_pairs);
		if (fang_pairs >= MIN_FANG_PAIRS)
			arr[count++] = value;
	}
	size = count;


#ifdef STORE_RESULTS
	struct array *new = malloc(sizeof(struct array));
	if (new == NULL)
		abort();

	new->data = arr;
	new->size = size;
	*ptr = new;
#else
	free(arr);
	*ptr = NULL;
#endif
	*count_ptr = count;
	return;
}
#endif /* PROCESS_RESULTS */

#if defined(STORE_RESULTS) && defined(CHECKSUM_RESULTS)
void array_checksum(struct array *ptr, struct hash *checksum)
{
#if SANITY_CHECK
	assert(ptr != NULL);
	assert(checksum != NULL);
#endif

	for (vamp_t i = 0; i < ptr->size; i++) {
		if (ptr->data[i] == 0)
			continue;

		vamp_t tmp = ptr->data[i];

		#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
		tmp = __builtin_bswap64(tmp);
		#endif

		EVP_DigestInit_ex(checksum->mdctx, checksum->md, NULL);

		EVP_DigestUpdate(checksum->mdctx, checksum->md_value, checksum->md_size);
		EVP_DigestUpdate(checksum->mdctx, &tmp, sizeof(tmp));

		EVP_DigestFinal_ex(checksum->mdctx, checksum->md_value, NULL);
	}
}
#endif

#if defined(STORE_RESULTS) && defined(PRINT_RESULTS)
void array_print(struct array *ptr, vamp_t count)
{
#if SANITY_CHECK
	assert(ptr != NULL);
#endif

	for (vamp_t i = 0; i < ptr->size; i++) {
		if (ptr->data[i] == 0)
			continue;

		fprintf(stdout, "%llu %llu\n", ++count, ptr->data[i]);
		fflush(stdout);
	}
}
#endif