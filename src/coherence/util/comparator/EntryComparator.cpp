/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/comparator/EntryComparator.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE3(coherence,util,comparator)

COH_REGISTER_PORTABLE_CLASS(41, EntryComparator);

// ----- constants ----------------------------------------------------------

COH_EXPORT_SPEC_MEMBER(const int32_t EntryComparator::cmp_auto)
COH_EXPORT_SPEC_MEMBER(const int32_t EntryComparator::cmp_value)
COH_EXPORT_SPEC_MEMBER(const int32_t EntryComparator::cmp_key)
COH_EXPORT_SPEC_MEMBER(const int32_t EntryComparator::cmp_entry)


// ----- constructors -------------------------------------------------------

EntryComparator::EntryComparator()
    : m_nStyle(cmp_auto)
    {
    }


EntryComparator::EntryComparator(Comparator::View vComparator, int32_t nStyle)
    : super(vComparator)
    {
    switch (nStyle)
        {
        case cmp_auto:
            if (isKeyComparator(vComparator))
                {
                nStyle = cmp_key;
                }
            else
                {
                nStyle = cmp_value;
                }
            break;
        case cmp_value:
        case cmp_key:
        case cmp_entry:
            break;
        default:
            COH_THROW_STREAM(IllegalArgumentException,
                    "Invalid comparison style: " << nStyle);
        }
    }


// ----- Comparator interface -----------------------------------------------

int32_t EntryComparator::compare(Object::View vO1, Object::View vO2) const
    {
    Map::Entry::View ve1 = cast<Map::Entry::View>(vO1);
    Map::Entry::View ve2 = cast<Map::Entry::View>(vO2);

    switch (m_nStyle)
        {
        case cmp_key:
            return SafeComparator::compare(ve1->getKey(), ve2->getKey());

        case cmp_entry:
            {
            QueryMap::Entry::View qve1 = cast<QueryMap::Entry::View>(ve1, false);
            QueryMap::Entry::View qve2 = cast<QueryMap::Entry::View>(ve2, false);

            if (NULL != qve1 && NULL != qve2)
                {
                return compareEntries(qve1, qve2);
                }

            // fall through
            }
        default:
        //case cmp_value:
            return SafeComparator::compare(ve1->getValue(), ve2->getValue());
        }
    }


// ----- PortableObject interface -------------------------------------------

void EntryComparator::readExternal(PofReader::Handle hIn)
    {
    SafeComparator::readExternal(hIn);

    m_nStyle = hIn->readInt32(1);
    }

void EntryComparator:: writeExternal(PofWriter::Handle hOut) const
    {
    SafeComparator::writeExternal(hOut);

    hOut->writeInt32(1, m_nStyle);
    }


// ----- Object interface ---------------------------------------------------

bool EntryComparator::equals(Object::View v) const
    {
    EntryComparator::View vThat = cast<EntryComparator::View>(v, false);
    if (NULL == vThat)
        {
        return false;
        }

    return SafeComparator::equals(vThat) && this->m_nStyle == vThat->m_nStyle;
    }


// ----- accessors ------------------------------------------------------

int32_t EntryComparator::getComparisonStyle()
    {
    return m_nStyle;
    }

bool EntryComparator::isCompareValue()
    {
    //return m_nStyle == cmp_value;
    return cmp_auto;
    }


bool EntryComparator::isCompareKey()
    {
    return m_nStyle == cmp_key;
    }

bool EntryComparator::isCompareEntry()
    {
    return m_nStyle == cmp_entry;
    }

COH_CLOSE_NAMESPACE3
