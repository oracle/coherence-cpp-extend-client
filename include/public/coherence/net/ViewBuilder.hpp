/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_VIEW_BUILDER_HPP
#define COH_VIEW_BUILDER_HPP

#include "coherence/lang.ns"

#include "coherence/util/Filter.hpp"
#include "coherence/util/MapListener.hpp"
#include "coherence/util/Supplier.hpp"
#include "coherence/util/ValueExtractor.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::util::Filter;
using coherence::util::MapListener;
using coherence::util::Supplier;
using coherence::util::ValueExtractor;

// forward reference to avoid circular header inclusion
class NamedCache;

/**
 * The ViewBuilder provides a means to build() a view (ContinuousQueryCache)
 * using a fluent pattern / style.
 *
 * @see ContinuousQueryCache
 *
 * @author rl 6.2.19
 * @since 12.2.1.4
 */
class COH_EXPORT ViewBuilder
    : public class_spec<ViewBuilder,
        extends<Object> >
    {
    friend class factory<ViewBuilder>;

    // ----- handle definitions ---------------------------------------------

     public:
        /**
        * NamedCache Handle definition.
        */
        typedef TypedHandle<NamedCache> NamedCacheHandle;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
         * Construct a new ViewBuilder for the provided NamedCache.
         *
         * @param hCache  the NamedCache from which the view will be created
         */
        ViewBuilder(NamedCacheHandle hCache);

        /**
         * Construct a new ViewBuilder for the provided NamedCache.
         * The Supplier should return a new NamedCache instance upon
         * each invocation.
         *
         * @param hSupplierCache  the Supplier returning a NamedCache
         *                        from which the view will be created
         */
        ViewBuilder(Supplier::Handle hSupplierCache);

    private:
        /**
        * Blocked copy constructor.
        */
        ViewBuilder(const ViewBuilder&);

    // ----- builder interface ----------------------------------------------

    public:
        /**
         * The Filter that will be used to define the entries maintained in this view.
         * If no Filter is specified, AlwaysFilter#INSTANCE will be used.
         *
         * @param vFilter  the Filter that will be used to query the
         *                 underlying NamedCache
         *
         * @return this ViewBuilder
         */
        ViewBuilder::Handle filter(Filter::View vFilter);

        /**
         * The MapListener that will receive all events, including those that
         * result from the initial population of the view.
         *
         * @param hListener  the MapListener that will receive all the events from
         *                   the view, including those corresponding to its initial
         *                   population.
         *
         * @return this ViewBuilder
         */
        ViewBuilder::Handle listener(MapListener::Handle hListener);

        /**
         * The ValueExtractor that this view will use to transform the results from
         * the underlying cache prior to storing them locally.
         *
         * @param vMapper  the ValueExtractor that will be used to
         *                 transform values retrieved from the underlying cache
         *                 before storing them locally; if specified, this
         *                 view will become read-only
         *
         * @return this ViewBuilder
         */
        ViewBuilder::Handle map(ValueExtractor::View VMapper);

        /**
         * The resulting view will only cache keys.
         *
         * NOTE: this is mutually exclusive with values().
         *
         * @return this ViewBuilder
         */
        ViewBuilder::Handle keys();

        /**
         * The resulting view with cache both keys and values.
         *
         * NOTE: this is mutually exclusive with keys().
         *
         * @return this ViewBuilder
         */
        ViewBuilder::Handle values();

        /**
         * Construct a view of the NamedCache provided to this builder.
         *
         * @return the view of the NamedCache provided to this builder
         */
        NamedCacheHandle build();

    // ----- data members ---------------------------------------------------

    protected:
        /**
         * The Supplier returning a NamedCache from which the
         * view will be created.
         */
        FinalHolder<Supplier> f_hSupplierCache;

        /**
         * The Filter that will be used to define the entries maintained
         * in this view.
         */
        MemberView<Filter> m_vFilter;

        /**
         * The MapListener that will receive all the events from
         * the view, including those corresponding to its initial
         * population.
         */
        MemberHandle<MapListener> m_hListener;

        /**
         * The ValueExtractor that will be used to transform values
         * retrieved from the underlying cache before storing them locally; if
         * specified, this view will become read-only.
         */
        MemberView<ValueExtractor> m_vMapper;

        /**
         * Flag controlling if the view will cache both keys and values
         * or only keys.
         */
        bool m_fCacheValues;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_VIEW_BUILDER_HPP
