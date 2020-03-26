/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/MapListenerSupport.hpp"

#include "coherence/util/ArrayList.hpp"
#include "coherence/util/ConverterCollections.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/List.hpp"
#include "coherence/util/LiteSet.hpp"
#include "coherence/util/MapEvent.hpp"
#include "coherence/util/MapEventTransformer.hpp"
#include "coherence/util/NullImplementation.hpp"

#include <sstream>

COH_OPEN_NAMESPACE2(coherence,util)

// ----- file local helpers -------------------------------------------------

namespace
    {
    /**
    * Statically create an empty Listeners instance.
    *
    * @return an empty Listeners instance
    */
    Listeners::View getNullListeners()
        {
        static FinalView<Listeners> vListeners(System::common(),
                Listeners::create());
        return vListeners;
        }
    COH_STATIC_INIT(getNullListeners());
    }

// ----- constructors -------------------------------------------------------

MapListenerSupport::MapListenerSupport()
        : m_hmapListeners(self()),
          m_hmapKeyListeners(self()),
          m_hmapStandardListeners(self()),
          m_hmapStandardKeyListeners(self()),
          m_optimizationPlan(none),
          m_vListenersCached(self())
    {
    }


// ----- MapListenerSupport interface ---------------------------------------

void MapListenerSupport::addListener(MapListener::Handle hListener,
        Filter::View vFilter, bool fLite)
    {
    COH_SYNCHRONIZED(this)
        {
        if (hListener != NULL)
            {
            Map::Handle hmapListeners = m_hmapListeners;
            if (NULL == hmapListeners)
                {
                //TODO: Was a LiteMap
                hmapListeners = m_hmapListeners = HashMap::create();
                }
            addSafeListener(hmapListeners, vFilter, hListener);

            Map::Handle hmapStandard = m_hmapStandardListeners;
            if (NULL == hmapStandard)
                {
                hmapStandard = m_hmapStandardListeners = HashMap::create();
                }
            addListenerState(hmapStandard, vFilter, hListener, fLite);

            m_optimizationPlan = none;
            m_vListenersCached = NULL;
            }
        }
    }

void MapListenerSupport::addListener(MapListener::Handle hListener,
        Object::View vKey, bool fLite)
    {
    COH_SYNCHRONIZED(this)
        {
        if (hListener != NULL)
            {
            Map::Handle hmapListeners = m_hmapKeyListeners;
            if (NULL == hmapListeners)
                {
                hmapListeners = m_hmapKeyListeners = HashMap::create();
                }
            addSafeListener(hmapListeners, vKey, hListener);

            Map::Handle hmapStandard = m_hmapStandardKeyListeners;
            if (NULL == hmapStandard)
                {
                hmapStandard = m_hmapStandardKeyListeners = HashMap::create();
                }
            addListenerState(hmapStandard, vKey, hListener, fLite);

            // if the optimization plan was already optimized for key
            // listeners, and the key listener that we just added is the same
            // as was already present, then keep the current plan, otherwise
            // reset it.
            bool fKeepPlan = false;
            if (m_optimizationPlan == key_listener)
                {
                ObjectArray::View vaListeners =
                        m_vListenersCached->listeners();
                if (vaListeners != NULL && vaListeners->length == 1 &&
                        vaListeners[0] == hListener)
                    {
                    fKeepPlan = true;
                    }
                }

            if (!fKeepPlan)
                {
                m_optimizationPlan = none;
                m_vListenersCached = NULL;
                }
            }
        }
    }

void MapListenerSupport::removeListener(MapListener::View vListener,
        Filter::View vFilter)
    {
    COH_SYNCHRONIZED(this)
        {
        if (vListener != NULL)
            {
            Map::Handle hmapListeners = m_hmapListeners;
            if (hmapListeners != NULL)
                {
                removeSafeListener(hmapListeners, vFilter, vListener);

                if (hmapListeners->isEmpty())
                    {
                    m_hmapListeners = NULL;
                    }

                Map::Handle hmapStandard = m_hmapStandardListeners;
                if (hmapStandard != NULL)
                    {
                    removeListenerState(hmapStandard, vFilter, vListener);
                    if (hmapStandard->isEmpty())
                        {
                        m_hmapStandardListeners = NULL;
                        }
                    }
                }
            m_optimizationPlan = none;
            m_vListenersCached = NULL;
            }
        }
    }

