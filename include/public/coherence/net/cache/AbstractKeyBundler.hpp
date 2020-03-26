/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_KEY_BUNDLER_HPP
#define COH_ABSTRACT_KEY_BUNDLER_HPP

#include "coherence/lang.ns"

#include "coherence/net/cache/AbstractBundler.hpp"

#include "coherence/util/Collection.hpp"
#include "coherence/util/Collections.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::util::Collection;
using coherence::util::Collections;
using coherence::util::Map;
using coherence::util::Set;

/**
 * An abstract key-based bundler serves as a base for NamedCache get() and
 * remove() operation bundling.
 *
 * @author gg 2007.01.28
 * @author lh 2012.06.05
 * @since Coherence 12.1.2
 */
class COH_EXPORT AbstractKeyBundler
    : public abstract_spec<AbstractKeyBundler,
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
        AbstractKeyBundler();

    private:
        /**
         * Blocked copy constructor.
         */
        AbstractKeyBundler(const AbstractKeyBundler&);

    // ----- bundling support -----------------------------------------------

    public:
        /**
         * Process the specified key in a most optimal way according to the
         * bundle settings.
         *
         * @param vKey  the key to process
         *
         * @return an execution result according to the caller's contract
         */
        Object::Holder process(Object::View vKey);

        /**
         * Process a colKeys of specified items in a most optimal way according to
         * the bundle settings.
         *
         * @param vColKeys  the collection of keys to process
         *
         * @return an execution result according to the caller's contract
         */
        Map::View processAll(Collection::View vColKeys);

        // ----- subclassing support --------------------------------------------

        /**
         * The bundle operation to be performed against a collected set of keys
         * by the concrete AbstractKeyBundler implementations. If an exception
         * occurs during bundle operation, it could be repeated using singleton sets.
         *
         * @param vColKeys  a key collection to perform the bundled operation for
         *
         * @return the Map of operation results
         */
        virtual Map::View bundle(Collection::View vColKeys) = 0;

        /**
         * Un-bundle bundled operation. This operation would be used if an exception
         * occurs during a bundled operation or if the number of active threads is
         * below the {@link #getThreadThreshold() ThreadThreshold} value.
         *
         * @param ohKey  a key to perform the un-bundled operation for
         *
         * @return the operation result for the specified key, may be NULL
         */
        virtual Object::Holder unbundle(Object::View vKey) = 0;

        // ----- AbstractBundler methods ------------------------------------

        /**
         * {@inheritDoc}
         */
        virtual Bundle::Handle instantiateBundle();

    // ----- inner class: Bundle --------------------------------------------

    /**
     * Bundle represents a unit of optimized execution.
     */
    protected:
        class COH_EXPORT Bundle
            : public class_spec<Bundle,
                extends<AbstractBundler::Bundle> >
            {
            friend class factory<Bundle>;

            // ----- constructors -------------------------------------

            protected:
                /**
                 * Default constructor.
                 *
                 * @param hBundler  the AbstructBundler
                 */
                Bundle(AbstractBundler::Handle hBundler);

            // ----- bundling support -----------------------------------

            public:
                /**
                 * Add the specified key to the Bundle.
                 *
                 * <b>Note:</b> a call to this method must be externally synchronized
                 * for this Bundle object.
                 *
                 * @param ohKey  the key to add to this Bundle
                 *
                 * @return true if this Bundle was empty prior to this call
                 */
                virtual bool add(Object::Holder ohKey);

                /**
                 * Add the specified collection of keys to the Bundle.
                 *
                 * <b>Note:</b> a call to this method must be externally synchronized
                 * for this Bundle object.
                 *
                 * @param vColKeys  the collection of keys to add to this Bundle
                 *
                 * @return true if this Bundle was empty prior to this call
                 */
                virtual bool addAll(Collection::View vColKeys);

                /**
                 * Process the specified key according to this Bundle state.
                 *
                 * @param fBurst  true if this thread is supposed to perform an actual
                 *                bundled operation (burst); false otherwise
                 * @param ohKey   the key to process
                 *
                 * @return an execution result according to the caller's contract
                 */
                virtual Object::Holder process(bool fBurst, Object::Holder ohKey);

                /**
                 * Process the specified kye collection according to this Bundle state.
                 *
                 * @param fBurst    true if this thread is supposed to perform an actual
                 *                  bundled operation (burst); false otherwise
                 * @param vColKeys  the collection of keys to process
                 *
                 * @return an execution result according to the caller's contract
                 */
                virtual Map::View processAll(bool fBurst, Collection::View vColKeys);

                // ----- AbstractBundler::Bundle methods-------------------------

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

            // ----- data fields ----------------------------------------

            private:
                /**
                 * This bundle content.
                 */
                FinalHandle<Set> f_hSetKeys;

                /**
                 * A result of the bundled processing.
                 */
                MemberView<Map> m_vMapResults;
            };
    };

COH_CLOSE_NAMESPACE3

#endif // COH_ABSTRACT_KEY_BUNDLER_HPP
