/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_ENTRY_BUNDLER_HPP
#define COH_ABSTRACT_ENTRY_BUNDLER_HPP

#include "coherence/lang.ns"

#include "coherence/net/cache/AbstractBundler.hpp"

#include "coherence/util/AtomicCounter.hpp"
#include "coherence/util/Collections.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/SafeHashMap.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::util::AtomicCounter;
using coherence::util::Collections;
using coherence::util::Map;
using coherence::util::SafeHashMap;

/**
 * An abstract entry-based bundler serves as a base for NamedCache::put()
 * operation bundling.
 *
 * @author gg 2007.01.28
 * @author lh 2012.05.06
 * @since Coherence 12.1.2
 */
class COH_EXPORT AbstractEntryBundler
    : public abstract_spec<AbstractEntryBundler,
        extends<AbstractBundler> >
    {
    // ----- handle definitions (needed for nested classes) -----------------

    public:
        typedef this_spec::Handle Handle;
        typedef this_spec::View   View;
        typedef this_spec::Holder Holder;

    // ----- constructors ---------------------------------------------------

    protected:
       /**
        * @internal
        */
        AbstractEntryBundler();

    private:
       /**
        * Blocked copy constructor.
        */
        AbstractEntryBundler(const AbstractEntryBundler&);

    // ----- bundling support -----------------------------------------------

    public:
        /**
         * Process the specified entry in a most optimal way according to the
         * bundle settings.
         *
         * @param vKey     the entry key
         * @param ohValue  the entry value
         */
        virtual void process(Object::View vKey, Object::Holder ohValue);
    
        /**
         * Process a colllection of entries in a most optimal way according to the
         * bundle settings.
         *
         * @param vMap  the collection of entries to process
         */
        virtual void processAll(Map::View vMap);

    // ----- subclassing support --------------------------------------------

    protected:
        /**
         * The bundle operation to be performed against a collected map of entries
         * by the concrete AbstractEntryBundler implementations. If an exception
         * occurs during bundle operation, it will be repeated using singleton maps.
         *
         * @param vMapEntries  a map to perform the bundled operation for
         */
        virtual void bundle(Map::View vMapEntries) = 0;

    // ----- AbstractBundler methods ----------------------------------------

        /**
         * {@inheritDoc}
         */
        virtual AbstractBundler::Bundle::Handle instantiateBundle();

    // ----- inner class: Bundle --------------------------------------------

    /*
     * Bundle represents a unit of optimized execution.
     */
    protected:
        class COH_EXPORT Bundle
            : public class_spec<Bundle,
                extends<AbstractBundler::Bundle> >
        {
        friend class factory<AbstractEntryBundler::Bundle>;

        protected:
            /**
             * Default constructor.
             *
             * @param hBundler  the AbstructBundler
             */
             Bundle(AbstractBundler::Handle hBundler);

        // ----- bundling support ---------------------------------------

        public:
            /**
             * Add the specified entry to the Bundle.
             *
             * <b>Note:</b> a call to this method must be externally synchronized
             * for this Bundle object.
             *
             * @param vKey     the entry key
             * @param ohValue  the entry value
             *
             * @return true if this Bundle was empty prior to this call
             */
            virtual bool add(Object::View vKey, Object::Holder ohValue);

            /**
             * Add the specified collection of entries to the Bundle.
             *
             * <b>Note:</b> a call to this method must be externally synchronized
             * for this Bundle object.
             *
             * @param vMap  the collection of entries
             *
             * @return true if this Bundle was empty prior to this call
             */
            virtual bool addAll(Map::View vMap);

            /**
             * Process the specified entry according to this Bundle state.
             *
             * @param fBurst   true if this thread is supposed to perform an actual
             *                 bundled operation (burst); false otherwise
             * @param vKey     the entry key
             * @param ohValue  the entry value
             */
            virtual void process(bool fBurst, Object::View vKey, Object::Holder ohValue);

            /**
             * Process the specified collection of entries according to this Bundle
             * state.
             *
             * @param fBurst  true if this thread is supposed to perform an actual
             *                bundled operation (burst); false otherwise
             * @param vMap    the collection of entries
             */
            virtual void processAll(bool fBurst, Map::View vMap);

            // ----- AbstractBundler::Bundle methods---------------------

            /**
             * {@inheritDoc}
             */
            virtual int32_t getBundleSize() const;

            /**
             * {@inheritDoc}
             */
            using AbstractBundler::Bundle::ensureResults;

            /**
             * {@inheritDoc}
             */
            virtual void ensureResults();

            /**
             * {@inheritDoc}
             */
            virtual bool releaseThread();

        // ----- data fileds --------------------------------------------

        private:
            /**
             * This bundle content.
             */
            FinalHandle<Map> f_hMapEntries;
        };
    };
    
COH_CLOSE_NAMESPACE3
    
#endif // COH_ABSTRACT_ENTRY_BUNDLER_HPP