void MapListenerSupport::removeListener(MapListener::View vListener,
        Object::View vKey)
    {
    COH_SYNCHRONIZED(this)
        {
        if (vListener != NULL)
            {
            Map::Handle hmapListeners = m_hmapKeyListeners;
            if (hmapListeners != NULL)
                {
                removeSafeListener(hmapListeners, vKey, vListener);
                if (hmapListeners->isEmpty())
                    {
                    m_hmapKeyListeners = NULL;
                    }

                Map::Handle hmapStandard = m_hmapStandardKeyListeners;
                if (hmapStandard != NULL)
                    {
                    removeListenerState(hmapStandard, vKey, vListener);
                    if (hmapStandard->isEmpty())
                        {
                        m_hmapStandardKeyListeners = NULL;
                        }
                    }
                }

            // if the optimization plan was already to optimize for key
            // listeners, and the cached set of key listeners is a set of
            // exactly one listener, and there are still other keys with
            // that same listener registered, then keep the current plan,
            // otherwise reset it
            bool fKeepPlan = false;
            if (key_listener == m_optimizationPlan)
                {
                ObjectArray::View vaListeners =
                        m_vListenersCached->listeners();
                if (vaListeners != NULL && vaListeners->length == 1 &&
                        vaListeners[0] == vListener)
                    {
                    // keep the plan if there are any keys still being
                    // listened to
                    fKeepPlan = (m_hmapKeyListeners != NULL);
                    }
                }

            if (!fKeepPlan)
                {
                m_optimizationPlan = none;
                m_vListenersCached = NULL;
                }
            }
        }
    }

void MapListenerSupport::clear()
    {
    COH_SYNCHRONIZED(this)
        {
        m_hmapListeners            = NULL;
        m_hmapKeyListeners         = NULL;
        m_hmapStandardListeners    = NULL;
        m_hmapStandardKeyListeners = NULL;
        m_optimizationPlan         = no_listeners;
        m_vListenersCached         = NULL;
        }
    }

bool MapListenerSupport::isEmpty() const
    {
    return (NULL == m_hmapListeners) && (NULL == m_hmapKeyListeners);
    }

bool MapListenerSupport::isEmpty(Filter::View vFilter) const
    {
    Map::View vmapListeners = m_hmapListeners;
    return NULL == vmapListeners  || !vmapListeners->containsKey(vFilter);
    }

bool MapListenerSupport::isEmpty(Object::View vKey) const
    {
    Map::View vmapListeners = m_hmapKeyListeners;
    return NULL == vmapListeners || !vmapListeners->containsKey(vKey);
    }

bool MapListenerSupport::containsStandardListeners(
        Filter::View vFilter) const
    {
    Map::View vmapStandard = m_hmapStandardListeners;
    if (NULL == vmapStandard)
        {
        return false;
        }
    Set::View vsetStandard = cast<Set::View>(vmapStandard->get(vFilter));
    return vsetStandard != NULL && !vsetStandard->isEmpty();
    }

bool MapListenerSupport::containsStandardListeners(
        Object::View vKey) const
    {
    Map::View vmapStandard = m_hmapStandardKeyListeners;
    if (NULL == vmapStandard)
        {
        return false;
        }
    Set::View vsetStandard = cast<Set::View>(vmapStandard->get(vKey));
    return vsetStandard != NULL && !vsetStandard->isEmpty();
    }

Set::View MapListenerSupport::getFilterSet() const
    {
    Map::View vmapListeners = m_hmapListeners;
    return NULL == vmapListeners ? (Set::View) NullImplementation::getSet() : vmapListeners->keySet();
    }

