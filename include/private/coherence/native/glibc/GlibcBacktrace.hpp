/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_GLIBC_BACKTRACE_HPP
#define COH_GLIBC_BACKTRACE_HPP

#include "coherence/lang.ns"

#include "coherence/util/ArrayList.hpp"

#include "private/coherence/native/NativeBacktrace.hpp"
#include "private/coherence/native/NativeABI.hpp"
#include "private/coherence/native/NativeStackElement.hpp"
#include "private/coherence/util/StringHelper.hpp"

#ifndef __USE_GNU
  #define __USE_GNU // needed for using SunPro compiler on Linux
#endif

#include <dlfcn.h>

#ifdef COH_OS_SOLARIS
    // solaris 9 didn't include execinfo.h, we we re-define the equivalent
    #include "private/coherence/native/solaris/SolarisBacktrace.hpp"
#else
    #include <execinfo.h>
#endif

COH_OPEN_NAMESPACE2(coherence,native)

using coherence::util::ArrayList;
using coherence::util::StringHelper;


// ----- file local helpers -------------------------------------------------

namespace
    {
    String::View getThreadRun()
        {
        static FinalHandle<String> hs(System::common(),
                String::create("coherence::lang::Thread::run()"));
        return hs;
        }
    COH_STATIC_INIT(getThreadRun());
    }

// ----- NativeBacktrace static interface -----------------------------------

ObjectArray::Handle NativeBacktrace::getStackTrace(size32_t cTrim)
    {
    static bool fTrim = Boolean::parse(System::getProperty
        ("coherence.threaddump.trim", "true")); // for diagnostics

    ArrayList::Handle haFrames    = ArrayList::create();
    String::View       vsThreadRun = getThreadRun();
    void *frames[100];
    int   size;

    size = backtrace(frames, 100);
    cTrim += 1;

    for (int i = 0, c = size - 1; i < c; i++)
        {
        if (fTrim && cTrim)
            {
            --cTrim;
            }
        else
            {
            void *pc = frames[i];

            Dl_info info;

            if (dladdr(pc, &info) == 0)
                {
                break;
                }

            String::View vsName = (info.dli_sname
                    ? NativeABI::demangle(info.dli_sname)
                    : (String::View) StringHelper::getEmptyString());

             haFrames->add(NativeStackElement::create(
                (info.dli_fname
                    ? String::create(info.dli_fname)
                    : StringHelper::getEmptyString()),
                vsName));

             // trim off OS specific bits below coherence created threads
             if (fTrim && vsName->endsWith(vsThreadRun))
                 {
                 break;
                 }
            }
        }

    return haFrames->toArray();
    }

COH_CLOSE_NAMESPACE2

#endif // COH_GLIBC_BACKTRACE_HPP
