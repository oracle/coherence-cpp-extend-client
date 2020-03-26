/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/ViewBuilder.hpp"

#include "coherence/net/NamedCache.hpp"

#include "coherence/net/cache/ContinuousQueryCache.hpp"

#include "coherence/util/Filter.hpp"
#include "coherence/util/MapListener.hpp"
#include "coherence/util/Supplier.hpp"
#include "coherence/util/ValueExtractor.hpp"

#include "coherence/util/filter/AlwaysFilter.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::net::NamedCache;
using coherence::net::ViewBuilder;
using coherence::net::cache::ContinuousQueryCache;
using coherence::util::Filter;
using coherence::util::MapListener;
using coherence::util::Supplier;
using coherence::util::ValueExtractor;
using coherence::util::filter::AlwaysFilter;

COH_OPEN_NAMESPACE_ANON(ViewBuilder)

/**
* Simple Supplier implementation that returns the NamedCache
* provided at construction.
*/
class COH_EXPORT CacheSupplier
        : public class_spec<CacheSupplier,
          extends<Object>,
          implements<Supplier> >
    {
    friend class factory<CacheSupplier>;

    // ---- constructors ----------------------------------------------------

    protected:
        /**
        * Create a new Supplier that returns the NamedCache
        * provided here.
        *
        * @param hNamedCache the NamedCache
        */
        CacheSupplier(NamedCache::Handle hNamedCache)
                : f_hNamedCache(self(), hNamedCache)
            {
            }

    private:
        /**
        * Blocked copy constructor.
        */
        CacheSupplier(const CacheSupplier&);
    
    // ----- Supplier interface -----------------------------------------
    
    public:
        virtual Object::Holder get() const
            {
            return f_hNamedCache;
            }

        virtual Object::Holder get()
            {
            return f_hNamedCache;
            }
    
    // ----- data members -----------------------------------------------
    
    protected:
        FinalHandle<NamedCache> f_hNamedCache;
    };

COH_CLOSE_NAMESPACE_ANON

// ----- constructors -------------------------------------------------------

ViewBuilder::ViewBuilder(NamedCache::Handle hCache)
    : f_hSupplierCache(self(), CacheSupplier::create(hCache)),
      m_vFilter(self(), NULL),
      m_hListener(self(), NULL),
      m_vMapper(self(), NULL),
      m_fCacheValues(false)
    {
    }

ViewBuilder::ViewBuilder(Supplier::Handle hSupplierCache)
    : f_hSupplierCache(self(), hSupplierCache),
      m_vFilter(self(), NULL),
      m_hListener(self(), NULL),
      m_vMapper(self(), NULL),
      m_fCacheValues(false)
    {
    }

// ----- ViewBuilder methods ------------------------------------------------

ViewBuilder::Handle ViewBuilder::filter(Filter::View hFilter)
    {
    m_vFilter = hFilter;
    return this;
    }

ViewBuilder::Handle ViewBuilder::listener(MapListener::Handle hListener)
    {
    m_hListener = hListener;
    return this;
    }

ViewBuilder::Handle ViewBuilder::map(ValueExtractor::View hMapper)
    {
    m_vMapper = hMapper;
    return this;
    }

ViewBuilder::Handle ViewBuilder::keys()
    {
    m_fCacheValues = false;
    return this;
    }

ViewBuilder::Handle ViewBuilder::values()
    {
    m_fCacheValues = true;
    return this;
    }

NamedCache::Handle ViewBuilder::build()
    {
    Filter::View vFilter = m_vFilter;
    if (vFilter == NULL)
        {
        vFilter = AlwaysFilter::getInstance();
        }
    return ContinuousQueryCache::create(f_hSupplierCache,
                                        vFilter,
                                        m_fCacheValues,
                                        m_hListener,
                                        m_vMapper);
    }

COH_CLOSE_NAMESPACE2
