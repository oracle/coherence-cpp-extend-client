/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/cache/AbstractEntryBundler.hpp"

#include <algorithm>

COH_OPEN_NAMESPACE3(coherence,net,cache)

AbstractEntryBundler::AbstractEntryBundler()
    {
    }

void AbstractEntryBundler::process(Object::View vKey, Object::Holder ohValue)
    {
    int64_t cThreads = f_hCountThreads->increment();

        // Finally block
        {
        struct ProcessFinally
            {
            ProcessFinally (AbstractEntryBundler::Handle h)
                : hThis(h)
                {
                }
            ~ProcessFinally()
                {
                hThis->f_hCountThreads->decrement();
                }

            AbstractEntryBundler::Handle hThis;
            } finally(this);

        if (cThreads < getThreadThreshold())
            {
            bundle(Collections::singletonMap(vKey, ohValue));
            return;
            }

        Bundle::Handle  hBundle;
        bool            fBurst;
        while (true)
            {
            hBundle = cast<Bundle::Handle>(getOpenBundle());
            COH_SYNCHRONIZED(hBundle)
                {
                if (hBundle->isOpen())
                    {
                    bool fFirst = hBundle->add(vKey, ohValue);

                    fBurst = hBundle->waitForResults(fFirst);
                    break;
                    }
                }
            }
        hBundle->process(fBurst, vKey, ohValue);
        }
    }

void AbstractEntryBundler::processAll(Map::View vMap)
    {
    AtomicCounter::Handle hCounter = f_hCountThreads;
    int64_t               cThreads = hCounter->increment();

        // Finally block
        {
        struct ProcessAllFinally
            {
            ProcessAllFinally (AbstractEntryBundler::Handle h)
                : hThis(h)
                {
                }
            ~ProcessAllFinally()
                {
                hThis->f_hCountThreads->decrement();
                }

            AbstractEntryBundler::Handle hThis;
            } finally(this);

        if (cThreads < getThreadThreshold())
            {
            bundle(vMap);
            return;
            }

        Bundle::Handle  hBundle;
        bool            fBurst;
        while (true)
            {
            hBundle = cast<Bundle::Handle>(getOpenBundle());
            COH_SYNCHRONIZED (hBundle)
                {
                if (hBundle->isOpen())
                    {
                    bool fFirst = hBundle->addAll(vMap);

                    fBurst = hBundle->waitForResults(fFirst);
                    break;
                    }
                }
            }
        hBundle->processAll(fBurst, vMap);
        }
    }

// ----- subclassing support ------------------------------------------------

AbstractBundler::Bundle::Handle AbstractEntryBundler::instantiateBundle()
    {
    Bundle::Handle hBundle = Bundle::create(this);
    return hBundle;
    }

// ----- inner classes ------------------------------------------------------

AbstractEntryBundler::Bundle::Bundle(AbstractBundler::Handle hBundler)
    : super(hBundler), f_hMapEntries(self(), SafeHashMap::create())
    {
    }

// ----- bundling support ---------------------------------------------------

bool AbstractEntryBundler::Bundle::add(Object::View vKey, Object::Holder ohValue)
    {
    bool fFirst = f_hMapEntries->isEmpty();
    f_hMapEntries->put(vKey, ohValue);
    return fFirst;
    }

bool AbstractEntryBundler::Bundle::addAll(Map::View vMap)
    {
    bool fFirst = f_hMapEntries->isEmpty();
    f_hMapEntries->putAll(vMap);
    return fFirst;
    }

void AbstractEntryBundler::Bundle::process(bool fBurst, Object::View vKey,
    Object::Holder ohValue)
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

    if (!ensureResults(fBurst))
        {
        cast<AbstractEntryBundler::Handle>(getBundler())->bundle(Collections::singletonMap(vKey, ohValue));
        }
    }

void AbstractEntryBundler::Bundle::processAll(bool fBurst, Map::View vMap)
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

    if (!ensureResults(fBurst))
        {
        cast<AbstractEntryBundler::Handle>(getBundler())->bundle(vMap);
        }
    }

int32_t AbstractEntryBundler::Bundle::getBundleSize() const
    {
    return std::max(super::getBundleSize(), (int32_t) f_hMapEntries->size());
    }

void AbstractEntryBundler::Bundle::ensureResults()
    {
    cast<AbstractEntryBundler::Handle>(getBundler())->bundle(f_hMapEntries);
    }

bool AbstractEntryBundler::Bundle::releaseThread()
    {
    COH_SYNCHRONIZED(this)
        {
        bool fRelease = super::releaseThread();
        if (fRelease)
            {
            // TODO: unfortunately, clear() will drop the underlying bucket
            // array, which may cause unnecesary resizing later on...
            // ideally, we would want to preserve the bucket count, but
            // clear the content; consider adding SafeHashMap.clearContent()
            f_hMapEntries->clear();
            }
        return fRelease;
        }
    }

COH_CLOSE_NAMESPACE3
