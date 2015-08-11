/* -*- mode: c; indent-tabs-mode: nil; c-basic-offset: 4; -*- */
/* vim:   set expandtab shiftwidth=4 softtabstop=4 : */
/****************************************************************************
 *  Copyright 2011, Lexmark International, Inc.
 *  All Rights Reserved.  Proprietary and Confidential.
 ***************************************************************************/
#include "gmalloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

// use 1/2 of the pool size - we should always be able to get this much memory
// on an unused pool.  We should also be able to, reliably, count on 2* this
// number to fail, since there are various overheads.
static const unsigned int ALLOC_SIZE = 64 * 1024;

static volatile bool was_more_memory_called = false;

unsigned int more_memory(void* u1, unsigned int u2, unsigned int u3)
{
    (void)u1;
    (void)u2;
    (void)u3;
    was_more_memory_called = true;
    return 0;
}

int main()
{
    void* ptr;

    gmalloc_set_callbacks(NULL, more_memory, NULL);

    printf("Initial report:\n");
    gmalloc_report(stdout, "");

    // make sure free really works
    ptr = malloc(ALLOC_SIZE);
    assert(ptr);
    assert(!was_more_memory_called);
    free(ptr);

    ptr = malloc(ALLOC_SIZE);
    assert(ptr);
    assert(!was_more_memory_called);
    free(ptr);

    // make sure malloc that is too big fails
    assert(NULL == malloc(2 * ALLOC_SIZE));
    assert(was_more_memory_called);
    was_more_memory_called = false;

    // now check realloc
    ptr = malloc(ALLOC_SIZE / 2);
    assert(ptr);
    ptr = realloc(ptr, ALLOC_SIZE);
    assert(ptr);
    assert(!was_more_memory_called);
    free(ptr);

    printf("Final report:\n");
    gmalloc_report(stdout, NULL);

    return 0;
}