Set::View MapListenerSupport::getKeySet() const
    {
    Map::View vmapListeners = m_hmapKeyListeners;
    return NULL == vmapListeners ? (Set::View) NullImplementation::getSet() : vmapListeners->keySet();
    }

Listeners::View MapListenerSupport::getListeners(Filter::View vFilter) const
    {
    COH_SYNCHRONIZED(this)
        {
        // this method is synchronized because the underlying map
        // implementation is not thread safe for "get" operations: it could
        // return NULL while there is a valid entry
        Map::View vmapListeners = m_hmapListeners;
        return NULL == vmapListeners ? (Listeners::View) NULL :
                cast<Listeners::View>(vmapListeners->get(vFilter));
        }
    }

Listeners::View MapListenerSupport::getListeners(Object::View vKey) const
    {
    COH_SYNCHRONIZED(this)
        {
        // this method is synchronized because the underlying map
        // implementation is not thread safe for "get" operations: it could
        // return NULL while there is a valid entry
        Map::View vmapListeners = m_hmapKeyListeners;
        return NULL == vmapListeners ? (Listeners::View) NULL :
                cast<Listeners::View>(vmapListeners->get(vKey));
        }
    }

Listeners::View MapListenerSupport::collectListeners(
        MapEvent::View vEvent)
    {
    switch (m_optimizationPlan)
      {
      case none:
      default:
          COH_SYNCHRONIZED(this)
              {
              // put a plan together
              Map::View vmapAllListeners = m_hmapListeners;
              Map::View vmapKeyListeners = m_hmapKeyListeners;
              if (NULL == vmapAllListeners || vmapAllListeners->isEmpty())
                  {
                  // no standard listeners; check for key listeners
                  if (NULL == vmapKeyListeners ||
                          vmapKeyListeners->isEmpty())
                      {
                      m_optimizationPlan = no_listeners;
                      m_vListenersCached = NULL;
                      }
                  else
                      {
                      // can only do key optimization if all keys have
                      // the same set of listeners registered
                      ObjectArray::View vaPrevious = NULL;
                      for (Iterator::Handle hIter =
                              vmapKeyListeners->values()->iterator();
                              hIter->hasNext(); )
                          {
                          Listeners::View vListeners =
                                  cast<Listeners::View>(hIter->next());
                          if (NULL == vaPrevious)
                              {
                              // assume that they are all the same
                              m_optimizationPlan = key_listener;
                              m_vListenersCached = vListeners;
                              vaPrevious         = vListeners->listeners();
                              }
                          else
                              {
                              ObjectArray::View vaCurrent =
                                      vListeners->listeners();
                              size32_t cCurLen    = vaCurrent->length;
                              size32_t cPrevLen   = vaPrevious->length;
                              bool fOptimize = cCurLen == cPrevLen;
                              if (fOptimize)
                                  {
                                  for (size32_t i = 0; i < cCurLen; ++i)
                                      {
                                      if (vaCurrent[i] != vaPrevious[i])
                                          {
                                          // assumption was incorrect --
                                          // some keys have different
                                          // listeners
                                          fOptimize = false;
                                          break;
                                          }
                                      }
                                  }
                              if (!fOptimize)
                                  {
                                  m_optimizationPlan = no_optimize;
                                  m_vListenersCached = NULL;
                                  break;
                                  }
                              }
                          }
                      }
                  }
              else // there are "all" listeners
                  {
                  // assume no optimizations
                  m_optimizationPlan = no_optimize;
                  m_vListenersCached  = NULL;

                  // it is possible to optimize if there are no key
                  // listeners AND no filtered listeners
                  if (NULL == vmapKeyListeners ||
                          vmapKeyListeners->isEmpty())
                      {
                      // check if there is only one listener and it has
                      // no filter
                      if (vmapAllListeners->size() == 1)
                          {
                          Listeners::View vListeners =
                                  cast<Listeners::View>
                                  (vmapAllListeners->get(NULL));
                          if (vListeners != NULL)
                              {
                              m_optimizationPlan = all_listener;
                              m_vListenersCached = vListeners;
                              }
                          }
                      }
                  }
              COH_ENSURE(m_optimizationPlan != none);
              }
          return collectListeners(vEvent);
          // end of case none/default
      case no_listeners:
          return getNullListeners();

      case all_listener:
          return m_vListenersCached;

      case key_listener:
          return m_hmapKeyListeners->containsKey(vEvent->getKey()) ?
                  m_vListenersCached : getNullListeners();

      case no_optimize:
          // fall through to the full implementation
          break;
      }

    Listeners::Handle hListeners = Listeners::create();

    // add global listeners
    Map::View vmapListeners = m_hmapListeners;
    if (vmapListeners != NULL)
      {
      MapEvent::View vEvt = unwrapEvent(vEvent);
      if (instanceof<FilterEvent::View>(vEvt))
          {
          hListeners->setFilters((cast<FilterEvent::View>(vEvt))->getFilter());
          }

      ObjectArray::View vaFilters = hListeners->getFilters();
      if (NULL == vaFilters)
          {
          // the server sent an event without a specified filter list;
          // attempt to match it to any registered filter-based listeners
          ObjectArray::View vaEntry;
          COH_SYNCHRONIZED(this)
              {
              vaEntry = vmapListeners->entrySet()->toArray();
              }

          List::Handle hListFilters = NULL;
          for (size32_t i = 0, len = vaEntry->length; i < len; i++)
              {
              Map::Entry::View vEntry = cast<Map::Entry::View>(vaEntry[i]);
              Filter::View vFilter    = cast<Filter::View>(vEntry->getKey());
              if (NULL == vFilter || evaluateEvent(vFilter, vEvent))
                  {
                  hListeners->addAll(cast<Listeners::View>(vEntry->getValue()));

                  if (vFilter != NULL)
                      {
                      if (hListFilters == NULL)
                          {
                          hListFilters = ArrayList::create();
                          }
                      hListFilters->add(vFilter);
                      }
                  }
              }

          if (hListFilters != NULL)
              {
              hListeners->setFilters(hListFilters->toArray(ObjectArray::create(hListFilters->size())));
              }
          }
      else
          {
          COH_SYNCHRONIZED(this)
              {
              for (size32_t i = 0, len = vaFilters->length; i < len; ++i)
                  {
                  Filter::View vFilter = cast<Filter::View>(vaFilters[i]);
                  hListeners->addAll(cast<Listeners::View>(vmapListeners->get(vFilter)));
                  }
              }
          }
      }

    // add key listeners, only if the event is not transformed (COH-9355)
    Map::View vmapKeyListeners = m_hmapKeyListeners;
    if (vmapKeyListeners != NULL && !isTransformedEvent(vEvent))
      {
      Listeners::View vLsnrs = cast<Listeners::View>
              (vmapKeyListeners->get(vEvent->getKey()));
      if (vLsnrs != NULL)
          {
          hListeners->addAll(vLsnrs);
          }
      }

    return hListeners;
    }

