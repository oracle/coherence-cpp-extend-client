/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/LimitFilter.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/Arrays.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/filter/ExtractorFilter.hpp"

#include "private/coherence/util/InvocableMapHelper.hpp"
#include "private/coherence/util/SimpleIterator.hpp"
#include "private/coherence/util/SimpleMapEntry.hpp"

#include <algorithm>


COH_OPEN_NAMESPACE3(coherence,util,filter)

COH_REGISTER_PORTABLE_CLASS(78, LimitFilter);


// ----- constructors -------------------------------------------------------

LimitFilter::LimitFilter()
    : f_vFilter(self()), m_cPageSize(0), m_nPage(0),
      f_vComparator(self(), NULL, /* mutable */ true), 
      m_vAnchorTop(self(), NULL, /* mutable */ true),
      m_vAnchorBottom(self(), NULL, /* mutable */ true), 
      m_vCookie(self(), NULL, /* mutable */ true)
    {
    }

LimitFilter::LimitFilter(Filter::View vFilter, int32_t cPageSize)
    : f_vFilter(self(),vFilter), m_cPageSize(cPageSize), m_nPage(0),
      f_vComparator(self(), NULL, /* mutable */ true), 
      m_vAnchorTop(self(), NULL, /* mutable */ true),
      m_vAnchorBottom(self(), NULL, /* mutable */ true), 
      m_vCookie(self(), NULL, /* mutable */ true)
    {
    if (vFilter == NULL)
        {
        COH_THROW (IllegalArgumentException::create("Filter must be specified"));
        }

    if (instanceof<LimitFilter::View>(vFilter))
        {
        COH_THROW (UnsupportedOperationException::create("Limit of limit"));
        }

    if (cPageSize <= 0)
        {
        COH_THROW (IllegalArgumentException::create("Invalid page size"));
        }
    }

LimitFilter::LimitFilter(const LimitFilter& that)
    : super(that),
      f_vFilter(self(), that.f_vFilter),
      m_cPageSize(that.m_cPageSize),
      m_nPage(that.m_nPage),
      f_vComparator(self(), that.f_vComparator, /* mutable */ true),
      m_vAnchorTop(self(), that.m_vAnchorTop, /* mutable */ true),
      m_vAnchorBottom(self(), that.m_vAnchorBottom, /* mutable */ true),
      m_vCookie(self(), that.m_vCookie, /* mutable */ true)
    {
    }


// ----- EntryFilter interface ----------------------------------------------

bool LimitFilter::evaluateEntry(Map::Entry::View vEntry) const
   {
   return InvocableMapHelper::evaluateEntry(f_vFilter, vEntry);
   }


// ----- Filter interface ---------------------------------------------------

bool LimitFilter::evaluate(Object::View v) const
   {
   return f_vFilter->evaluate(v);
   }


// ----- IndexAwareFilter interface -----------------------------------------

size32_t LimitFilter::calculateEffectiveness(Map::View vMapIndexes,
        Set::View vSetKeys) const
    {
    Filter::View vFilter = f_vFilter;
    return instanceof<IndexAwareFilter::View>(vFilter)
        ? cast<IndexAwareFilter::View>(vFilter)->calculateEffectiveness(vMapIndexes, vSetKeys)
        : vSetKeys->size()*ExtractorFilter::eval_cost;
    }

Filter::View LimitFilter::applyIndex(Map::View vMapIndexes,
        Set::Handle hSetKeys) const
    {
    Filter::View vFilter = f_vFilter;
    if (instanceof<IndexAwareFilter::View>(vFilter))
        {
        return cast<IndexAwareFilter::View>(vFilter)->applyIndex(vMapIndexes, hSetKeys);
        }
    else
        {
        return vFilter;
        }
    }

// ----- PortableObject interface -------------------------------------------

void LimitFilter::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vFilter, cast<Filter::View>(hIn->readObject(0)));
    m_cPageSize     = hIn->readInt32(1);
    m_nPage         = hIn->readInt32(2);
    initialize(f_vComparator, cast<Comparator::View>(hIn->readObject(3)));
    m_vAnchorTop    = hIn->readObject(4);
    m_vAnchorBottom = hIn->readObject(5);
   }

