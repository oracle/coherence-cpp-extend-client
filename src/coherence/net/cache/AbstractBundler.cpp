/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/cache/AbstractBundler.hpp"

#include "coherence/util/Random.hpp"

#include <algorithm>
#include <cmath>

COH_OPEN_NAMESPACE3(coherence,net,cache)

using namespace std;

using coherence::util::Random;

AbstractBundler::AbstractBundler()
    : m_fFirstAdjustment(true),
      m_dPreviousSizeThreshold(0.0),
      f_hListBundle(self(), ArrayList::create()),
      f_hCountThreads(self(), AtomicCounter::create()),
      m_dSizeThreshold(0.0),
      m_cThreadThreshold(0),
      m_fAllowAuto(true),
      m_lDelayMillis(1L),
      m_iActiveBundle(self()),
      f_hStats(self(), AbstractBundler::Statistics::create())
    {
    }

void AbstractBundler::init(Bundle::Handle hBundle)
    {
    hBundle->setMaster();
    f_hListBundle->add(hBundle);
    }

// ----- property accessors -------------------------------------------------

int32_t AbstractBundler::getSizeThreshold() const
    {
    return (int32_t) m_dSizeThreshold;
    }

void AbstractBundler::setSizeThreshold(int32_t cSize)
    {
    if (cSize <= 0)
        {
        COH_THROW(IllegalArgumentException::create("Negative bundle size threshold"));
        }
    m_dSizeThreshold = cSize;

    // reset the previous value used for auto adjustment
    m_dPreviousSizeThreshold = 0.0;
    m_fFirstAdjustment = true;
    }

int32_t AbstractBundler::getThreadThreshold() const
    {
    return m_cThreadThreshold;
    }

void AbstractBundler::setThreadThreshold(int32_t cThreads)
    {
    if (cThreads <= 0)
        {
        COH_THROW(IllegalArgumentException::create("Invalid thread threshold"));
        }
    m_cThreadThreshold = cThreads;
    }

int64_t AbstractBundler::getDelayMillis() const
    {
    return m_lDelayMillis;
    }

void AbstractBundler::setDelayMillis(int64_t lDelay)
    {
    if (lDelay <= 0)
        {
        COH_THROW(IllegalArgumentException::create("Invalid delay value"));
        }
    m_lDelayMillis = lDelay;
    }

bool AbstractBundler::isAllowAutoAdjust() const
    {
    return m_fAllowAuto;
    }

void AbstractBundler::setAllowAutoAdjust(bool fAutoAdjust)
    {
    m_fAllowAuto = fAutoAdjust;
    }

// ----- statistics ---------------------------------------------------------

void AbstractBundler::updateStatistics()
    {
    while (true)
        {
        try
            {
            int64_t cTotalBundles = 0L;
            int64_t cTotalSize    = 0L;
            int64_t cTotalBurst   = 0L;
            int64_t cTotalWait    = 0L;

            for (int32_t i = 0, c = f_hListBundle->size(); i < c; i++)
                {
                Bundle::Handle hBundle = cast<Bundle::Handle>(f_hListBundle->get(i));

                cTotalBundles += hBundle->m_cTotalBundles;
                cTotalSize    += hBundle->m_cTotalSize;
                cTotalBurst   += hBundle->m_cTotalBurstDuration;
                cTotalWait    += hBundle->m_cTotalWaitDuration;
                }

            int64_t cDeltaBundles = cTotalBundles - f_hStats->m_cBundleCountSnapshot;
            int64_t cDeltaSize    = cTotalSize    - f_hStats->m_cBundleSizeSnapshot;
            int64_t cDeltaBurst   = cTotalBurst   - f_hStats->m_cBurstDurationSnapshot;
            int64_t cDeltaWait    = cTotalWait    - f_hStats->m_cThreadWaitSnapshot;

            // log("DeltaBundles=" + cDeltaBundles + ", DeltaSize=" + cDeltaSize
            // + ", DeltaBurst=" + cDeltaBurst + ", DeltaWait=" + cDeltaWait);

            if (cDeltaBundles > 0 && cDeltaWait > 0)
                {
                f_hStats->m_cAverageBundleSize = (int32_t) floor(0.5 +
                        (double) cDeltaSize / (double) cDeltaBundles);
                f_hStats->m_cAverageBurstDuration = (int32_t) floor(0.5 +
                        (double) cDeltaBurst / (double) cDeltaBundles);
                f_hStats->m_cAverageThreadWaitDuration = (int32_t) floor(0.5 +
                        (double) cDeltaWait / (double) cDeltaBundles);
                f_hStats->m_nAverageThroughput = (int32_t) floor(0.5 +
                        (double) cDeltaSize*1000 / (double) cDeltaWait);
                }

            f_hStats->m_cBundleCountSnapshot   = cTotalBundles;
            f_hStats->m_cBundleSizeSnapshot    = cTotalSize;
            f_hStats->m_cBurstDurationSnapshot = cTotalBurst;
            f_hStats->m_cThreadWaitSnapshot    = cTotalWait;

            return;
            }
        catch (IndexOutOfBoundsException::View e)
            {
            // there is theoretical possibility that the Java Memory Model
            // causes the list size to be not in sync with the actual list
            // storage; try again
            }
        }
    }

