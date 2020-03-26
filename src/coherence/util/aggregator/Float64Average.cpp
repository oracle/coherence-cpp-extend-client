/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/aggregator/Float64Average.hpp"

#include "coherence/io/OctetArrayReadBuffer.hpp"
#include "coherence/io/OctetArrayWriteBuffer.hpp"
#include "coherence/io/ReadBuffer.hpp"
#include "coherence/io/WriteBuffer.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)

using coherence::io::OctetArrayReadBuffer;
using coherence::io::OctetArrayWriteBuffer;
using coherence::io::ReadBuffer;
using coherence::io::WriteBuffer;

COH_REGISTER_PORTABLE_CLASS(30, Float64Average);


// ----- constructors -------------------------------------------------------

Float64Average::Float64Average()
    : super()
    {
    }

Float64Average::Float64Average(ValueExtractor::View vExtractor)
    : super(vExtractor)
    {
    }

Float64Average::Float64Average(String::View vsMethod)
    : super(vsMethod)
    {
    }

// ----- AbstractAggregator Interface  --------------------------------------

void Float64Average::init(bool fFinal)
    {
    AbstractFloat64Aggregator::init(fFinal);
    m_dflResult = 0.0;
    }

void Float64Average::process(Object::View v, bool fFinal)
    {
    if (v != NULL)
        {
        if (fFinal)
            {
            // aggregate partial results packed into a byte array
            Array<octet_t>::View vOA = cast<Array<octet_t>::View>(v);
            OctetArrayReadBuffer::Handle hBuff =
                    OctetArrayReadBuffer::create(vOA, 0 , vOA->length);
            ReadBuffer::BufferInput::Handle hIn = hBuff->getBufferInput();
            m_count     += hIn->readInt32();
            m_dflResult += hIn->readFloat64();
            }
        else
            {
            // collect partial results
            m_count++;
            m_dflResult += cast<Number::View>(v)->getFloat64Value();
            }
        }
    }

Object::Holder Float64Average::finalizeResult(bool fFinal)
    {
    int32_t    c   = m_count;
    float64_t  dfl = m_dflResult;
    if (fFinal)
        {
        // return the final aggregated result
        return c == 0 ? NULL : Float64::valueOf(dfl/c);
        }
    else
        {
        // return partial aggregation data packed into a byte array
        OctetArrayWriteBuffer::Handle hBuff =
                    OctetArrayWriteBuffer::create(12);
        WriteBuffer::BufferOutput::Handle hOut = hBuff->getBufferOutput();

        hOut->writeInt32(c);
        hOut->writeFloat64(dfl);
        return hBuff->toOctetArray();
        }
    }

COH_CLOSE_NAMESPACE3
