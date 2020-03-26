/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INVOCABLE_MAP_HPP
#define COH_INVOCABLE_MAP_HPP

#include "coherence/lang.ns"

#include "coherence/util/Collection.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/QueryMap.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/ValueUpdater.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* Map with additional query features.
*
* @author jh  2008.02.27
*/
class COH_EXPORT InvocableMap
    : public interface_spec<InvocableMap,
        implements<Map> >
    {
    // ----- inner interface: Entry -----------------------------------------

    public:
        /**
        * An InvocableMap::Entry contains additional information and exposes
        * additional operations that the basic Map::Entry does not. It allows
        * non-existent entries to be represented, thus allowing their
        * optional creation. It allows existent entries to be removed from
        * the Map. It supports a number of optimizations that can ultimately
        * be mapped through to indexes and other data structures of the
        * underlying Map.
        */
        class COH_EXPORT Entry
            : public interface_spec<Entry,
                implements<QueryMap::Entry> >
            {
            // ----- InvocableMap::Entry interface ----------------------

            public:
                /**
                * Store the value corresponding to this entry. If the entry
                * does not exist, then the entry will be created by invoking
                * this method, even with a NULL value (assuming the Map
                * supports NULL values).
                *
                * Unlike the other form of {@link #setValue(Object::Holder)
                * setValue}, this form does not return the previous value,
                * and as a result may be significantly less expensive (in
                * terms of cost of execution) for certain Map
                * implementations.
                *
                * @param ohValue     the new value for this Entry
                * @param fSynthetic  pass true only if the insertion into or
                *                    modification of the Map should be
                *                    treated as a synthetic event
                *
                * @return the prior value
                */
                virtual Object::Holder setValue(Object::Holder ohValue,
                        bool fSynthetic) = 0;

                /**
                * Update the Entry's value. Calling this method is
                * semantically equivalent to:
                * <pre>
                *   Object::Handle hTarget = cast<Object::Handle>(
                *           hEntry->getValue());
                *   vUpdater->update(hTarget, ohValue);
                *   hEntry->setValue(hTarget, false);
                * </pre>
                * The benefit of using this method is that it may allow the
                * Entry implementation to significantly optimize the
                * operation, such as for purposes of delta updates and backup
                * maintenance.
                *
                * @param vUpdater  a ValueUpdater used to modify the Entry's
                *                  value
                * @param ohValue   the new value to use
                */
                virtual void update(ValueUpdater::View vUpdater,
                        Object::Holder ohValue) = 0;

                /**
                * Determine if this Entry exists in the Map. If the Entry is
                * not present, it can be created by calling {@link
                * #setValue}. If the Entry is present, it can be destroyed by
                * calling {@link #remove}.
                *
                * @return true iff this Entry is existent in the containing
                *         Map
                */
                virtual bool isPresent() const = 0;

                /**
                * Remove this Entry from the Map if it is present in the Map.
                * This method supports both the operation corresponding to
                * Map#remove as well as synthetic operations such as
                * eviction. If the containing Map does not differentiate
                * between the two, then this method will always be identical
                * to calling <tt>remove(getKey())</tt> on the containing Map.
                *
                * @param fSynthetic  pass true only if the removal from the
                *                    Map should be treated as a synthetic
                *                    event
                */
                virtual void remove(bool fSynthetic) = 0;

            // ----- QueryMap::Entry interface --------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Object::Holder extract(ValueExtractor::View vExtractor) const = 0;

            // ----- Map::Entry interface -------------------------------

            public:
                /**
                * Return the key corresponding to this entry. The resultant
                * key does not necessarily exist within the containing Map,
                * which is to say that calling <tt>containsKey(getKey())</tt>
                * on the containing Map could return false. To test for the
                * presence of this key within the Map, use {@link
                * #isPresent}, and to create the entry for the key, use
                * {@link #setValue}.
                *
                * @return the key corresponding to this entry; may be NULL if
                *         the underlying Map supports NULL keys
                */
                using Map::Entry::getKey;

                /**
                * Return the value corresponding to this entry. If the entry
                * does not exist, then the value will be NULL. To
                * differentiate between a NULL value and a non-existent
                * entry, use {@link #isPresent}.
                *
                * <b>Note:</b> any modifications to the value retrieved using
                * this method are not guaranteed to persist unless followed
                * by a {@link #setValue} or {@link #update} call.
                *
                * @return the value corresponding to this entry; may be NULL
                *         if the value is NULL or if the Entry does not exist
                *         in the Map
                */
                using Map::Entry::getValue;

                /**
                * Store the value corresponding to this entry. If the entry
                * does not exist, then the entry will be created by invoking
                * this method, even with a NULL value (assuming the Map
                * supports NULL values).
                *
                * @param ohValue  the new value for this Entry
                *
                * @return the previous value of this Entry, or NULL if the
                *         Entry did not exist
                */
                using Map::Entry::setValue;
            };


    // ----- inner interface: EntryProcessor --------------------------------

    public:
        /**
        * An invocable agent that operates against the Entry objects within a
        * Map.
        */
        class COH_EXPORT EntryProcessor
            : public interface_spec<EntryProcessor>
            {
            // ----- EntryProcessor interface ---------------------------

            public:
                /**
                * Process a Map::Entry object.
                *
                * @param hEntry  the Entry to process
                *
                * @return the result of the processing, if any
                */
                virtual Object::Holder process(
                        Entry::Handle hEntry) const = 0;

                /**
                * Process a Set of InvocableMap::Entry objects. This method
                * is semantically equivalent to:
                * <pre>
                *   Map::Handle hMapResults = HashMap::create();
                *   for (Iterator::Handle hIter = vSetEntries->iterator();
                *       iter->hasNext; )
                *       {
                *       InvocableMap::Entry::Handle hEntry =
                *               cast<InvocableMap::Entry::Handle>(hIter->next());
                *       hMapResults->put(hEntry->getKey(), process(hEntry));
                *       }
                *   return hMapResults;
                * </pre>
                *
                * @param vSetEntries  a Set of InvocableMap::Entry objects
                *                     to process
                *
                * @return a Map containing the results of the processing, up
                *         to one entry for each InvocableMap::Entry that was
                *         processed, keyed by the keys of the Map that were
                *         processed, with a corresponding value being the
                *         result of the processing for each key
                */
                virtual Map::View processAll(Set::View vSetEntries) const = 0;
            };


    // ----- inner interface: EntryAggregator -------------------------------

    public:
        /**
        * An EntryAggregator represents processing that can be directed to
        * occur against some subset of the entries in an InvocableMap,
        * resulting in a aggregated result. Common examples of aggregation
        * include functions such as min(), max() and avg(). However, the
        * concept of aggregation applies to any process that needs to
        * evaluate a group of entries to come up with a single answer.
        */
        class COH_EXPORT EntryAggregator
            : public interface_spec<EntryAggregator>
            {
            // ----- EntryAggregator interface --------------------------

            public:
                /**
                * Process a set of InvocableMap::Entry objects in order to
                * produce an aggregated result.
                *
                * @param vSetEntries  a Set of read-only InvocableMap::Entry
                *                     objects to aggregate
                *
                * @return the aggregated result from processing the entries
                */
                virtual Object::Holder aggregate(Set::View vSetEntries) = 0;
            };


    // ----- inner interface: ParallelAwareAggregator -----------------------

    public:
        /**
        * A ParallelAwareAggregator is an advanced extension to
        * EntryAggregator that is explicitly capable of being run in
        * parallel, for example in a distributed environment.
        */
        class COH_EXPORT ParallelAwareAggregator
            : public interface_spec<ParallelAwareAggregator,
                implements<EntryAggregator> >
            {
            // ----- ParallelAwareAggregator interface ------------------

            public:
                /**
                * Get an aggregator that can take the place of this
                * aggregator in situations in which the InvocableMap can
                * aggregate in parallel.
                *
                * @return the aggregator that will be run in parallel
                */
                virtual EntryAggregator::Handle getParallelAggregator() = 0;

                /**
                * Aggregate the results of the parallel aggregations.
                *
                * @param vCollResults  the parallel aggregation results
                *
                * @return the aggregation of the parallel aggregation results
                */
                virtual Object::Holder aggregateResults(
                        Collection::View vCollResults) = 0;

            // ----- inner interface: PartialResultAggregator ---------------

            public:
                /**
                 * PartialResultAggregator allows for the intermediate {@link
                 * #aggregatePartialResults aggregation} of the partial results of a {@link
                 * ParallelAwareAggregator parallel aggregation}.
                 */
                class COH_EXPORT PartialResultAggregator
                    : public interface_spec<PartialResultAggregator>
                    {
                    // ----- PartialResultAggregator interface --------------

                    public:
                    /**
                     * Aggregate the results of the parallel aggregations, producing a
                     * partial result logically representing the partial aggregation. The
                     * returned partial result will be further {@link
                     * ParallelAwareAggregator#aggregateResults aggregated} to produce
                     * the final result.
                     *
                     * @param vColPartialResults  the partial results
                     *
                     * @return an aggregation of the collection of partial results
                     */
                    virtual Object::Holder aggregatePartialResults(
                            Collection::View vColPartialResults) = 0;

                    };
            };


    // ----- InvocableMap interface -----------------------------------------

    public:
        /**
        * Invoke the passed EntryProcessor against the Entry specified by the
        * passed key, returning the result of the invocation.
        *
        * @param vKey    the key to process; it is not required to exist
        *                within the Map
        * @param hAgent  the EntryProcessor to use to process the specified
        *                key
        *
        * @return the result of the invocation as returned from the
        *         EntryProcessor
        */
        virtual Object::Holder invoke(Object::View vKey,
                EntryProcessor::Handle hAgent) = 0;

        /**
        * Invoke the passed EntryProcessor against the entries specified by
        * the passed keys, returning the result of the invocation for each.
        *
        * @param vCollKeys  the keys to process; these keys are not required
        *                   to exist within the Map
        * @param hAgent     the EntryProcessor to use to process the
        *                   specified keys
        *
        * @return a Map containing the results of invoking the EntryProcessor
        *         against each of the specified keys
        */
        virtual Map::View invokeAll(Collection::View vCollKeys,
                EntryProcessor::Handle hAgent) = 0;

        /**
        * Invoke the passed EntryProcessor against the set of entries that
        * are selected by the given Filter, returning the result of the
        * invocation for each.
        *
        * Unless specified otherwise, InvocableMap implementations will
        * perform this operation in two steps: (1) use the filter to retrieve
        * a matching entry set; (2) apply the agent to every filtered entry.
        * This algorithm assumes that the agent's processing does not affect
        * the result of the specified filter evaluation, since the filtering
        * and processing could be performed in parallel on different threads.
        * If this assumption does not hold, the processor logic has to be
        * idempotent, or at least re-evaluate the filter. This could be
        * easily accomplished by wrapping the processor with a
        * ConditionalProcessor.
        *
        * @param vFilter  a Filter that results in the set of keys to be
        *                 processed
        * @param hAgent   the EntryProcessor to use to process the specified
        *                 keys
        *
        * @return a Map containing the results of invoking the EntryProcessor
        *         against the keys that are selected by the given Filter
        */
        virtual Map::View invokeAll(Filter::View vFilter,
                EntryProcessor::Handle hAgent) = 0;

        /**
        * Perform an aggregating operation against the entries specified by
        * the passed keys.
        *
        * @param vCollKeys  the Collection of keys that specify the entries
        *                   within this Map to aggregate across
        * @param hAgent     the EntryAggregator that is used to aggregate
        *                   across the specified entries of this Map
        *
        * @return the result of the aggregation
        */
        virtual Object::Holder aggregate(Collection::View vCollKeys,
                EntryAggregator::Handle hAgent) const = 0;

        /**
        * Perform an aggregating operation against the set of entries that
        * are selected by the given Filter.
        *
        * @param vFilter  the Filter that is used to select entries within
        *                 this Map to aggregate across
        * @param hAgent   the EntryAggregator that is used to aggregate
        *                 across the selected entries of this Map
        *
        * @return the result of the aggregation
        */
        virtual Object::Holder aggregate(Filter::View vFilter,
                EntryAggregator::Handle hAgent) const = 0;

        /**
        * Get the value associated with the given key or return the default value if
        * there is no value associated with the key.
        *
        * @param vKey            the key
        * @param ohDefaultValue  the default value
        *
        * @return the value to which the specified key is mapped,
        *         or defaultValue if this map contains no mapping for the key
        *
        * @since Coherence 12.2.1
        */
        virtual Object::Holder getOrDefault(Object::View vKey,
                Object::Holder ohDefaultValue) const;

        /**
        * If the specified key is not already associated with a value (or is
        * mapped to null) associates it with the given value and returns null,
        * else returns the current value.
        *
        * @param vKey    the key
        * @param vValue  the value
        *
        * @return the previous value associated with the specified key,
        *         or null if there was no mapping for the key.
        *
        * @since Coherence 12.2.1
        */
        virtual Object::Holder putIfAbsent(Object::View vKey,
                Object::Holder ohValue);

        /**
        * Removes the entry for the specified key only if it is currently
        * mapped to the specified value.
        *
        * @param vKey    the key
        * @param vValue  the value
        *
        * @return true if the value was removed
        *
        * @since Coherence 12.2.1
        */
        virtual bool removeValue(Object::View vKey,
                Object::View vValue);

        /**
        * Replaces the entry for the specified key only if it is currently
        * mapped to some value.
        *
        * @param vKey    the key
        * @param vValue  the value
        *
        * @return the previous value associated with the specified key,
        *         or null if there was no mapping for the key. A null return
        *         can also indicate that the map previously associated null
        *         with the key.
        *
        * @since Coherence 12.2.1
        */
        virtual Object::Holder replace(Object::View vKey,
                Object::Holder ohValue);

        /**
        * Replaces the entry for the specified key only if currently mapped
        * to the specified value.
        *
        * @param vKey       the key
        * @param vOldValue  the old value
        * @param vNewValue  the new value
        *
        * @return true if the value was replaced
        *
        * @since Coherence 12.2.1
        */
        virtual bool replace(Object::View vKey,
                Object::View vOldValue, Object::Holder ohNewValue);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_INVOCABLE_MAP_HPP
