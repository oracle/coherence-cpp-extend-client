/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/System.hpp"

#include "coherence/lang/Class.hpp"
#include "coherence/lang/ClassBasedHeapAnalyzer.hpp"
#include "coherence/lang/IllegalArgumentException.hpp"
#include "coherence/lang/IllegalStateException.hpp"
#include "coherence/lang/FinalView.hpp"
#include "coherence/lang/ObjectCountHeapAnalyzer.hpp"
#include "coherence/lang/String.hpp"
#include "coherence/lang/SynchronizedMemberWriteBlock.hpp"
#include "coherence/lang/SystemClassLoader.hpp"
#include "coherence/lang/TypedBarrenClass.hpp"
#include "coherence/lang/TypedExecutableClass.hpp"

#include "private/coherence/lang/LifeCycle.hpp"
#include "private/coherence/native/NativeDynamicLibrary.hpp"
#include "private/coherence/native/NativeTime.hpp"
#include "private/coherence/util/SafeClock.hpp"
#include "private/coherence/util/StringHelper.hpp"
#include "private/coherence/util/logging/Logger.hpp"

#include <algorithm>
#include <climits>
#include <iostream>
#include <stdlib.h>

#include <list>

COH_OPEN_NAMESPACE2(coherence,lang)

using coherence::native::NativeDynamicLibrary;
using coherence::native::NativeTime;
using coherence::util::SafeClock;
using coherence::util::StringHelper;
using coherence::util::logging::Logger;


COH_REGISTER_CLASS((TypedExecutableClass<System,
        TypedBarrenClass<System> >::create()));

// ---- local helper functions ----------------------------------------------

/**
* Getter/Setter used by the SystemClassLoader and the System::loadHeapAnalyzer
* to load custom HeapAnalyzer impelementations.
*
* @param vsName  when specified by System::loadHeapAnalyzer it is the name of
*                a class to watch for; when specified by the SystemClassLoader
*                it is the name of the class being loaded (which will be tested
*                against a previously specified HeapAnalyzer name.
* @param vClass  specified by the SystemClassLoader and is the class corresponding
*                to vsName
*
* @return the Class matching the name specified by the System::loadHeapAnalyzer
*             or NULL if one has not yet been found
*/
COH_EXPORT Class::View coh_exchangeCustomHeapAnalyzerClass(String::View vsName,
        Class::View vClass)
    {
    // horribly scary non-thread safe code, part of bootstrapping
    // custom HeapAnalyzers.  This code will only take effect during static
    // initialization while we are on the main thread.  It may be called
    // later as part of registering other classes on additional threads, but
    // by that point the HA loading will have given up, and
    static const String* cpsNameTarget = NULL;
    static const Class*  cpClassTarget = NULL;

    if (NULL == vClass)
        {
        // search request made by System::loadHeapAnalyzer, this will only
        // occur during static initialization
        cpsNameTarget = get_pointer(vsName);
        cpsNameTarget->_attach(/*fEscaped*/ true);
        return cpClassTarget;
        }

    // registration made by SystemClassLoader, test for match
    if (vsName->equals(cpsNameTarget))
        {
        // this match can occur exactly once, since a class cannot be
        // registered multiple times.  Thus we can a single writer thread,
        // the only thread which will ever read vClassTarget is the
        // main thread, during static init, at which point there is only
        // one thread in existance
        cpClassTarget = get_pointer(vClass);
        cpClassTarget->_attach(/*fEscaped*/ true);
        }
    return NULL;
    }
// no COH_STATIC_INIT, see usage

