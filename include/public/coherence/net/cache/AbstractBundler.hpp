/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_BUNDLER_HPP
#define COH_ABSTRACT_BUNDLER_HPP

#include "coherence/lang.ns"

#include "coherence/net/NamedCache.hpp"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/AtomicCounter.hpp"
#include "coherence/util/List.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::net::NamedCache;
using coherence::util::ArrayList;
using coherence::util::AtomicCounter;
using coherence::util::List;

/**
 * An abstract base for processors that implement bundling strategy.
 *
 * Assume that we receive a continuous and concurrent stream of individual
 * operations on multiple threads in parallel. Let's also assume those individual
 * operations have relatively high latency (network or database-related) and
 * there are functionally analogous [bulk] operations that take a collection of
 * arguments instead of a single one without causing the latency to grow
 * linearly, as a function of the collection size. Examples of operations and
 * topologies that satisfy these assumptions are:
 * <ul>
 *   <li> get() and getAll() methods for the NamedCache API for the
 *        partitioned cache service topology;
 *   <li> put() and putAll() methods for the NamedCache API for the
 *        partitioned cache service topology;
 * </ul>
 *
 * Under these assumptions, it's quite clear that the bundler could achieve a
 * better utilization of system resources and better throughput if slightly
 * delays the individual execution requests with a purpose of "bundling" them
 * together and passing int32_to a corresponding bulk operation. Additionally,
 * the "bundled" request should be triggered if a bundle reaches a "preferred
 * bundle size" threshold, eliminating a need to wait till a bundle timeout is
 * reached.
 *
 * Note: we assume that all bundle-able operations are idempotent and could be
 * repeated if un-bundling is necessary due to a bundled operation failure.
 *
 * @author gg 2007.01.28
 * @author lh 2012.06.05
 * @since Coherence 12.1.2
 */