void MapListenerSupport::fireEvent(MapEvent::Handle hEvent, bool fStrict)
    {
    Listeners::View vListeners = collectListeners(hEvent);
    enrichEvent(hEvent, vListeners)->dispatch(vListeners, fStrict);
    }

MapEvent::Handle MapListenerSupport::convertEvent(MapEvent::View vEvent,
        ObservableMap::Handle hmapConv, Converter::View vConvKey,
        Converter::View vConvVal)
    {
    CacheEvent::View vCacheEvent = cast<CacheEvent::View>(vEvent, false);
    return vCacheEvent != NULL ? (MapEvent::Handle)
            ConverterCollections::ConverterCacheEvent::create(hmapConv,
                    vCacheEvent, vConvKey, vConvVal) : (MapEvent::Handle)
            ConverterCollections::ConverterMapEvent::create(
                    hmapConv, vEvent, vConvKey, vConvVal);
    }

MapEvent::Handle MapListenerSupport::enrichEvent(MapEvent::Handle hEvent,
         Listeners::View vListeners)
    {
    if (!instanceof<MapListenerSupport::FilterEvent::View>(hEvent))
        {
        ObjectArray::View vaFilters = vListeners->getFilters();
        if (vaFilters != NULL)
            {
            FilterEvent::Handle hNewEvent = FilterEvent::create(hEvent->getMap(), hEvent->getId(),
                    hEvent->getKey(), hEvent->getOldValue(),
                    hEvent->getNewValue(),
                    instanceof<CacheEvent::View>(hEvent) 
                        && (cast<CacheEvent::View>(hEvent))->isSynthetic(),
                    instanceof<CacheEvent::View>(hEvent)
                        && (cast<CacheEvent::View>(hEvent))->isPriming(),
                    vaFilters);
            hNewEvent->setEvent(hEvent);
            hEvent = hNewEvent;
            }
        }
    return hEvent;
    }

