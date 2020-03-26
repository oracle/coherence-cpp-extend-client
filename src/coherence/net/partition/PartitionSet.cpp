/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/partition/PartitionSet.hpp"

#include "coherence/io/IOException.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE3(coherence,net,partition)

using coherence::io::IOException;

COH_REGISTER_PORTABLE_CLASS(171, PartitionSet);


// ----- constructors -------------------------------------------------------

PartitionSet::PartitionSet()
    : f_halBits(self())
    {
    }


// ----- pseudo Set operations ----------------------------------------------

bool PartitionSet::add(int32_t nPartition)
    {
    if (nPartition < 0 || nPartition >= m_cPartitions)
        {
        COH_THROW_STREAM(IndexOutOfBoundsException,
                nPartition << " not in [0, " << m_cPartitions << ')');
        }

    Array<int64_t>::Handle halBits = f_halBits;
    int32_t                iLong   = (int32_t) ((uint32_t) nPartition >> 6);
    int64_t                lBits   = halBits[iLong];
    int64_t                lMask   = int64_t(1) << (nPartition & 63);

    if ((lBits & lMask) == int64_t(0))
        {
        halBits[iLong] = lBits | lMask;

        int cMarked = m_cMarked;
        if (cMarked >= 0)
            {
            m_cMarked = cMarked + 1;
            }

        return true;
        }
    else
        {
        return false;
        }
    }

bool PartitionSet::add(PartitionSet::View vPartitions)
    {
    Array<int64_t>::Handle halBitsThis = f_halBits;
    Array<int64_t>::View   valBitsThat = vPartitions->f_halBits;
    bool                   fResult     = true;

    for (int i = 0, c = halBitsThis->length; i < c; ++i)
        {
        int64_t lBitsThis = halBitsThis[i];
        int64_t lBitsThat = valBitsThat[i];

        fResult &= (lBitsThis & lBitsThat) == int64_t(0);
        halBitsThis[i] = lBitsThis | lBitsThat;
        }

    m_cMarked = -1;
    return fResult;
    }

void PartitionSet::fill()
    {
    Array<int64_t>::Handle halBits = f_halBits;
    int32_t                iLast   = halBits->length - 1;

    for (int32_t i = 0; i < iLast; ++i)
        {
        halBits[i] = int64_t(-1);
        }
    halBits[iLast] = m_lTailMask;

    m_cMarked = m_cPartitions;
    }

int32_t PartitionSet::next(int32_t nPartition) const
    {
    int32_t cPartitions = m_cPartitions;
    if (nPartition < 0 || nPartition > cPartitions)
        {
        COH_THROW_STREAM(IndexOutOfBoundsException,
                nPartition << " not in [0, " << cPartitions << ')');
        }

    if (nPartition == cPartitions || m_cMarked == 0)
        {
        return -1;
        }

    Array<int64_t>::View valBits = f_halBits;
    int32_t              iLong  = (int32_t) ((uint32_t) nPartition >> 6);
    int32_t              ofBit  = nPartition & 63;
    int64_t              lBits  = valBits[iLong] >> ofBit;

    if (lBits == int64_t(0))
        {
        ofBit = 0;

        // skip empty parts
        for (int iLast = valBits->length - 1; lBits == int64_t(0) && iLong < iLast; )
            {
            lBits = valBits[++iLong];
            }

        if (lBits == int64_t(0))
            {
            return -1;
            }
        }

    return (iLong << 6) + ofBit + getTrailingZeroCount(lBits);
    }


// ----- helpers ------------------------------------------------------------

int32_t PartitionSet::getTrailingZeroCount(int64_t l)
    {
    if (l == 0)
        {
        return 64;
        }

    l = l & -l; // Long.lowestOneBit(l)

    // Long.numberOfTrailingZeros(l);
    int32_t x;
    int32_t y;
    int32_t n = 63;

    y = (int32_t) l; if (y != 0) { n = n -32; x = y; } else x = (int32_t) (((uint64_t) l)>>32);

    y = x << 16; if (y != 0) { n = n -16; x = y; }
    y = x <<  8; if (y != 0) { n = n - 8; x = y; }
    y = x <<  4; if (y != 0) { n = n - 4; x = y; }
    y = x <<  2; if (y != 0) { n = n - 2; x = y; }

    return n - (((uint32_t) (x << 1)) >> 31);
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> PartitionSet::toString() const
    {
    String::View sv      = String::create("PartitionSet{");
    bool         fAppend = false;
    int          cRange  = 0;
    int          iPrev   = -1;

    for (int iPid = next(0); iPid >= 0; iPid = next(iPid + 1))
        {
        if (iPid == (iPrev + 1) && iPrev >= 0)
            {
            // range continuation
            cRange++;
            }
        else
            {
            if (cRange > 0)
                {
                // range completion
                sv     = COH_TO_STRING(sv << (cRange > 1 ? ".." : ", ") << iPrev);
                cRange = 0;
                }
            
            if (fAppend)
                {
                sv = COH_TO_STRING(sv << ", ");
                }
            else
                {
                fAppend = true;
                }
                
            sv = COH_TO_STRING(sv << iPid);
            }
            
        iPrev = iPid;
        }

    if (cRange > 0)
        {
        sv = COH_TO_STRING(sv << (cRange > 1 ? ".." : ", ") << iPrev);
        }
        
    sv = COH_TO_STRING(sv << "}");

    return sv;
    }


// ----- PortableObject interface -------------------------------------------

void PartitionSet::readExternal(PofReader::Handle hIn)
    {
    // 0: partition-count
    // 1: format-indicator
    // 2: int array of gaps (for MARKED_FEW format)
    // 3: long array of bit masks (for MARKED_MANY format)
    // 4: reserved

    int32_t cPartitions = hIn->readInt32(0);
    Format  nFormat     = (Format) hIn->readInt32(1);
    int32_t cLongs      = (int32_t) ((uint32_t) (cPartitions + 63) >> 6);

    Array<int64_t>::Handle halBits = nFormat == marked_many
                       ? hIn->readInt64Array(3)
                       : Array<int64_t>::create(cLongs);

    m_cPartitions = cPartitions;
    initialize(f_halBits, halBits);
    m_lTailMask   = (int64_t) (uint64_t(-1) >> (64 - (cPartitions & 63)));
    m_cMarked     = -1;

    switch (nFormat)
        {
        case marked_none:
            m_cMarked = 0;
            break;

        case marked_few:
            {
            Array<int32_t>::Handle hacSkip = hIn->readInt32Array(2);
            int32_t                cSkips  = hacSkip->length;
            for (int32_t i = 0, iLast = 0; i < cSkips; ++i)
                {
                iLast += hacSkip[i];
                add(iLast);
                }
            m_cMarked = cSkips;
            }
            break;

        case marked_many:
            // handled above
            break;

        case marked_all:
            fill();
            break;

        default:
            COH_THROW_STREAM(IOException, "stream corrupted; format=" << nFormat);
        }
    }

void PartitionSet:: writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeInt32(0, m_cPartitions);
    hOut->writeInt32(1, marked_many);
    hOut->writeInt64Array(3, f_halBits);
    }

COH_CLOSE_NAMESPACE3
