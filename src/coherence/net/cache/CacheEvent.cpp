/*
 * Copyright (c) 2000, 2022, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/cache/CacheEvent.hpp"

#include "coherence/util/MapListenerSupport.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::util::MapListenerSupport;


// ----- constructors -------------------------------------------------------

CacheEvent::CacheEvent(ObservableMap::Handle hMap, int32_t nId,
        Object::View voKey, Object::View voValueOld, Object::View voValueNew,
        bool fSynthetic)
        : super(hMap, nId, voKey, voValueOld, voValueNew),
          m_fSynthetic(fSynthetic), m_nTransformState(CacheEvent::transformable),
          m_fPriming(false)
    {
    }

CacheEvent::CacheEvent(ObservableMap::Handle hMap, int32_t nId,
        Object::View voKey, Object::View voValueOld, Object::View voValueNew,
        bool fSynthetic, bool fPriming)
        : super(hMap, nId, voKey, voValueOld, voValueNew),
          m_fSynthetic(fSynthetic), m_nTransformState(CacheEvent::transformable),
          m_fPriming(fPriming)
    {
    }

CacheEvent::CacheEvent(ObservableMap::Handle hMap, int32_t nId,
        Object::View voKey, Object::View voValueOld, Object::View voValueNew,
        bool fSynthetic, TransformationState nTransformState)
        : super(hMap, nId, voKey, voValueOld, voValueNew),
          m_fSynthetic(fSynthetic), m_nTransformState(nTransformState),
          m_fPriming(false)
    {
    }

CacheEvent::CacheEvent(ObservableMap::Handle hMap, int32_t nId,
        Object::View voKey, Object::View voValueOld, Object::View voValueNew,
        bool fSynthetic, TransformationState nTransformState, bool fPriming)
        : super(hMap, nId, voKey, voValueOld, voValueNew),
          m_fSynthetic(fSynthetic), m_nTransformState(nTransformState),
          m_fPriming(fPriming)
    {
    }

CacheEvent::CacheEvent(ObservableMap::Handle hMap, int32_t nId,
        Object::View voKey, Object::View voValueOld, Object::View voValueNew,
        bool fSynthetic, TransformationState nTransformState, bool fPriming,
        bool fExpired)
        : super(hMap, nId, voKey, voValueOld, voValueNew),
          m_fSynthetic(fSynthetic), m_nTransformState(nTransformState),
          m_fPriming(fPriming), m_fExpired(fExpired)
    {
    }

// ----- CacheEvent interface -----------------------------------------------

bool CacheEvent::isSynthetic() const
    {
    return m_fSynthetic;
    }

CacheEvent::TransformationState CacheEvent::getTransformState() const
    {
    return m_nTransformState;
    }

bool CacheEvent::isPriming() const
    {
    return m_fPriming;
    }

bool CacheEvent::isExpired() const
    {
    return m_fExpired;
    }

bool CacheEvent::shouldDispatch(TypedHandle<const MapListener> vListener) const
    {
    return (!isPriming() || MapListenerSupport::isPrimingListener(vListener));
    }

// ----- Describable interface ----------------------------------------------

String::View CacheEvent::getDescription() const
    {
    String::View vs = super::getDescription();
    if (isSynthetic())
        {
        vs = COH_TO_STRING(vs << ", synthetic");
        }

    if (isPriming())
        {
        vs = COH_TO_STRING(vs << ", priming");
        }
    if (isExpired())
        {
        vs = COH_TO_STRING(vs << ", expired");
        }

    return vs;
    }

COH_CLOSE_NAMESPACE3

