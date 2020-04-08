/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TEST_SERIALIZER_HPP
#define COH_TEST_SERIALIZER_HPP

#include "coherence/lang.ns"

#include "coherence/io/Serializer.hpp"
#include "coherence/io/pof/PofContext.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE2(common,test)

using namespace coherence::lang;

using coherence::io::ReadBuffer;
using coherence::io::Serializer;
using coherence::io::WriteBuffer;
using coherence::io::pof::PofContext;
using coherence::io::pof::SystemPofContext;

/**
* Custom Serializer that delegates to the SystemPofContext
*
* To use this class, you must have a similar delegating class for the
* Java proxy.
*
* Converts string "grid" to "TestSerializer" to verify this serializer
* is getting invoked.
*/
class TestSerializer
    : public class_spec<TestSerializer,
        extends<Object>,
        implements<Serializer> >
    {
    friend class factory<TestSerializer>;

    public:
        virtual void serialize(WriteBuffer::BufferOutput::Handle hOut,
                Object::View v) const
            {
            // transform a well known string value to demonstrate that we are being called
            if (instanceof<String::View>(v) && cast<String::View>(v)->equals("grid"))
                {
                v = String::create("TestSerializer");
                }
            PofContext::Handle hPofContext = SystemPofContext::getInstance();
            hPofContext->serialize(hOut, v);
            }

        virtual Object::Holder deserialize(
                ReadBuffer::BufferInput::Handle hIn) const
            {
            PofContext::Handle hPofContext = SystemPofContext::getInstance();
            return hPofContext->deserialize(hIn);
            }
    };

COH_REGISTER_TYPED_CLASS(TestSerializer);

COH_CLOSE_NAMESPACE2

#endif // COH_TEST_SERIALIZER_HPP