MapEvent::View MapListenerSupport::unwrapEvent(MapEvent::View vEvt)
    {
    while (instanceof<ConverterCollections::ConverterCacheEvent::View>(vEvt))
        {
        vEvt = (cast<ConverterCollections::ConverterCacheEvent::View>(vEvt))->getCacheEvent();
        }
    return vEvt;
    }

bool MapListenerSupport::isPrimingListener(MapListener::View listener)
    {
    return instanceof<MapListenerSupport::PrimingListener::View>(unwrapListener(listener));
    }

// ----- helper methods -----------------------------------------------------

bool MapListenerSupport::evaluateEvent(Filter::View vFilter, MapEvent::View vEvent)
    {
    if (instanceof<CacheEvent::View>(vEvent) &&
           (cast<CacheEvent::View>(vEvent))->getTransformState() == CacheEvent::non_transformable &&
           instanceof<MapEventTransformer::View>(vFilter))
        {
        // if the event is marked as non-transformable, ensure that it does not
        // get delivered to listeners associated with transformer-filters
        return false;
        }

    return vFilter->evaluate(vEvent);
    }

bool MapListenerSupport::isTransformedEvent(MapEvent::View vEvent)
    {
    return instanceof<CacheEvent::View>(vEvent) &&
            (cast<CacheEvent::View>(vEvent))->getTransformState() == CacheEvent::transformed;
    }

void MapListenerSupport::addSafeListener(Map::Handle hmapListeners,
        Object::View vKey, MapListener::Handle hListener)
    {
    Listeners::Handle hListeners = cast<Listeners::Handle>
            (hmapListeners->get(vKey));
    if (NULL == hListeners)
        {
        hListeners = Listeners::create();
        hmapListeners->put(vKey, hListeners);
        }
    hListeners->add(hListener);
    }

void MapListenerSupport::addSafeListener(Map::Handle hMapListeners,
        Filter::View vAnyFilter, MapListener::Handle hListener)
    {
    Listeners::Handle hListeners = cast<Listeners::Handle>
            (hMapListeners->get(vAnyFilter));
    if (hListeners == NULL)
        {
        hListeners = Listeners::create();
        if (vAnyFilter != NULL)
            {
            ObjectArray::Handle hArray = ObjectArray::create(1);
            hArray[0] = vAnyFilter;
            hListeners->setFilters(hArray);
            }
        hMapListeners->put(vAnyFilter, hListeners);
        }
    hListeners->add(hListener);
    }

void MapListenerSupport::removeSafeListener(Map::Handle hmapListeners,
        Object::View vKey, MapListener::View vListener)
    {
    Listeners::Handle hListeners = cast<Listeners::Handle>
            (hmapListeners->get(vKey));

    if (hListeners != NULL)
        {
        hListeners->remove(vListener);
        if (hListeners->isEmpty())
            {
            hmapListeners->remove(vKey);
            }
        }
    }

void MapListenerSupport::addListenerState(Map::Handle hmapStandardListeners,
        Object::View vKey, MapListener::Handle hListener, bool fLite)
    {
    Set::Handle hSetStandard = cast<Set::Handle>
            (hmapStandardListeners->get(vKey));
    if (fLite)
        {
        if (hSetStandard != NULL)
            {
            hSetStandard->remove(hListener);
            }
        }
    else
        {
        if (NULL == hSetStandard)
            {
            hSetStandard = LiteSet::create();
            hmapStandardListeners->put(vKey, hSetStandard);
            }
        hSetStandard->add(hListener);
        }
    }

