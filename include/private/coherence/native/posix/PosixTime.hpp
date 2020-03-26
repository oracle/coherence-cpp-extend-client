/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POSIX_TIME_HPP
#define COH_POSIX_TIME_HPP

#include "coherence/lang/compatibility.hpp"

#include "private/coherence/native/NativeTime.hpp"

#include <sstream>
#include <sys/time.h>

COH_OPEN_NAMESPACE3(coherence,native,posix)


/**
* Posix implementation of NativeTime.
*
* @author mf 2007.12.19
*/
class COH_EXPORT PosixTime
    : public coherence::native::NativeTime
    {
    // ----- constructor ----------------------------------------------------

    public:
        /**
        * Construct a new PosixTime object.
        */
        PosixTime()
            {
            }

        /**
        * Destructor.
        */
        virtual ~PosixTime()
            {
            }


    // ----- NativeTime interface -------------------------------------------

    public:
        /**
        * @inheritDoc
        */
        virtual int64_t currentTimeMillis() const
            {
            struct timeval tvNow;

            COH_ENSURE_EQUALITY(gettimeofday(&tvNow, NULL), 0);

            int64_t cMillis = tvNow.tv_sec;
            cMillis *= 1000;
            cMillis += tvNow.tv_usec / 1000;

            return cMillis;
            }

        /**
        * @inheritDoc
        */
        virtual Date::View createDate(int64_t lMillis) const
            {
            COH_ENSURE_PARAM_RELATION(lMillis, >, 0);
            COH_ENSURE_PARAM_RELATION(lMillis / 1000, <, 0x7FFFFFFF);

            time_t  nSeconds = (time_t) (lMillis / 1000);
            int32_t nMillis  = (int32_t) (lMillis % 1000);

            tm timeData;
            localtime_r(&nSeconds, &timeData);

            return Date::create(1900 + timeData.tm_year,
                    timeData.tm_mon + 1, timeData.tm_mday, timeData.tm_hour,
                    timeData.tm_min, timeData.tm_sec, nMillis);
            }
    };

COH_CLOSE_NAMESPACE3


// ----- NativeTime factory methods -----------------------------------------

COH_OPEN_NAMESPACE2(coherence,native)

NativeTime* NativeTime::instance()
    {
    static NativeTime* INSTANCE =
            new coherence::native::posix::PosixTime();

    return INSTANCE;
    }
COH_STATIC_INIT(NativeTime::instance());

COH_CLOSE_NAMESPACE2

#endif // COH_POSIX_TIME_HPP
