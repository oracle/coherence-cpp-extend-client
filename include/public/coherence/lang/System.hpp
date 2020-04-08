/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SYSTEM_HPP
#define COH_SYSTEM_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/abstract_spec.hpp"
#include "coherence/lang/HeapAnalyzer.hpp"
#include "coherence/lang/ObjectArray.hpp"
#include "coherence/lang/String.hpp"

#include <locale>

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* A collection of general purpose utility methods.
*
* @author mf 2007.12.19
*/
class COH_EXPORT System
    : public abstract_spec<System>
    {
    // ----- constructors ---------------------------------------------------

    private:
        /**
        * Blocked constructor.
        */
        System();


    // ----- System interface -----------------------------------------------

    public:
        /**
         * Runs the garbage collector.
         *
         * While most managed objects are automatically reclaimed when their
         * reference count reaches zero, some special case objects utilize
         * deferred automatic cleanup.  Calling this method will expedite this
         * deferred cleanup.
         *
         * @param fFull  true iff an exhaustive collection is desired
         */
        static void gc(bool fFull = false);

        /**
        * Return the number of milliseconds which have elapsed since the
        * process was started.
        *
        * @return the number of milliseconds which have elapsed since the
        *         process was started
        */
        static int64_t upTimeMillis();

        /**
        * Return the elapsed milliseconds since midnight January 1, 1970 UTC.
        *
        * This function does not guard against the clock rolling back or
        * jumping forward.
        *
        * @return the elapsed milliseconds
        */
        static int64_t currentTimeMillis();

        /**
        * Returns a "safe" current time in milliseconds.
        *
        * Unlike the #currentTimeMillis() this method guarantees that the
        * time never "goes back".
        *
        * More specifically, when called twice on the same thread, the second
        * call will never return a value that is less then the value returned
        * by the first call. If a system time correction becomes necessary, an
        * attempt will be made to gradually compensate the returned value, so
        * in the long run the value returned by this method is the same as the
        * system time.
        *
        * Additionally, the following always holds true:
        * <pre>
        *   System::safeTimeMillis() >= System::currentTimeMillis();
        * </pre>
        *
        * @return the difference, measured in milliseconds, between
        *         the corrected current time and midnight, January 1, 1970 UTC.
        */
        static int64_t safeTimeMillis();

        /**
        * Returns the last "safe" time as computed by a previous call to
        * the {@link #safeTimeMillis} method.
        *
        * Note: Since the underlying field is non-volatile, the returned value
        * is only guaranteed to be no less than the last value returned by
        * safeTimeMillis() call on the same thread.
        *
        * @return the last "safe" time in milliseconds
        */
        static int64_t lastSafeTimeMillis();

        /**
        * Return the value of a system property.
        *
        * The standard naming convention for system properties is '.'
        * Separated property names, such as "coherence.setting".
        *
        * As of Coherence 3.4 the only source for system properties is
        * environment variables. As some environments do not allow '.' in
        * variable names, a failed lookup will be automatically re-issued
        * using a camel-cased version of the supplied name, i.e. the property
        * name "coherence.setting" would translate to
        * "CoherenceSetting".  From a code level perspective
        * camel-case does not need to be a consideration, it is only when
        * setting environment variables that it may be needed.
        *
        * As of Coherence 12.2.1, the Coherence system property pattern
        * begins with "coherence.".  This method looks up system property
        * by the provided vsName format.  If vsName starts with "coherence." and
        * the system property lookup fails to find a value, this method retries
        * lookup with leading "tangosol." for backwards compatibility support.
        *
        * If vsName starts with "tangosol.",
        * it follows pre Coherence 12.2.1 Coherence system property pattern.
        * If lookup of vsName following this pattern occurs, then
        * a failed lookup is retried following Coherence 12.2.1 system
        * property pattern, replacing leading "tangosol." with
        * "coherence."
        *
        * @param vsName     the name of the property to return
        * @param vsDefault  the default value for the property
        *
        * @return the corresponding property value, or the supplied default if
        *         no value could be found
        */
        static String::View getProperty(String::View vsName,
                String::View vsDefault = String::null_string);

        /**
        * Set the system property to a value.
        *
        * @param vsName    the system property name
        * @param vsValue   the value of the system property
        *
        * @return previous value of system property
        *
        * @since Coherence 12.2.1
        */
        static String::View setProperty(String::View vsName, String::View vsValue); 

        /**
        * Clear system property's value.
        *
        * @param vsName  the system property name to remove its value
        *
        * @return previous value of system property
        *
        * @since Coherence 12.2.1
        */
        static String::View clearProperty(String::View vsName);

        /**
        * Return the same hash code for the given object as would be returned
        * by the default implementation provided in Object. In the case of
        * NULL zero is returned.
        *
        * @param v  the object to hash
        *
        * @return the object's identity hash code.
        */
        static size32_t identityHashCode(Object::View v);

        /**
        * Specify the system interrupt resolution.
        *
        * The resolution at which a blocked thread will test if it has been
        * interrupted.  Not all blocking states are guaranteed to utilize this
        * feature, but at a minimum Object::wait(), and Coherence based network
        * communications do respect this setting.
        *
        * Once in a blocking state the blocked call may not see updates to this
        * setting. If this setting is to be updated it is recommended that it
        * only occur once at application startup.
        *
        * Recommended values are between 100 and 1000 milliseconds, default is
        * 250 milliseconds.
        *
        * @param cMillis  the new interrupt resolution
        *
        * @throws IllegalArgumentException if cMillis <= 0
        */
        static void setInterruptResolution(int64_t cMillis);

        /**
        * Return the interrupt resolution.
        *
        * @return the interrupt resolution.
        */
        static int64_t getInterruptResolution();

        /**
        * Load the specified library.
        */
        static void loadLibrary(String::View vsLibName);

        /**
         * Set the global locale.
         *
         * @param vsLocale the name of the locale to use, or NULL for the classic locale
         *
         * @return the name of the former locale
         */
        COH_INLINE static String::View setLocale(String::View vsLocale)
            {
            if (vsLocale == NULL)
                {
                vsLocale = "C"; // classic locale
                }

            // pass through to coh internals in case is using a different standard library.
            // Note we only do this by name as passing a std::local object across the lib
            // boundary may not be safe if different standard libraries are used
            String::View vsOld = System::setLocaleInternal(vsLocale);

            // we also set it on the std version used by the caller, since this is inlined
            // it will pick up the callers library even if different from the one used internally
            std::locale loc(vsLocale->getCString());
            std::cin.imbue(loc);
            std::cout.imbue(loc);
            std::cerr.imbue(loc);
            std::clog.imbue(loc);
            std::wcin.imbue(loc);
            std::wcout.imbue(loc);
            std::wcerr.imbue(loc);
            std::wclog.imbue(loc);
            std::locale::global(loc);

            return vsOld;
            }

    // ----- diagnostics methods --------------------------------------------

    public:
        /**
        * Test that the attach count on the Object is the expected value.
        *
        * This method is for testing purposes only, and will throw an
        * exception if the count is not the expected value.
        *
        * @param v        the Object to test
        * @param cHandle  the expected number of handles attached to this
        *                 Object
        * @param fEscaped the expected object escape state
        */
        static void assertAttachCount(Object::View v, uint32_t cHandle,
                uint32_t cView);

        /**
        * Return a string representation of the Object's life-cycle state.
        *
        * This is intended for diagnostics purposes only.
        *
        * @param v  the Object to describe
        *
        * @return a human-readable description of the object's life-cycle
        *         state
        */
        static String::View getLifeCycleDescription(Object::View v);

        /**
        * Return the System HeapAnalyzer.
        *
        * There may be only one HeapAnalyzer for the life of the process, the
        * default being the ObjectCountHeapAnalyzer, which is very low cost.
        *
        * The HeapAnalyzer to use may be specified via the
        * "coherence.heap.analyzer" system property, can can be set
        * to either a registered class name, or to "none".
        *
        * If using a heap-analyzer other then the default, some Objects
        * created during static initialization may not be registered.
        *
        * @return the System HeapAnalyzer.
        */
        static HeapAnalyzer::Handle getHeapAnalyzer();

        /**
        * Return the common monitor associated with the specified integer
        * value.
        *
        * Common monitors allow for a low-cost means to reduce contention by
        * spreading synchronization over a large number of monitors. An
        * example usage would be to produce an "atomic array". For instance
        * to atomically change an element within an array which is being
        * simultaneously updated by multiple threads:
        * <pre>
        * COH_SYNCHRONIZED (System::getCommonMonitor(
        *   System::identityHashCode(aoShared) + i))
        *     {
        *     oOld = haoShared[i];
        *     haoShared[i] = oNew;
        *     }
        * </pre>
        * With this approach many threads may concurrently access various
        * array elements without having to synchronize on the array itself,
        * and contend with each other. The use of common monitors also avoids
        * the overhead of allocating a unique monitor per index. This example
        * additionally makes use of the array's identity hash code to avoid
        * frequent collisions against other atomic arrays for the same indices.
        *
        * As they are shared, these monitors will apply to any number of
        * unrelated entities, and as such certain precautions must be employed
        * when using them.
        * <ul>
        * <li>The holder of a common monitor MUST not synchronize on any other
        *     common monitor. Failure to adhere to this precaution will result
        *     in a deadlock.
        * <li>Notifications on a common monitor MUST use notifyAll() rather
        *     then notify(), as there may be unrelated threads waiting for
        *     notification on the same monitor which could consume a single
        *     notification. Thus the only way to ensure that the desired
        *     thread does receive notification is to notify all threads
        *     waiting on the monitor.
        * <li>Threads waiting for a notification must protect themselves
        *     against spurious style wakeups. While this is a general, though
        *     often overlooked part of the normal use of notification, with
        *     common monitors it is far more likely that a thread will be
        *     notified due to an unrelated event.
        * <li>A thread which is holding synchronization on a common monitor
        *     should avoid blocking operations as this could block unrelated
        *     threads which happen to be utilizing the same common monitor.
        * </ul>
        * The ideal number of common monitors in a process is one per
        * concurrently executing thread. As this number is generally unknown
        * the default number of monitors set to a relatively high value. The
        * value may also be manually specified via the
        * <code>tangosol.coherence.commonmonitors</code> system property.
        *
        * @param i the common monitor identifier
        *
        * @return the associated monitor
        */
        static Object::Handle getCommonMonitor(size32_t i);

        /**
        * Return the common monitor associated with the specified long value.
        *
        * @param l the common monitor identifier
        *
        * @return the associated monitor
        *
        * @see #getCommonMonitor(size32_t)
        */
        static Object::Handle getCommonMonitor(size64_t l);

        /**
        * Return a random common monitor for use as the guardian of
        * thread-safe handles which are not a data member of a managed object.
        */
        static Object& common();

        /**
         * Return true if common monitors have been initialized.
         */
        static bool isCommonMonitorInitialized();

        /**
        * Executable entrypoint for the System class.
        *
        * Print the Coherence library version information.
        */
        static void main(ObjectArray::View);


    // ----- helper methods -------------------------------------------------

    protected:
        /**
         * Internal helper for specifying the global locale used within
         * the Coherence library.
         */
        static String::View setLocaleInternal(String::View vsLocale);


    // ----- nested class: CommonMonitor ------------------------------------

    public:
        /**
        * A class which acts as CommonMonitor.
        *
        * The custom class is produced to aid in identification while profiling.
        */
        class COH_EXPORT CommonMonitor
            : public class_spec<CommonMonitor>
            {
            friend class factory<CommonMonitor>;

            protected:
                CommonMonitor() {}
            };
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SYSTEM_HPP