class COH_EXPORT AbstractBundler
    : public abstract_spec<AbstractBundler>
    {
    // ----- handle definitions (needed for nested classes) -----------------

    public:
        typedef this_spec::Handle Handle;
        typedef this_spec::View   View;
        typedef this_spec::Holder Holder;

    // ----- constructors ---------------------------------------------------

    /**
     * Construct the bundler. By default, the timeout delay value is set to
     * one millisecond and the auto-adjustment feature is turned on.
     */
    protected:
        AbstractBundler();

    private:
        /**
         * Blocked copy constructor.
         */
        AbstractBundler(const AbstractBundler&);

    // ----- property accessors ---------------------------------------------

    public:
        /**
         * Obtain the bundle size threshold value.
         *
         * @return the bundle size threshold value expressed in the same units
         *         as the value returned by the {@link Bundle::getBundleSize()}
         *         method
         */
        virtual int32_t getSizeThreshold() const;

        /**
         * Specify the bundle size threshold value.
         *
         * @param cSize  the bundle size threshold value; must be positive
         *               value expressed in the same units as the value returned
         *               by the {@link Bundle::getBundleSize()} method
         */
        virtual void setSizeThreshold(int32_t cSize);

        /**
         * Obtains the minimum number of threads that will trigger the bundler
         * to switch from a pass through to a bundled mode.
         *
         * @return a the number of threads threshold
         */
        virtual int32_t getThreadThreshold() const;

        /**
         * Specify the minimum number of threads that will trigger the bundler to
         * switch from a pass through to a bundled mode.
         *
         * @param cThreads  the number of threads threshold
         */
        virtual void setThreadThreshold(int32_t cThreads);

        /**
         * Obtain the timeout delay value.
         *
         * @return the timeout delay value in milliseconds
         */
        virtual int64_t getDelayMillis() const;

        /**
         * Specify the timeout delay value.
         *
         * @param lDelay  the timeout delay value in milliseconds
         */
        virtual void setDelayMillis(int64_t lDelay);

        /**
         * Check whether or not auto-adjustment is allowed.
         *
         * @return true if auto-adjustment is allowed
         */
        virtual bool isAllowAutoAdjust() const;

        /**
         * Specify whether or not auto-adjustment is allowed.
         *
         * @param fAutoAdjust  true if auto-adjustment should be allowed;
         *                     false otherwise
         */
        virtual void setAllowAutoAdjust(bool fAutoAdjust);

    // ----- statistics ------------------------------------------------------

    protected:
        /**
         * Update the statistics for this Bundle.
         */
        virtual void updateStatistics();

    public:
        /**
         * Reset this Bundler statistics.
         */
        virtual void resetStatistics();

        /**
         * Adjust this Bundler's parameters according to the available
         * statistical information.
         */
        virtual void adjust();

    // ----- Object interface -----------------------------------

    public:
        /**
         * {@inheritDoc}
         */
        virtual TypedHandle<const String> toString() const;

    // ----- inner class: Bundle --------------------------------------------

    /**
     * Bundle represents a unit of optimized execution.
     */
    protected:
        class COH_EXPORT Bundle
            : public abstract_spec<Bundle>
            {
            friend class factory<Bundle>;
            friend class AbstractBundler;

            // ----- constructors ---------------------------------------------------

            /**
             * Default constructor.
             *
             * @param hBundler  the AbstructBundler
             */
            protected:
                Bundle(AbstractBundler::Handle hBundler);

            // ----- accessors -------------------------------------------------

            public:
                /**
                 * Check whether or not this bundle is open for adding request elements.
                 *
                 * @return true if this Bundle is still open
                 */
                virtual bool isOpen() const;

                /**
                 * Return the Bundler.
                 *
                 * @return the Bundler
                 */
                virtual AbstractBundler::Handle getBundler();

            protected:
                /**
                 * Check whether or not this bundle is in the "pending" state -
                 * awaiting for the execution results.
                 *
                 * @return true if this Bundle is in the "pending" state
                 */
                virtual bool isPending() const;

                /**
                 * Check whether or not this bundle is in the "processed" state -
                 * ready to return the result of execution back to the client.
                 *
                 * @return true if this Bundle is in the "processed" state
                 */
                virtual bool isProcessed() const;

                /**
                 * Check whether or not this bundle is in the "exception" state -
                 * bundled execution threw an exception and requests have to be
                 * un-bundled.
                 *
                 * @return true if this Bundle is in the "exception" state
                 */
                virtual bool isException() const;

                /**
                 * Change the status of this Bundle.
                 *
                 * @param iStatus  the new status value
                 */
                virtual void setStatus(int32_t iStatus);

                /**
                 * Obtain this bundle size. The return value should be expressed in the
                 * same units as the value returned by the
                 * {@link AbstractBundler::getSizeThreshold getSizeThreshold} method.
                 *
                 * @return the bundle size
                 */
                virtual int32_t getBundleSize() const;

                /**
                 * Check whether or not this is a "master" Bundle.
                 *
                 * @return true if this Bundle is a designated "master" Bundle
                 */
                virtual bool isMaster() const;

                /**
                 * Designate this Bundle as a "master" bundle.
                 */
                virtual void setMaster();

                // ----- processing and subclassing support --------------------------

            public:
                /**
                 * Wait until results of bundled requests are retrieved.
                 * 
                 * Note that calls to this method must be externally synchronized.
                 *
                 * @param fFirst  true if this is the first thread entering the bundle
                 *
                 * @return true if this thread is supposed to perform an actual bundled
                 *         operation (burst); false otherwise
                 */
                virtual bool waitForResults(bool fFirst);

            protected:
                /**
                 * Obtain results of the bundled requests. This method should be
                 * implemented by concrete Bundle implementations using the most
                 * efficient mechanism.
                 */
                virtual void ensureResults() = 0;

                /**
                 * Obtain results of the bundled requests or ensure that the results
                 * have already been retrieved.
                 *
                 * @param fBurst  specifies whether or not the actual results have to be
                 *                fetched on this thread; this parameter will be true
                 *                for one and only one thread per bundle
                 *
                 * @return true if the bundling has succeeded; false if the un-bundling
                 *         has to be performed as a result of a failure
                 */
                virtual bool ensureResults(bool fBurst);

                /**
                 * Release all bundle resources associated with the current thread.
                 *
                 * @return true if all entered threads have released
                 */
                virtual bool releaseThread();

            // ----- statistics and debugging  -----------------------------------

            public:
                /**
                 * Reset statistics for this Bundle.
                 */
                virtual void resetStatistics();

            // ----- Object interface -----------------------------------

            protected:
                /**
                 * {@inheritDoc}
                 */
                virtual TypedHandle<const String> toString() const;

            protected:
                /**
                 * Return a human readable name for the specified status value.
                 *
                 * @param iStatus  the status value to format
                 *
                 * @return a human readable status name
                 */
                virtual String::View formatStatusName(int32_t iStatus) const;

            // ----- data fields and constants ---------------------------------

            public:
                /**
                 * This Bundle accepting additional items.
                 */
                static const int32_t status_open      = 0;

                /**
                 * This Bundle is closed for accepting additional items and awaiting
                 * for the execution results.
                 */
                static const int32_t status_pending   = 1;

                /**
                 * This Bundle is in process of returning the result of execution
                 * back to the client.
                 */
                static const int32_t status_processed = 2;

                /**
                 * Attempt to bundle encountered and exception; the execution has to be
                 * de-optimized and performed by individual threads.
                 */
                static const int32_t status_exception = 3;

            private:
                /**
                 * The bundler the operations are performed on.
                 */
                FinalHandle<AbstractBundler> f_hBundler;

                /**
                 * This Bundle status.
                 */
                Volatile<int32_t> m_iStatus;

                /**
                 * A count of threads that are using this Bundle.
                 */
                int32_t m_cThreads;

                /**
                 * A flag that differentiates the "master" bundle which is responsible
                 * for all auto-adjustments. It's set to "true" for one and only one
                 * Bundle object.
                 */
                bool m_fMaster;

                // stat fields intentionally have the "package private" access to
                // prevent generation of synthetic access methods

            protected:
                /**
                 * Statistics: the total number of times this Bundle has been used for
                 * bundled request processing.
                 */
                Volatile<int64_t> m_cTotalBundles;

                /**
                 * Statistics: the total size of individual requests processed by this
                 * Bundle expressed in the same units as values returned by the
                 * {@link Bundle::getBundleSize()} method.
                 */
                Volatile<int64_t> m_cTotalSize;

                /**
                 * Statistics: a timestamp of the first thread entering the bundle.
                 */
                int64_t m_ldtStart;

                /**
                 * Statistics: a total time duration this Bundle has spent in bundled
                 * request processing (burst).
                 */
                Volatile<int64_t> m_cTotalBurstDuration;

                /**
                 * Statistics: a total time duration this Bundle has spent waiting
                 * for bundle to be ready for processing.
                 */
                Volatile<int64_t> m_cTotalWaitDuration;
            };

    // ----- inner class: Statistics -------------------------------------------

    /**
     * Statistics class contains the latest bundler statistics.
     */
    protected:
        class COH_EXPORT Statistics
            : public class_spec<Statistics>
            {
            friend class factory<Statistics>;
            friend class AbstractBundler;

            public:
                /**
                 * Reset the statistics.
                 */
                virtual void reset();

            // ----- Object interface -----------------------------------

            public:
                /**
                 * {@inheritDoc}
                 */
                virtual TypedHandle<const String> toString() const;

            // ----- running averages ------------------------------------------

            protected:
                /**
                 * An average time for bundled request processing (burst).
                 */
                int32_t m_cAverageBurstDuration;

                /**
                 * An average bundle size for this Bundler.
                 */
                int32_t m_cAverageBundleSize;

                /**
                 * An average thread waiting time caused by under-filled bundle. The
                 * wait time includes the time spend in the bundled request processing.
                 */
                int32_t m_cAverageThreadWaitDuration;

                /**
                 * An average bundled request throughput in size units per millisecond
                 * (total bundle size over total processing time)
                 */
                int32_t m_nAverageThroughput;

                // ----- snapshots --------------------------------------------------

                /**
                 * Snapshot for a total number of processed bundled.
                 */
                int64_t m_cBundleCountSnapshot;

                /**
                 * Snapshot for a total size of processed bundled.
                 */
                int64_t m_cBundleSizeSnapshot;

                /**
                 * Snapshot for a burst duration.
                 */
                int64_t m_cBurstDurationSnapshot;

                /**
                 * Snapshot for a combined thread waiting time.
                 */
                int64_t m_cThreadWaitSnapshot;
            };

    // ----- sublcassing support --------------------------------------------

    protected:
        /**
         * Initialize the bundler.
         *
         * @param hBundle              specifies the bundle for this bundler
         */
        virtual void init(Bundle::Handle hBundle);

        /**
         * Retrieve any Bundle that is currently in the open state. This method
         * does not assume any external synchronization and as a result, a
         * caller must double check the returned bundle open state (after
         * synchronizing on it).
         *
         * @return an open Bundle
         */
        virtual Bundle::Handle getOpenBundle();

        /**
         * Instantiate a new Bundle object.
         *
         * @return a new Bundle object
         */
        virtual Bundle::Handle instantiateBundle() = 0;

    // ----- constants and data fields ---------------------------------------

    public:
        /**
         * Frequency of the adjustment attempts. This number represents a number of
         * iterations of the master bundle usage after which an adjustment attempt
         * will be performed.
         */
        static const int32_t adjustment_frequency = 128;

    protected:
        /**
         * Indicate whether this is a first time adjustment.
         */
        bool m_fFirstAdjustment;

        /**
         * The previous bundle size threshold value.
         */
        double m_dPreviousSizeThreshold;

        /**
         * A pool of Bundle objects. Note that this list never shrinks.
         */
        FinalHandle<List> f_hListBundle;

        /**
         * A counter for the total number of threads that have started any bundle
         * related execution. This counter is used by subclasses to reduce an impact
         * of bundled execution for lightly loaded environments.
         */
        FinalHandle<AtomicCounter> f_hCountThreads;

    private:
        /**
         * The bundle size threshold. We use double for this value to allow for
         * fine-tuning of the auto-adjust algorithm.
         *
         * @see adjust
         */
        double m_dSizeThreshold;

        /**
         * The minimum number of threads that should trigger the bundler to switch
         * from a pass through mode to a bundled mode.
         */
        int32_t m_cThreadThreshold;

        /**
         * Specifies whether or not auto-adjustment is on. Default value is "true".
         */
        bool m_fAllowAuto;

        /**
         * The delay timeout in milliseconds. Default value is one millisecond.
         */
        int64_t m_lDelayMillis;

        /**
         * Last active (open) bundle position.
         */
        Volatile<int32_t> m_iActiveBundle;

        /**
         * An instance of the Statistics object containing the latest statistics.
         */
        FinalHandle<Statistics> f_hStats;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_ABSTRACT_BUNDLER_HPP