void AbstractBundler::resetStatistics()
    {
    while (true)
        {
        try
            {
            for (int32_t i = 0, c = f_hListBundle->size(); i < c; i++)
                {
                Bundle::Handle hBundle = cast<Bundle::Handle>(f_hListBundle->get(i));

                hBundle->resetStatistics();
                }
            break;
            }
        catch (IndexOutOfBoundsException::View e)
            {
            // there is theoretical possibility that certain memory model causes
            // the list size to be not in sync with the actual list storage;
            // try again
            }
        }
    f_hStats->reset();
    m_dPreviousSizeThreshold = 0.0;
    m_fFirstAdjustment = true;
    }

void AbstractBundler::adjust()
    {
    double dSizePrev = m_dPreviousSizeThreshold;
    double dSizeCurr = m_dSizeThreshold;

    int32_t nThruPrev = f_hStats->m_nAverageThroughput;
    updateStatistics();
    int32_t nThruCurr = f_hStats->m_nAverageThroughput;

    if (isAllowAutoAdjust())
        {
        double dDelta = 0.0;

        if (m_fFirstAdjustment)
            {
            // the very first adjustment after reset
            dDelta = std::max(1.0, 0.1 * dSizeCurr);
            }
        else if (std::abs(nThruCurr - nThruPrev) <=
                    std::max(1, (nThruCurr + nThruPrev) / 100))
            {
            // not more than 2% throughput change;
            // with a probability of 10% lets nudge the size up to 5%
            // in a random direction
            int32_t nRandom = Random::getInstance()->nextInt32(100);
            if (nRandom < 10 || fabs(dSizePrev - dSizeCurr) < 0.001)
                {
                dDelta = std::max(1.0, 0.05 * dSizeCurr);
                if (nRandom < 5)
                    {
                    dDelta = -dDelta;
                    }
                }
            }
        else if (nThruCurr > nThruPrev)
            {
            // the throughput has improved; keep moving the size threshold
            // in the same direction at the same rate
            dDelta = (dSizeCurr - dSizePrev);
            }
        else
            {
            // the throughput has dropped; reverse the direction with half
            // of the previous rate
            dDelta = (dSizePrev - dSizeCurr) / 2;
            }

        if (dDelta > 0.000001 || dDelta < -0.000001)  // dDelta != 0.0
            {
            double dSizeNew = dSizeCurr + dDelta;
            if (dSizeNew > 1.0)
                {
                // out("Adjusting size by: " +
                //     (float) dDelta + " to " + (float) dSizeNew);
                m_dPreviousSizeThreshold = dSizeCurr;
                m_dSizeThreshold         = dSizeNew;
                }
            }
        }
    }

TypedHandle<const String> AbstractBundler::toString() const
    {
    return COH_TO_STRING(Class::getClassName(this)
         << "{SizeThreshold="    << getSizeThreshold()
         << ", ThreadThreshold=" << getThreadThreshold()
         << ", DelayMillis="     << getDelayMillis()
         << ", AutoAdjust="      << (isAllowAutoAdjust() ? "on" : "off")
         << ", ActiveBundles="   << f_hListBundle->size()
         << ", Statistics="      << f_hStats
         << "}");
    }

// ----- sublcassing support ------------------------------------------------

