// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (c) 2021 Pierro Zachareas
 */

#include "configuration.h"

#ifdef PROCESS_RESULTS
#include <stdlib.h>
#include "llhandle.h"
#include "btnode.h"
#include "bthandle.h"
#endif

#if defined (PROCESS_RESULTS) && SANITY_CHECK
#include <assert.h>
#endif

#ifdef PROCESS_RESULTS
void bthandle_new(struct bthandle **ptr)
{
	if (ptr == NULL)
		return;

	struct bthandle *new = malloc(sizeof(struct bthandle));
	if (new == NULL)
		abort();

	new->node = NULL;
	new->size = 0;
	*ptr = new;
}

void bthandle_free(struct bthandle *handle)
{
	if (handle == NULL)
		return;
		
	btnode_free(handle->node);
	free(handle);
}

void bthandle_add(
	struct bthandle *handle,
	vamp_t key)
{
#if SANITY_CHECK
	assert(handle != NULL);
#endif
	handle->node = btnode_add(handle->node, key, &(handle->size));
}

void bthandle_reset(struct bthandle *handle)
{
	btnode_free(handle->node);
	handle->node = NULL;
	handle->size = 0;
}

/*
 * bthandle_cleanup
 *
 * Move data from binary tree to linked list.
 * This function is used to reduce memory usage, as a linked list has less
 * memory overhead.
 */
void bthandle_cleanup(
	struct bthandle *handle,
	struct llhandle *lhandle,
	vamp_t key)
{
	struct btnode *tree = handle->node;
	vamp_t *size = &(handle->size);
	handle->node = btnode_cleanup(tree, key, lhandle, size);
}

#endif /* PROCESS_RESULTS */