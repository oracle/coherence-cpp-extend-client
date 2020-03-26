/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NATIVE_THREAD_HPP
#define COH_NATIVE_THREAD_HPP

#include "coherence/lang/compatibility.hpp"

#include <memory>

COH_OPEN_NAMESPACE2(coherence,native)


/**
* NativeThread provides a cross-platform means for invokeing Runnables on
* new threads.
*
* @author mf 2007.12.09
*/
class COH_EXPORT NativeThread
    {
    // ----- factory methods ------------------------------------------------

    public:
        /**
        * Create a new thread and execute the supplied Runnable. The thread
        * manages its own memory.
        */
        static void create(Runnable::Handle hRunnable, size32_t cBytesStack = 0);


    // ----- constructors ---------------------------------------------------

    public:
        /**
        * NativeThread destructor
        */
        virtual ~NativeThread()
            {
            }


    // ----- NativeThread interface -----------------------------------------

    public:
        /**
        * Start the NativeThread.
        */
        virtual void start() = 0;

        /**
        * Invoke the runnable associated with this Thread, on the current
        * thread.  This method is called automatically by start()
        */
        virtual void run() = 0;

        /**
        * Yield the current thread.
        */
        static void yield();
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NATIVE_THREAD_HPP