AbstractBundler::Bundle::Handle AbstractBundler::getOpenBundle()
    {
    List::Handle hListBundle   = f_hListBundle;
    int32_t      cBundles      = hListBundle->size();
    int32_t      iActiveBundle = m_iActiveBundle;
    try
        {
        for (int32_t i = 0; i < cBundles; i++)
            {
            int32_t        iBundle = (iActiveBundle + i) % cBundles;
            Bundle::Handle hBundle = cast<Bundle::Handle>(hListBundle->get(iBundle));

            if (hBundle->isOpen())
                {
                m_iActiveBundle = iBundle;
                return hBundle;
                }
            }
        }
    catch (IndexOutOfBoundsException::View e)
        {
        // there is theoretical possibility that the memory model causes
        // the list size to be not in sync with the actual list storage;
        // proceed with synchronization...
        }
    catch (NullPointerException::View e)
        {
        // ditto
        }

    // we may need to create a new Bundle; synchronize to prevent the
    // creation of unnecessary bundles
    COH_SYNCHRONIZED (hListBundle)
        {
        // double check under synchronization
        cBundles = hListBundle->size();
        for (int32_t i = 0; i < cBundles; i++)
            {
            int32_t        iBundle = (iActiveBundle + i) % cBundles;
            Bundle::Handle hBundle = cast<Bundle::Handle>(hListBundle->get(iBundle));

            if (hBundle->isOpen())
                {
                m_iActiveBundle = iBundle;
                return hBundle;
                }
            }

        // nothing available; add a new one
        Bundle::Handle hBundle = instantiateBundle();
        hListBundle->add(hBundle);
        m_iActiveBundle = cBundles;
        return hBundle;
        }
    }

// ----- inner class Bundle -------------------------------------------------

AbstractBundler::Bundle::Bundle(AbstractBundler::Handle hBundler)
    : f_hBundler(self(), hBundler),
      m_iStatus(self(), 0),    //    m_iStatus = status_open;
      m_cThreads(0),
      m_fMaster(false),
      m_cTotalBundles(self()),
      m_cTotalSize(self()),
      m_ldtStart(0L),
      m_cTotalBurstDuration(self()),
      m_cTotalWaitDuration(self())
    {
    }

// ----- accessors ----------------------------------------------------------

bool AbstractBundler::Bundle::isOpen() const
    {
    return m_iStatus == status_open;
    }

bool AbstractBundler::Bundle::isPending() const
    {
    return m_iStatus == status_pending;
    }

bool AbstractBundler::Bundle::isProcessed() const
    {
    return m_iStatus == status_processed ||
           m_iStatus == status_exception;
    }

bool AbstractBundler::Bundle::isException() const
    {
    return m_iStatus == status_exception;
    }

void AbstractBundler::Bundle::setStatus(int32_t iStatus)
    {
    COH_SYNCHRONIZED(this)
        {
        bool fValid;
        switch (m_iStatus)
            {
            case status_open:
                fValid = iStatus == status_pending
                      || iStatus == status_exception;
                break;

            case status_pending:
                fValid = iStatus == status_processed
                      || iStatus == status_exception;
                break;

            case status_processed:
            case status_exception:
                fValid = iStatus == status_open;
                break;
            default:
                fValid = false;
            }

        if (!fValid)
            {
            COH_THROW_STREAM(IllegalStateException, *this <<
                "; invalid transition to " << formatStatusName(iStatus));
            }

        m_iStatus = iStatus;

        if (iStatus == status_processed
         || iStatus == status_exception)
            {
            int64_t cDuration = (int64_t) System::currentTimeMillis() - m_ldtStart;
            m_cTotalWaitDuration = m_cTotalWaitDuration + std::max((int64_t) 0L, cDuration);
            notifyAll();
            }
        }
    }

int32_t AbstractBundler::Bundle::getBundleSize() const
    {
    return m_cThreads;
    }

bool AbstractBundler::Bundle::isMaster() const
    {
    return m_fMaster;
    }

void AbstractBundler::Bundle::setMaster()
    {
    m_fMaster = true;
    }

AbstractBundler::Handle AbstractBundler::Bundle::getBundler()
    {
    return f_hBundler;
    }

