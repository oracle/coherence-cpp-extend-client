/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NATIVE_TIME_HPP
#define COH_NATIVE_TIME_HPP

#include "coherence/lang/compatibility.hpp"

#include "private/coherence/util/Date.hpp"

COH_OPEN_NAMESPACE2(coherence,native)

using coherence::util::Date;


/**
* NativeTime provides an abtract interface for obtaining the system time.
*
* @author mf 2007.12.19
*/
class COH_EXPORT NativeTime
    {
    // ----- factory methods ------------------------------------------------

    public:
        /**
        * Return the NativeTime singleton
        *
        * @return the NativeTime instance
        */
        static NativeTime* instance();


    // ----- destructor -----------------------------------------------------

    public:
        /**
        * Destructor.
        */
        virtual ~NativeTime() {}


    // ----- NativeTime interface -------------------------------------------

    public:
        /**
        * Return the number of milliseconds which have elapsed since the
        * undefined platform specific start time.
        */
        virtual int64_t currentTimeMillis() const = 0;

        /**
        * Creates a new Date instance for the given time specified in
        * milliseconds.
        *
        * @param lMillis  the time in milliseconds for which to obtain the
        *                 Date instance
        *
        * @return a Date instance for the given time specified in
        *         milliseconds
        */
        virtual Date::View createDate(int64_t lMillis) const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NATIVE_TIME_HPP