void MapListenerSupport::removeListenerState(
        Map::Handle hmapStandardListeners, Object::View vKey,
        MapListener::View vListener)
    {
    Set::Handle hSetStandard = cast<Set::Handle>
            (hmapStandardListeners->get(vKey));
    if (hSetStandard != NULL)
        {
        hSetStandard->remove(vListener);
        if (hSetStandard->isEmpty())
            {
            hmapStandardListeners->remove(vKey);
            }
        }
    }

MapListener::View MapListenerSupport::unwrapListener(MapListener::View vListener)
    {
    while (true)
        {
        if (instanceof<ConverterCollections::ConverterMapListener::View>(vListener))
            {
            vListener = (cast<ConverterCollections::ConverterMapListener::View>(vListener))->getMapListener();
            }
        else if (instanceof<MapListenerSupport::WrapperSynchronousListener::View>(vListener)
                 && !(instanceof<MapListenerSupport::PrimingListener::View>(vListener)))
            {
            vListener = (cast<MapListenerSupport::WrapperSynchronousListener::View>(vListener))->getMapListener();
            }
        else
            {
            return vListener;
            }
        }
    }

// ----- Object interface ---------------------------------------------------

TypedHandle<const String> MapListenerSupport::toString() const
    {
    String::View vs = "Global listeners:";
    if (NULL == m_hmapListeners)
        {
        vs = COH_TO_STRING(vs << " none");
        }
    else
        {
        //TODO: A NPE could occur as a result of a race condition on m_hmapListeners
        for (Iterator::Handle hIter = m_hmapListeners->keySet()->iterator();
                hIter->hasNext();)
            {
            Filter::View vFilter = cast<Filter::View>(hIter->next());

            vs = COH_TO_STRING(vs << "\n  Filter=" << vFilter << "; lite=" << !containsStandardListeners(vFilter));
            }
        }

    vs = COH_TO_STRING(vs << "\nKey listeners:");
    if (NULL == m_hmapKeyListeners)
        {
        vs = COH_TO_STRING(vs << " none");
        }
    else
        {
        for (Iterator::Handle hIter = m_hmapKeyListeners->keySet()->iterator();
                hIter->hasNext();)
            {
            Object::View vKey = hIter->next();

            vs = COH_TO_STRING(vs << "\n  Key=" << vKey << "; lite=" << !containsStandardListeners(vKey));
            }
        }

    return vs;
    }


// ----- inner class: FilterEvent -------------------------------------------

// ----- constructors ---------------------------------------------------

MapListenerSupport::FilterEvent::FilterEvent(ObservableMap::Handle hMap,
        int32_t nId, Object::View vKey, Object::View vValueOld,
        Object::View vValueNew, bool fSynthetic,
        ObjectArray::View vaFilters)
    : super(hMap, nId, vKey, vValueOld, vValueNew, fSynthetic, CacheEvent::transformable),
      f_vaFilters(self(), vaFilters), f_vEvent(self(), NULL)
    {
    COH_ENSURE_PARAM(vaFilters);
    }

MapListenerSupport::FilterEvent::FilterEvent(ObservableMap::Handle hMap,
        int32_t nId, Object::View vKey, Object::View vValueOld,
        Object::View vValueNew, bool fSynthetic, bool fPriming,
        ObjectArray::View vaFilters)
    : super(hMap, nId, vKey, vValueOld, vValueNew, fSynthetic, CacheEvent::transformable, fPriming),
      f_vaFilters(self(), vaFilters), f_vEvent(self(), NULL)
    {
    COH_ENSURE_PARAM(vaFilters);
    }