void LimitFilter::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, f_vFilter);
    hOut->writeInt32(1, m_cPageSize);
    hOut->writeInt32(2, m_nPage);
    hOut->writeObject(3, f_vComparator);
    hOut->writeObject(4, m_vAnchorTop);
    hOut->writeObject(5, m_vAnchorBottom);
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> LimitFilter::toString() const
    {
    String::View vs = COH_TO_STRING("LimitFilter: ("
        << f_vFilter
        << " [pageSize=" << m_cPageSize
        << ", pageNum=" << m_nPage);

    if (f_vComparator != NULL)
        {
        vs = COH_TO_STRING(vs << ", top=" << m_vAnchorTop
            << ", bottom=" << m_vAnchorBottom
            << ", comparator=" << f_vComparator);
        }

    vs = COH_TO_STRING(vs << "])");
    return vs;
    }

// ----- data members accessor-----------------------------------------------

Filter::View LimitFilter::getFilter() const
    {
    return f_vFilter;
    }

int32_t LimitFilter::getPageSize() const
    {
    return m_cPageSize;
    }

void LimitFilter::setPageSize(int32_t cPageSize)
    {
    if (cPageSize <= 0)
        {
        COH_THROW (IllegalArgumentException::create("Invalid page size"));
        }
    m_cPageSize = cPageSize;
    }

int32_t LimitFilter::getPage() const
    {
    return m_nPage;
    }

void LimitFilter::setPage(int32_t nPage)
    {
    if (nPage < 0)
        {
        COH_THROW (IllegalArgumentException::create("Negative page"));
        }

    if (nPage == 0) // "reset"
        {
        setTopAnchor   (NULL);
        setBottomAnchor(NULL);
        setCookie(NULL);
        }
    else
        {
        int nPageCurr = m_nPage;
        if (nPage == nPageCurr + 1)
            {
            setTopAnchor(getBottomAnchor());
            setBottomAnchor(NULL);
            }
        else if (nPage == nPageCurr - 1)
            {
            setBottomAnchor(getTopAnchor());
            setTopAnchor   (NULL);
            }
        else if (nPage != nPageCurr)
            {
            setTopAnchor   (NULL);
            setBottomAnchor(NULL);
            }
        }
    m_nPage = nPage;
    }

Comparator::View LimitFilter::getComparator() const
    {
    return f_vComparator;
    }

void LimitFilter::setComparator(Comparator::View vComparator) const
    {
    initialize(f_vComparator, vComparator);
    }

Object::View LimitFilter::getTopAnchor() const
    {
    return m_vAnchorTop;
    }

void LimitFilter::setTopAnchor(Object::View vAnchor) const
    {
    m_vAnchorTop = vAnchor;
    }

Object::View LimitFilter::getBottomAnchor() const
    {
    return m_vAnchorBottom;
    }

void LimitFilter::setBottomAnchor(Object::View vAnchor) const
    {
    m_vAnchorBottom = vAnchor;
    }

Object::View LimitFilter::getCookie() const
    {
    return m_vCookie;
    }

void LimitFilter::setCookie(Object::View vCookie) const
    {
    m_vCookie = vCookie;
    }


// ----- helpers ------------------------------------------------------------

void LimitFilter::nextPage()
    {
    setPage(getPage() + 1);
    }

void LimitFilter::previousPage()
    {
    setPage(getPage() - 1);
    }

ObjectArray::Handle LimitFilter::extractPage(ObjectArray::Handle haEntry) const
    {
    int32_t            cEntries    = haEntry->length;
    int32_t            cPageSize   = getPageSize();
    Comparator::View   vComparator = getComparator();

    // no reason to optimize for a small result set
    if (NULL != vComparator && cEntries > cPageSize)
        {
        Object::View voAnchorTop    = getTopAnchor();
        Object::View voAnchorBottom = getBottomAnchor();

        if (NULL != voAnchorTop)
            {
            // if both AnchorTop and AnchorBottom are present;
            // it's a repetitive request for the same page
            int32_t ofAnchor = Arrays::binarySearch(haEntry,
                SimpleMapEntry::create((Object::View) NULL, voAnchorTop),
                vComparator);
            int32_t nShift   = NULL == voAnchorBottom ? 1 : 0;
            int32_t ofFirst  = ofAnchor >= 0 ? ofAnchor + nShift : -ofAnchor - 1;

            if (ofFirst < cEntries)
                {
                return extractPage(cast<Iterator::Handle>(
                        SimpleIterator::create(haEntry, ofFirst,
                        std::min(cPageSize, cEntries - ofFirst))));
                }
            else
                {
                return ObjectArray::create(0);
                }
            }
        else if (NULL != voAnchorBottom)
            {
            int32_t ofAnchor    = Arrays::binarySearch(haEntry,
                    SimpleMapEntry::create((Object::View) NULL,
                            voAnchorBottom), vComparator);
            int32_t ofAfterLast = ofAnchor >= 0 ? ofAnchor : -ofAnchor - 1;

            if (ofAfterLast > 0)
                {
                int32_t ofFirst = std::max(0, ofAfterLast - cPageSize);
                return extractPage(cast<Iterator::Handle>(
                        SimpleIterator::create(haEntry, ofFirst,
                        std::min(cPageSize, ofAfterLast - ofFirst))));
                }
            else
                {
                return ObjectArray::create(0);
                }
            }
        }

    return extractPage(cast<Iterator::Handle>(SimpleIterator::create(haEntry)));
    }