bool AbstractBundler::Bundle::waitForResults(bool fFirst)
    {
    m_cThreads++;
    try
        {
        if (fFirst)
            {
            m_ldtStart = System::currentTimeMillis();
            }

        AbstractBundler::Handle hBundler = getBundler();
        if (getBundleSize() < hBundler->getSizeThreshold())
            {
            if (fFirst)
                {
                int64_t lDelay = hBundler->getDelayMillis();
                do
                    {
                    wait(lDelay);

                    // if someone has already "submitted" the bundle
                    // need to keep waiting
                    lDelay = 0L;
                    }
                while (isPending());
                }
            else
                {
                while (true)
                    {
                    wait();

                    if (isProcessed())
                        {
                        return false;
                        }
                    // spurious wake-up; continue waiting
                    }
                }
            }

        if (isProcessed())
            {
            return false;
            }

        // this bundle should be closed and processed right away
        setStatus(status_pending);

        // update stats
        m_cTotalSize = m_cTotalSize + (int64_t) getBundleSize();
        int64_t cTotal = m_cTotalBundles + 1;
        if (cTotal > 1000       // allow the "hotspot" to kick in
            && cTotal % adjustment_frequency == 0 && isMaster())
            {
            // attempt to adjust for every 1000 iterations of the master
            // bundle
            hBundler->adjust();
            }
        }
    catch (InterruptedException::View e)
        {
        Thread::currentThread()->interrupt();
        setStatus(status_exception);
        }
    catch (RuntimeException::View e)
        {
        // should never happen
        --m_cThreads;
        COH_THROW(e);
        }
    catch (Exception::View e)
        {
        // should never happen
        --m_cThreads;
        COH_THROW(e);
        }
    return true;
    }

bool AbstractBundler::Bundle::ensureResults(bool fBurst)
    {
    if (isException())
        {
        return false;
        }

    if (fBurst)
        {
        // bundle is closed and ready for the actual execution (burst);
        // it must be performed without holding any synchronization
        try
            {
            int64_t ldtStart = System::currentTimeMillis();

            ensureResults();

            int64_t cElapsedMillis = System::currentTimeMillis() - ldtStart;
            if (cElapsedMillis > 0)
                {
                m_cTotalBurstDuration = m_cTotalBurstDuration + cElapsedMillis;
                }

            setStatus(status_processed);
            }
        catch (Exception::View vEx)
            {
            setStatus(status_exception);
            return false;
            }
        }
    else
        {
        COH_ENSURE(isProcessed());
        }
    return true;
    }

bool AbstractBundler::Bundle::releaseThread()
    {
    COH_SYNCHRONIZED(this)
        {
        COH_ENSURE(isProcessed() && m_cThreads > 0);

        if (--m_cThreads == 0)
            {
            setStatus(status_open);
            return true;
            }

        return false;
        }
    }

// ----- statistics and debugging  ------------------------------------------

void AbstractBundler::Bundle::resetStatistics()
    {
    m_cTotalBundles       = 0L;
    m_cTotalSize          = 0L;
    m_cTotalBurstDuration = 0L;
    m_cTotalWaitDuration  = 0L;
    }

TypedHandle<const String> AbstractBundler::Bundle::toString() const
    {
    return COH_TO_STRING("Bundle@" << hashCode() << "{" << formatStatusName(m_iStatus)
        << ", size=" << getBundleSize() << '}');
    }

String::View AbstractBundler::Bundle::formatStatusName(int32_t iStatus) const
    {
    switch (iStatus)
        {
        case status_open:
            return "status_open";
        case status_pending:
            return "status_pending";
        case status_processed:
            return "status_processed";
        case status_exception:
            return "status_exception";
        default:
            return "unknown";
        }
    }

void AbstractBundler::Statistics::reset()
    {
    m_cBundleCountSnapshot   = 0L;
    m_cBundleSizeSnapshot    = 0L;
    m_cBurstDurationSnapshot = 0L;
    m_cThreadWaitSnapshot    = 0L;
    }

TypedHandle<const String> AbstractBundler::Statistics::toString() const
    {
    return COH_TO_STRING("(AverageBundleSize="     << m_cAverageBundleSize
        << ", AverageBurstDuration=" << m_cAverageBurstDuration      << "ms"
        << ", AverageWaitDuration="  << m_cAverageThreadWaitDuration << "ms"
        << ", AverageThroughput="    << m_nAverageThroughput         << "/sec"
        << ")");
    }

COH_CLOSE_NAMESPACE3
