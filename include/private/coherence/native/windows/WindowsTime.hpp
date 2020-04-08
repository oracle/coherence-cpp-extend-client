/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_WINDOWS_TIME_HPP
#define COH_WINDOWS_TIME_HPP

#include "coherence/lang/compatibility.hpp"

#include "private/coherence/native/NativeTime.hpp"

#include <time.h>
#include <sstream>

COH_OPEN_NAMESPACE3(coherence,native,windows)

/**
* Delta between Windows C++ epoch 1/1/1601 and Unix epoch 1/1/1970
*/
#define DELTA_EPOCH_IN_MILLIS 11644473600000LL

/**
* Windows implementation of NativeTime.
*
* @author mf 2007.12.19
*/
class COH_EXPORT WindowsTime
    : public coherence::native::NativeTime
    {
    // ----- constructor ----------------------------------------------------

    public:
        /**
        * Construct a new WindowsTime object.
        */
        WindowsTime()
            {
            }

        /**
        * Destructor.
        */
        virtual ~WindowsTime()
            {
            }


    // ----- NativeTime interface -------------------------------------------

    public:
        /**
        * @inheritDoc
        */
        virtual int64_t currentTimeMillis() const
            {
            SYSTEMTIME systemTime;
            GetSystemTime(&systemTime);

            FILETIME fileTime;
            SystemTimeToFileTime(&systemTime, &fileTime);

            ULARGE_INTEGER uli;
            uli.LowPart  = fileTime.dwLowDateTime;
            uli.HighPart = fileTime.dwHighDateTime;

            ULONGLONG systemTimeIn_ms(uli.QuadPart / 10000);
            return systemTimeIn_ms - DELTA_EPOCH_IN_MILLIS;
            }

        /**
        * @inheritDoc
        */
        virtual Date::View createDate(int64_t lMillis) const
            {
            lMillis += DELTA_EPOCH_IN_MILLIS;
            // ensure that millis is > 0  and that it isn't too large
            COH_ENSURE_PARAM_RELATION(lMillis, >, 0);
            COH_ENSURE_PARAM_RELATION(lMillis, <,
                    int64_t(COH_INT64(0x7FFFFF, 0xFFFFFFFF)));

            ULARGE_INTEGER uli;
            uli.QuadPart = lMillis * 10000;

            FILETIME fileTime;
            fileTime.dwLowDateTime  = uli.LowPart;
            fileTime.dwHighDateTime = uli.HighPart;

            SYSTEMTIME systemTime;
            FileTimeToSystemTime(&fileTime, &systemTime);
            SystemTimeToTzSpecificLocalTime(NULL, &systemTime, &systemTime);

            return Date::create(systemTime.wYear, systemTime.wMonth,
                    systemTime.wDay, systemTime.wHour, systemTime.wMinute,
                    systemTime.wSecond, systemTime.wMilliseconds);
            }
    };

COH_CLOSE_NAMESPACE3


// ----- NativeTime factory methods -----------------------------------------

COH_OPEN_NAMESPACE2(coherence,native)

NativeTime* NativeTime::instance()
    {
    static NativeTime* INSTANCE =
            new coherence::native::windows::WindowsTime();

    return INSTANCE;
    }
COH_STATIC_INIT(NativeTime::instance());

COH_CLOSE_NAMESPACE2

#endif // COH_WINDOWS_TIME_HPP