ObjectArray::Handle LimitFilter::extractPage(Iterator::Handle hIter) const
    {
    int32_t              cPageSize      = getPageSize();
    Comparator::View     vComparator    = getComparator();
    Object::View         voAnchorTop    = getTopAnchor();
    Object::View         voAnchorBottom = getBottomAnchor();
    ObjectArray::Handle  haEntry        = ObjectArray::create(cPageSize);
    int32_t              iEntry         = 0;

    if (NULL == vComparator ||
            (NULL == voAnchorTop && NULL == voAnchorBottom))
        {
        int32_t cSkip = getPage()*cPageSize;

        // THIS IS A HACK: reconsider
        if (NULL == vComparator && instanceof<Integer32::View>(voAnchorTop))
            {
            cSkip = (cast<Integer32::View>(voAnchorTop))->getInt32Value();
            }

        while (hIter->hasNext())
            {
            Object::Holder ohEntry = hIter->next();

            if (--cSkip >= 0)
                {
                continue;
                }

            haEntry[iEntry] = ohEntry;

            if (++iEntry == cPageSize)
                {
                break;
                }
            }

        if (iEntry < cPageSize)
            {
            // last page is not full
            int32_t             cSize  = iEntry;
            ObjectArray::Handle hao    = ObjectArray::create(cSize);

            if (cSize > 0)
                {
                Arrays::copy(haEntry, 0, hao, 0, cSize);
                }
            haEntry = hao;
            }
        }
    else
        {
        bool fHeading    = NULL != voAnchorTop || NULL == voAnchorBottom;
        bool fInclusive  = NULL != voAnchorTop && NULL != voAnchorBottom;
        bool fSkip       = fHeading;
        bool fWrap       = false;

        Map::Entry::View vEntryTop    = SimpleMapEntry::create((Object::View) NULL, voAnchorTop);
        Map::Entry::View vEntryBottom = SimpleMapEntry::create((Object::View) NULL, voAnchorBottom);

        while (hIter->hasNext())
            {
            Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());

            if (fSkip)
                {
                int32_t nCompare = vComparator->compare(vEntry, vEntryTop);

                fSkip = fInclusive ? (nCompare < 0) : (nCompare <= 0);
                if (fSkip)
                    {
                    continue;
                    }
                }

            if (fHeading)
                {
                haEntry[iEntry] = vEntry;

                if (++iEntry == cPageSize)
                    {
                    break;
                    }
                }
            else
                {
                if (vComparator->compare(vEntry, vEntryBottom) >= 0)
                    {
                    break;
                    }

                haEntry[iEntry] = vEntry;

                if (++iEntry == cPageSize)
                    {
                    fWrap  = true;
                    iEntry = 0;
                    }
                }
            }

        if (fWrap)
            {
            ObjectArray::Handle hao = ObjectArray::create(cPageSize);

            Arrays::copy(haEntry, iEntry, hao, 0, cPageSize - iEntry);
            Arrays::copy(haEntry, 0, hao, cPageSize - iEntry, iEntry);
            haEntry = hao;
            }
        else if (iEntry < cPageSize)
            {
            // last page is not full
            int                 cSize = iEntry;
            ObjectArray::Handle hao   = ObjectArray::create(cSize);

            if (cSize > 0)
                {
                Arrays::copy(haEntry, 0, hao, 0, cSize);
                }
            haEntry = hao;
            }
        }
    return haEntry;
    }

COH_CLOSE_NAMESPACE3