/**
* Coherence version tag for use with unix "what" command.
*
* Executing 'what libcoherence.so', will output the following string.
*/
const char* coh_version = COH_SYMB_TO_STRING(\
        @(#) COH_PRODUCT COH_VERSION Build COH_BUILD_INFO \
        (COH_PLATFORM COH_BUILD_TYPE COH_CC));

namespace
    {
    /**
    * Static lock used as a mutex.
    *
    * @return a mutex
    */
    Object::View getLock()
        {
        static FinalView<Object> vLock(System::common(),
                Object::create());
        return vLock;
        }
    COH_STATIC_INIT(getLock());

    /**
    * Accessor for the interrupt resolution
    *
    * @return reference to the interrupt resolution.
    */
    int64_t& interruptResolution()
        {
        static int64_t cMillis = 250; // see setInterruptResolution()
        return cMillis;
        }

    /**
    * Load the configured HeapAnalyzer.
    *
    * @param fEnabled  return false if the analyzer should be disabled
    *
    * @return the loaded analyzer
    */
    HeapAnalyzer::Handle loadHeapAnalyzer(bool& fEnabled)
        {
        static bool fBypass = false;

        if (fBypass)
            {
            return NULL;
            }

        HeapAnalyzer::Handle hAnalyzer;
        fBypass = true; // open bypass scope
        try
            {
            String::View vsClassName = System::getProperty
                    ("coherence.heap.analyzer");
            if (NULL == vsClassName || vsClassName->equals("none"))
                {
                // disable analysis for good
                fEnabled = false;
                }
            else if (vsClassName->equals("class"))
                {
                hAnalyzer = ClassBasedHeapAnalyzer::create();
                }
            else if (vsClassName->equals("alloc"))
                {
                hAnalyzer = ClassBasedHeapAnalyzer::create(true);
                }
            else if (vsClassName->equals("object"))
                {
                hAnalyzer = ObjectCountHeapAnalyzer::create();
                }
            else
                {
                if (Thread::currentThread()->getName()->equals("main"))
                    {
                    Class::View vClass = coh_exchangeCustomHeapAnalyzerClass
                            (vsClassName, NULL);
                    if (NULL != vClass)
                        {
                        hAnalyzer = cast<HeapAnalyzer::Handle>(vClass->newInstance());
                        }
                    }
                else
                    {
                    // disable analysis for good
                    fEnabled = false;

                    // another thread calling into us, we're past the loading
                    // phase and still haven't found the class, log a warning
                    // and disable analysis and further loading attempts
                    // by keepin fBypass = true
                    COH_LOG("Unable to load HeapAnalyzer \"" << vsClassName
                            << "\" disabling heap analysis", -1);

                    }
                }
            }
        catch (...)
            {
            fBypass = false;
            throw;
            }
        fBypass = false; // cannot be inside of catch

        return hAnalyzer;
        }

    String::View getLibPrefix()
        {
        static FinalView<String> vsPrefix(System::common(),
                String::create(COH_LIB_PREFIX));
        return vsPrefix;
        }
    COH_STATIC_INIT(getLibPrefix());

    String::View getLibSuffix()
        {
        static FinalView<String> vsSuffix(System::common(),
                String::create(COH_LIB_SUFFIX));
        return vsSuffix;
        }
    COH_STATIC_INIT(getLibSuffix());

    size32_t getCommonMonitorCount()
        {
        // Note: we can't use System::getProperty() here because that would
        // involve Object creation, and we'd endlessly recurse, we need to
        // return a single stable value for the life of the process
        // COH-12944 requires checking system property prefix for either coherence or tangosol
        static const char* achTmp1 = getenv("coherence.commonmonitors");
        static const char* achTmp2 = achTmp1 == NULL
            ? getenv("CoherenceCommonmonitors")
            : achTmp1;
        static const char*    achTmp3 = achTmp2 == NULL
            ? getenv("tangosol.coherence.commonmonitors")
            : achTmp2;
        static const char*    ach     = achTmp3 == NULL
            ? getenv("TangosolCoherenceCommonmonitors")
            : achTmp3;
        static const size32_t cMonitor = size32_t(ach == NULL ? 1021 : strtoul(ach, NULL, 10));

        return cMonitor;
        }
    COH_STATIC_INIT(getCommonMonitorCount());

    Object** getCommonMonitorArray()
        {
        static Object** paRaw = (Object**) calloc(getCommonMonitorCount(), sizeof(Object*));
        return paRaw;
        }
    COH_STATIC_INIT(getCommonMonitorArray());

    Object::Handle getCommonMonitorInternal(size_t n)
        {
        static size32_t cMonitor = getCommonMonitorCount();
        static Object** paRaw    = getCommonMonitorArray();
        static bool     fInit    = true;
        if (fInit)
            {
            fInit = false;
            for (size32_t i = 0; i < cMonitor; ++i)
                {
                paRaw[i] = System::CommonMonitor::create()->
                    _attach(/*fEscaped*/ true);
                }
            }

        return paRaw[n % cMonitor];
        }
    COH_STATIC_INIT(getCommonMonitorInternal(0));

    /**
    * Return System's SafeClock
    */
    SafeClock::Handle getSafeClock()
        {
        static FinalHandle<SafeClock> s_clock(System::common(),
                SafeClock::create(System::currentTimeMillis()));
        return s_clock;
        }
    COH_STATIC_INIT(getSafeClock());
    }

void coh_register_initializer(coh_static_initializer pInit, bool fAdd)
    {
    Object::View vLock = getLock();

    // COH-7585: in some cases, the libcoherence.so might be unlinked
    // before customer library. Thus causing the getLock to return NULL.
    // This workaround will fix the problem without the program core
    // dump during shutdown.
    if (vLock == NULL)
        {
        return;
        }

    COH_SYNCHRONIZED (vLock)
        {
        static std::list<coh_static_initializer> s_inits;
        if (pInit == NULL)
            {
            while (!s_inits.empty())
                {
                coh_static_initializer func = s_inits.front();
                s_inits.pop_front();
                (*func)();
                }
            }
        else if (fAdd)
            {
            s_inits.push_back(pInit);
            }
        else
            {
            s_inits.remove(pInit);
            }
        }
    }

/**
* Helper method, imported by Object to get the heap analyzer without attaching
* to it.
*/
COH_EXPORT HeapAnalyzer* coh_get_heap_analyzer()
    {
    // Note: the analyzer is initialized as part of static initialization
    // and onse set, cannot be reset. This is why a thread-safe handles are
    // not needed here. This is obviously a peformance critial, area.
    static HeapAnalyzer* pAnalyzer = NULL;
    static bool          fEnabled  = true;

    if (fEnabled)
        {
        if (NULL == pAnalyzer)
            {
            HeapAnalyzer::Handle hAnalyzer = loadHeapAnalyzer(fEnabled);
            if (NULL != hAnalyzer)
                {
                pAnalyzer = get_pointer(hAnalyzer);
                pAnalyzer->_attach(/*fEscaped*/ true); // just once
                }
            }
        return pAnalyzer;
        }
    return NULL;
    }
COH_STATIC_INIT(coh_get_heap_analyzer());

extern void coh_tls_compact(); // see ThreadLocalReference.cpp

// ----- System interface ---------------------------------------------------

void System::gc(bool fFull)
    {
    if (fFull)
        {
        Thread::doGlobalMaintenance(Thread::compact_tls);
        }
    else // minor GC
        {
        // just do TLS cleanup for this thread
        coh_tls_compact();
        }
    }

int64_t System::upTimeMillis()
    {
    static const int64_t ldtStart = currentTimeMillis();
    return currentTimeMillis() - ldtStart;
    }
COH_STATIC_INIT(System::upTimeMillis());

int64_t System::currentTimeMillis()
    {
    return NativeTime::instance()->currentTimeMillis();
    }

int64_t System::safeTimeMillis()
    {
    static FinalHandle<SafeClock> hSafeClock(System::common(), getSafeClock());
    return hSafeClock->getSafeTimeMillis(currentTimeMillis());
    }
COH_STATIC_INIT(System::safeTimeMillis());

int64_t System::lastSafeTimeMillis()
    {
    static FinalHandle<SafeClock> hSafeClock(System::common(), getSafeClock());
    return hSafeClock->getLastSafeTimeMillis();
    }
COH_STATIC_INIT(System::lastSafeTimeMillis());

void System::setInterruptResolution(int64_t cMillis)
    {
    if (cMillis <=0)
        {
        COH_THROW (IllegalArgumentException::create("interrupt resolution must be > 0"));
        }
    int32_t nLogLevel = cMillis > 1000 ? 2 : 9;
    COH_LOG("Setting thread interrupt resolution to " << cMillis << "ms", nLogLevel);
    interruptResolution() = cMillis;
    }
COH_STATIC_INIT(System::setInterruptResolution(Integer64::parse(System::
        getProperty("coherence.interrupt.resolution", "250"))));

int64_t System::getInterruptResolution()
    {
    return interruptResolution();
    }

void System::loadLibrary(String::View vsLibName)
    {
    if (vsLibName != NULL)
        {
        Object::View vLock = getLock();
        try
            {
            COH_LOG("Loading library: " << vsLibName, 6);
            COH_SYNCHRONIZED (vLock)
                {
                NativeDynamicLibrary::load(vsLibName);
                }
            }
        catch (Exception::View vex)
            {
            if (vsLibName->indexOf('/')  != String::npos ||
                vsLibName->indexOf('\\') != String::npos)
                {
                // patch was specified, no generic names allowed
                COH_THROW(vex);
                }

            // generate a platorm specific name and retry
            String::View vsPrefix = getLibPrefix();
            String::View vsSuffix = getLibSuffix();

            if (!vsLibName->startsWith(vsPrefix))
                {
                vsLibName = COH_TO_STRING(vsPrefix << vsLibName);
                }
            if (!vsLibName->endsWith(vsSuffix))
                {
                vsLibName = COH_TO_STRING(vsLibName << vsSuffix);
                }

            COH_SYNCHRONIZED (vLock)
                {
                NativeDynamicLibrary::load(vsLibName);
                }
            }
        }

    // run any static initializers loaded by the library; or by the sanka
    // itself if NULL was passed in
    coh_register_initializer(NULL, true);
    }

/**
 * Lookup up environment variable via vsName and if not found then by camel case of vsName.
 * Internal helper method to implement System::getProperty.
 *
 * @return value of vsName property if one is set in environment for vsName or its camel case variant.
 */
static String::View _getProperty(String::View vsName)
    {
    char* pProp = NULL;

    pProp = getenv(vsName->getCString());
    if (NULL == pProp)
    {
        // some shells (bash) at least don't support env variables with . in
        // them, check for _ instead
        if (vsName->indexOf('.') != String::npos)
            {
            return _getProperty(StringHelper::camel(vsName, "."));
            }
    }

    return pProp;
    }

String::View System::getProperty(String::View vsName,
        String::View vsDefault)
    {
    String::View vsValue = _getProperty(vsName);
        
    if (NULL == vsValue)
        {
        if (StringHelper::startsWith(vsName, "coherence."))
            {
            // backwards compatibility:
            // check for pre Coherence 12.2.1 system property pattern starting with "tangosol."
            vsValue = _getProperty(StringHelper::replaceFirst(vsName, "coherence.", "tangosol.coherence."));
            if (NULL == vsValue)
                {
                vsValue = _getProperty(StringHelper::replaceFirst(vsName, "coherence.", "tangosol."));
                }
            }
        else if (StringHelper::startsWith(vsName, "tangosol.coherence."))
            {
            // check for Coherence 12.2.1 system property pattern starting with "coherence."
            vsValue = _getProperty(StringHelper::replaceFirst(vsName, "tangosol.coherence.", "coherence."));
            }
        else if (StringHelper::startsWith(vsName, "tangosol."))
            {
            // check for Coherence 12.2.1 system property pattern starting with "coherence."
            vsValue = _getProperty(StringHelper::replaceFirst(vsName, "tangosol.", "coherence."));
            }
        }
    return NULL == vsValue ? vsDefault : vsValue;
    }

String::View System::setProperty(String::View vsName, String::View vsValue)
    {
    const char*  vacName         = vsName->getCString();
    String::View vsValuePrevious = getenv(vacName);

#ifdef COH_CC_MSVC
    _putenv_s(vacName, vsValue->getCString());
#else
    setenv(vacName, vsValue->getCString(), true);
#endif
    return vsValuePrevious;
    }

String::View System::clearProperty(String::View vsName)
    {
    String::View vsValuePrevious = getenv(vsName->getCString());

#ifdef COH_CC_MSVC
    _putenv_s(vsName->getCString(), "");
#else
    unsetenv(vsName->getCString());
#endif
    return vsValuePrevious;
    }


size32_t System::identityHashCode(Object::View v)
    {
    return NULL == v ? 0 : v->Object::hashCode();
    }

// ----- diagnostics methods ------------------------------------------------


void System::assertAttachCount(Object::View v, uint32_t cHandle,
        uint32_t cView)
    {
    LifeCycle::Refs refs(v->m_atomicLifeCycleRefs.get());

    if (refs.value.cHandle != cHandle || refs.value.cView != cView)
        {
        COH_THROW_STREAM (IllegalStateException,
            "Unexpected attach state: actual/expected Handles "
            << refs.value.cHandle << '/' << cHandle << ", Views "
            << refs.value.cView   << '/' << cView
            << ", FullState " << refs);
        }
    }

String::View System::getLifeCycleDescription(Object::View v)
    {
    return COH_TO_STRING(LifeCycle(v->m_atomicLifeCycleState.get(), v->m_atomicLifeCycleRefs.get()));
    }

HeapAnalyzer::Handle System::getHeapAnalyzer()
    {
    return coh_get_heap_analyzer();
    }

Object::Handle System::getCommonMonitor(size32_t i)
    {
    return getCommonMonitorInternal(i);
    }

Object::Handle System::getCommonMonitor(size64_t l)
    {
    return getCommonMonitorInternal(size_t(l));
    }

Object& System::common()
    {
    static size_t c = 0; // does not need to be thread safe
    size_t garbage; // uninitialized

    // NOTE: garbage is uninitialized, we use it's address, which is unique
    // in time across all threads.  We also increment "c" on each call so that
    // a thread calling common() repeatdly will get different values.
    return *(getCommonMonitorInternal(((size_t) &garbage) + ++c));
    }

bool System::isCommonMonitorInitialized()
    {
    Object** pao = getCommonMonitorArray();
    size32_t c   = getCommonMonitorCount();

    for (size32_t i = 0; i < c; ++i)
        {
        if (pao[i] == NULL)
            {
            return false;
            }
        }

    return true;
    }

void System::main(ObjectArray::View)
    {
    // trim of '@(#) '
    std::cout << coh_version + 5 << std::endl;
    }

String::View System::setLocaleInternal(String::View vsLocale)
    {
    std::locale loc(vsLocale->getCString());
    std::cin.imbue(loc);
    std::cout.imbue(loc);
    std::cerr.imbue(loc);
    std::clog.imbue(loc);
    std::wcin.imbue(loc);
    std::wcout.imbue(loc);
    std::wcerr.imbue(loc);
    std::wclog.imbue(loc);
    return std::locale::global(loc).name();
    }

COH_CLOSE_NAMESPACE2
