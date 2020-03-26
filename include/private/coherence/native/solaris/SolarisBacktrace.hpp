/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SOLARIS_BACKTRACE_HPP
#define COH_SOLARIS_BACKTRACE_HPP

#include <ucontext.h>

extern "C"
    {
    static int stacktrace_callback(uintptr_t pc, int signo, void *arg);
    }

typedef struct backtrace
    {
    void **frames;
    int    framesMax;
    int    frameCurrent;
    } backtrace_t;

// ----- helper functions ---------------------------------------------------
    
int backtrace(void **array, int size)
    {
    backtrace_t bt;
    ucontext_t  u;
    
    bt.frames       = array;
    bt.framesMax    = size;
    bt.frameCurrent = 0;
    
    if (getcontext(&u) < 0)
        {
        return 0;
        }
    
    (void) walkcontext(&u, stacktrace_callback, &bt);
    
    return bt.frameCurrent;
    }    

extern "C"
    {
    static int stacktrace_callback(uintptr_t pc, int signo, void *arg)
        {
        signo;

        backtrace_t* pBt = (backtrace_t*)arg;

        if (pBt->frameCurrent > pBt->framesMax)
            {
            return -1;
            }

        pBt->frames[pBt->frameCurrent++] = (void*) pc;

        return 0;
        }
    }

#endif // COH_SOLARIS_BACKTRACE_HPP
