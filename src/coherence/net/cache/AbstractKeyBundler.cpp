/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/cache/AbstractKeyBundler.hpp"

#include "coherence/util/AtomicCounter.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/SafeHashSet.hpp"
#include "coherence/util/Iterator.hpp"

#include <algorithm>

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::util::AtomicCounter;
using coherence::util::HashMap;
using coherence::util::SafeHashSet;
using coherence::util::Iterator;

AbstractKeyBundler::AbstractKeyBundler()
    {
    }

Object::Holder AbstractKeyBundler::process(Object::View vKey)
    {
    int64_t cThreads = f_hCountThreads->increment();

        // Finally block
        {
        struct ProcessFinally
            {
            ProcessFinally (Handle h)
                : hThis(h)
                {
                }
            ~ProcessFinally()
                {
                hThis->f_hCountThreads->decrement();
                }

            Handle hThis;
            } finally(this);

        if (cThreads < getThreadThreshold())
            {
            return unbundle(vKey);
            }

        Bundle::Handle  hBundle;
        bool            fBurst;
        while (true)
            {
            hBundle = cast<AbstractKeyBundler::Bundle::Handle>(getOpenBundle());
            COH_SYNCHRONIZED (hBundle)
                {
                if (hBundle->isOpen())
                    {
                    bool fFirst = hBundle->add(vKey);

                    fBurst = hBundle->waitForResults(fFirst);
                    break;
                    }
                }
            }
        return hBundle->process(fBurst, vKey);
        }
    }

Map::View AbstractKeyBundler::processAll(Collection::View vColKeys)
    {
    int64_t cThreads = f_hCountThreads->increment();

        // Finally block
        {
        struct ProcessAllFinally
            {
            ProcessAllFinally (Handle h)
                : hThis(h)
                {
                }
            ~ProcessAllFinally()
                {
                hThis->f_hCountThreads->decrement();
                }

            Handle hThis;
            } finally(this);

        if (cThreads < getThreadThreshold())
            {
            return bundle(vColKeys);
            }

        Bundle::Handle hBundle;
        bool           fBurst;
        while (true)
            {
            hBundle = cast<Bundle::Handle>(getOpenBundle());
            COH_SYNCHRONIZED(hBundle)
                {
                if (hBundle->isOpen())
                    {
                    bool fFirst = hBundle->addAll(vColKeys);

                    fBurst = hBundle->waitForResults(fFirst);
                    break;
                    }
                }
            }
        return hBundle->processAll(fBurst, vColKeys);
        }
    }

AbstractBundler::Bundle::Handle AbstractKeyBundler::instantiateBundle()
    {
    Bundle::Handle hBundle = Bundle::create(this);
    return hBundle;
    }


// ----- inner classes ------------------------------------------------------

AbstractKeyBundler::Bundle::Bundle(AbstractBundler::Handle hBundler)
    : super(hBundler), f_hSetKeys(self(), SafeHashSet::create()),
      m_vMapResults(self())
    {
    }

// ----- bundling support -----------------------------------------------

bool AbstractKeyBundler::Bundle::add(Object::Holder ohKey)
    {
    bool fFirst = f_hSetKeys->isEmpty();
    f_hSetKeys->add(ohKey);
    return fFirst;
    }

bool AbstractKeyBundler::Bundle::addAll(Collection::View vColKeys)
    {
    bool fFirst = f_hSetKeys->isEmpty();
    f_hSetKeys->addAll(vColKeys);
    return fFirst;
    }

Object::Holder AbstractKeyBundler::Bundle::process(bool fBurst, Object::Holder ohKey)
    {
    struct ProcessFinally
        {
        ProcessFinally (Handle h)
            : hThis(h)
            {
            }
        ~ProcessFinally()
            {
            hThis->releaseThread();
            }

        Handle hThis;
        } finally(this);

    return ensureResults(fBurst)
        ? m_vMapResults->get(ohKey) : cast<AbstractKeyBundler::Handle>(getBundler())->unbundle(ohKey);
    }

Map::View AbstractKeyBundler::Bundle::processAll(bool fBurst, Collection::View vColKeys)
    {
    struct ProcessAllFinally
        {
        ProcessAllFinally (Handle h)
            : hThis(h)
            {
            }
        ~ProcessAllFinally()
            {
            hThis->releaseThread();
            }

        Handle hThis;
        } finally(this);

    if (ensureResults(fBurst))
        {
        Map::View   vMapResults = m_vMapResults;
        Map::Handle hMap        = HashMap::create(vColKeys->size());
        for (Iterator::Handle hIter = vColKeys->iterator(); hIter->hasNext(); )
            {
            Object::View vKey = hIter->next();
            Object::View vVal = vMapResults->get(vKey);
            if (vVal != NULL)
                {
                hMap->put(vKey, vVal);
                }
            }
        return hMap;
        }
    else
        {
        return cast<AbstractKeyBundler::Handle>(getBundler())->bundle(vColKeys);
        }
    }

int32_t AbstractKeyBundler::Bundle::getBundleSize() const
    {
    return std::max(super::getBundleSize(), (int32_t) f_hSetKeys->size());
    }

void AbstractKeyBundler::Bundle::ensureResults()
    {
    m_vMapResults = cast<AbstractKeyBundler::Handle>(getBundler())->bundle(f_hSetKeys);
    }

bool AbstractKeyBundler::Bundle::releaseThread()
    {
    COH_SYNCHRONIZED(this)
        {
        bool fRelease = super::releaseThread();
        if (fRelease)
            {
            // TODO: unfortunately, clear() will drop the underlying bucket
            // array, which may cause unnecessary resizing later on...
            // ideally, we would want to preserve the bucket count, but
            // clear the content; consider adding SafeHashSet::clearContent()
            f_hSetKeys->clear();
            m_vMapResults = NULL;
            }
        return fRelease;
        }
    }

COH_CLOSE_NAMESPACE3