MapListenerSupport::FilterEvent::FilterEvent(ObservableMap::Handle hMap,
        int32_t nId, Object::View vKey, Object::View vValueOld,
        Object::View vValueNew, bool fSynthetic, CacheEvent::TransformationState nTransformState,
        ObjectArray::View vaFilters)
    : super(hMap, nId, vKey, vValueOld, vValueNew, fSynthetic, nTransformState),
      f_vaFilters(self(), vaFilters), f_vEvent(self(), NULL)
    {
    COH_ENSURE_PARAM(vaFilters);
    }

MapListenerSupport::FilterEvent::FilterEvent(ObservableMap::Handle hMap,
        int32_t nId, Object::View vKey, Object::View vValueOld,
        Object::View vValueNew, bool fSynthetic, CacheEvent::TransformationState nTransformState,
        bool fPriming, ObjectArray::View vaFilters)
    : super(hMap, nId, vKey, vValueOld, vValueNew, fSynthetic, nTransformState, fPriming),
      f_vaFilters(self(), vaFilters), f_vEvent(self(), NULL)
    {
    COH_ENSURE_PARAM(vaFilters);
    }

 // ----- FilterEvent interface ------------------------------------------

ObjectArray::View MapListenerSupport::FilterEvent::getFilter() const
    {
    return f_vaFilters;
    }

void MapListenerSupport::FilterEvent::setEvent(MapEvent::View vEvent)
   {
   initialize(f_vEvent, vEvent);
   }

// ----- Describable interface ------------------------------------------

String::View MapListenerSupport::FilterEvent::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << ", filters=" << f_vaFilters);
    }

// ----- MapEvent methods -----------------------------------------------

Object::View MapListenerSupport::FilterEvent::getKey() const
    {
    return f_vEvent == NULL ? super::getKey() : f_vEvent->getKey();
    }

Object::View MapListenerSupport::FilterEvent::getOldValue() const
    {
    return f_vEvent == NULL ? super::getOldValue() : f_vEvent->getOldValue();
    }

Object::View MapListenerSupport::FilterEvent::getNewValue() const
    {
    return f_vEvent == NULL ? super::getNewValue() : f_vEvent->getNewValue();
    }

// ----- helper methods -------------------------------------------------

bool MapListenerSupport::FilterEvent::containsFilter(
        Filter::View vFilter) const
    {
    ObjectArray::View vaFilters = getFilter();
    for (size32_t i = 0, len = vaFilters->length; i < len; ++i)
        {
        if (NULL == vFilter ? NULL == vaFilters[i] :
                vFilter->equals(vaFilters[i]))
            {
            return true;
            }
        }
        return false;
    }


// ----- inner class: WrapperSynchronousListener ----------------------------

// ----- constructors ---------------------------------------------------

MapListenerSupport::WrapperSynchronousListener::WrapperSynchronousListener(
        MapListener::Handle hListener)
        : m_hListener(self(), hListener)
    {
    COH_ENSURE_PARAM(hListener);
    }

// ----- accessors -----------------------------------------------------

MapListener::View MapListenerSupport::WrapperSynchronousListener::getMapListener() const
    {
    return m_hListener;
    }

// ----- MultiplexingMapListener Interface ------------------------------

void MapListenerSupport::WrapperSynchronousListener::onMapEvent(
        MapEvent::View vEvent)
    {
    vEvent->dispatch((MapListener::Handle) m_hListener);
    }

// ----- Object interface -----------------------------------------------

size32_t MapListenerSupport::WrapperSynchronousListener::hashCode() const
    {
    return m_hListener->hashCode();
    }

bool MapListenerSupport::WrapperSynchronousListener::equals(
        Object::View v) const
    {
    if (NULL != v && Class::getClassName(this)->equals(Class::getClassName(v)))
        {
        WrapperSynchronousListener::View vThat =
            cast<WrapperSynchronousListener::View>(v);
        return m_hListener->equals(vThat->m_hListener);
        }
    return false;
    }

// ----- inner class: PrimingListener -----------------------------------

// ----- constructors ---------------------------------------------------

MapListenerSupport::WrapperPrimingListener::WrapperPrimingListener(
        MapListener::Handle hListener)
        : super(hListener)
    {
    }


COH_CLOSE_NAMESPACE2

