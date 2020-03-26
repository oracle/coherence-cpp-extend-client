/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SIMPLE_TYPE_HPP
#define COH_SIMPLE_TYPE_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Collection.hpp"

COH_OPEN_NAMESPACE2(common,test)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::util::ArrayList;
using coherence::util::Collection;


class SimpleType:
    public class_spec<SimpleType,
        extends<Object>,
        implements<PortableObject> >
    {
    friend class factory<SimpleType>;

    // ----- constructors ---------------------------------------------------

    protected:
        SimpleType()
            {
            }


    // ----- PortableObject interface ---------------------------------------

    public:
        virtual void readExternal(PofReader::Handle hIn)
            {
            TS_ASSERT(0 == hIn->readInt32(0));
            TS_ASSERT(1 == hIn->readInt32(1));
            TS_ASSERT(2 == hIn->readInt32(2));
            PofReader::Handle hIn2 = hIn->createNestedPofReader(3);
            TS_ASSERT(0 == hIn2->readInt32(0));
            TS_ASSERT(1 == hIn2->readInt32(1));
            TS_ASSERT(2 == hIn2->readInt32(2));
            TS_ASSERT(4 == hIn->readInt32(4));
            TS_ASSERT(5 == hIn->readInt32(5));
            }

        virtual void writeExternal(PofWriter::Handle hOut) const
            {
            hOut->writeInt32(0, 0);
            hOut->writeInt32(1, 1);
            hOut->writeInt32(2, 2);
            PofWriter::Handle hOut2 = hOut->createNestedPofWriter(3);
            hOut2->writeInt32(0, 0);
            hOut2->writeInt32(1, 1);
            hOut2->writeInt32(2, 2);
            hOut->writeInt32(4, 4);
            hOut->writeInt32(5, 5);
            }
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SIMPLE_TYPE_HPP
