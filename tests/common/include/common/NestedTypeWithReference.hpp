/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NESTED_TYPE_WITH_REFERENCE_HPP
#define COH_NESTED_TYPE_WITH_REFERENCE_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"

#include "common/PortablePerson.hpp"

COH_OPEN_NAMESPACE2(common,test)

using namespace coherence::lang;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


// ----- constants ----------------------------------------------------------

static PortablePerson::Handle getPerson()
    {
    static PortablePerson::Handle hPerson    = NULL;
    static ObjectArray::Handle    haChildren = NULL;
    if (NULL == hPerson)
        {
        hPerson = PortablePerson::create(String::create("Joe Smith"), 45);
        haChildren = ObjectArray::create(2);
        haChildren[0] = PortablePerson::create(String::create("Tom Smith"), 20);
        haChildren[1] = PortablePerson::create(String::create("Ellen Smith"), 17);
        hPerson->setChildren(haChildren);
        }
    return hPerson;
    }
COH_STATIC_INIT(getPerson());


class NestedTypeWithReference:
    public class_spec<NestedTypeWithReference,
        extends<Object>,
        implements<PortableObject> >
    {
    friend class factory<NestedTypeWithReference>;

    // ----- constructors ---------------------------------------------------

    protected:
        NestedTypeWithReference()
            {
            }


    // ----- PortableObject interface ---------------------------------------

    public:
        virtual void readExternal(PofReader::Handle hIn)
            {
            TS_ASSERT(getINTEGER() == (hIn->readInt32(PROP0)));
            PofReader::Handle hNested1 = hIn->createNestedPofReader(PROP1);

            PofReader::Handle hNested2 = hNested1->createNestedPofReader(PROP0);
            TS_ASSERT(hNested2->readString(PROP0)->equals(getSTRING()));
            PortablePerson::View vPerson2 = cast<PortablePerson::View>(hNested2->readObject(PROP1));
            Array<float32_t>::View hFloatArray = hNested2->readFloat32Array(PROP2);
            TS_ASSERT(getFLOAT_ARRAY()->equals(hFloatArray));

            PofReader::Handle hNested3 = hNested2->createNestedPofReader(PROP3);
            ObjectArray::View hStringArray = hNested3->readObjectArray(PROP0);
            TS_ASSERT(getSTRING_ARRAY()->equals(hStringArray));
            hNested3->readRemainder();

            // close nested3 and continue to nested2
            bool b = hNested2->readBoolean(PROP4);
            TS_ASSERT(! b);

            // nested1
            Collection::View hCol = hNested1->readCollection(PROP1, NULL);
            TS_ASSERT(getList()->equals(hCol));

            TS_ASSERT(Float64::compare((float64_t) 2.0, hNested1->readFloat64(PROP2)) == 0);
            TS_ASSERT(5 == hNested1->readInt32(PROP3));
            PortablePerson::View vPerson1 = cast<PortablePerson::View>(hNested1->readObject(PROP5));
            TS_ASSERT(Float64::compare((float64_t) 2.222, hNested1->readFloat64(PROP10)) == 0);

            hNested1->readRemainder();

            TS_ASSERT(Float64::compare((float64_t) 4.444, hIn->readFloat64(PROP2)) == 0);
            TS_ASSERT(15 == hIn->readInt32(PROP3));
            PortablePerson::View vPerson = cast<PortablePerson::View>(hIn->readObject(PROP4));
            TS_ASSERT(vPerson == vPerson1);
            TS_ASSERT(vPerson1 == vPerson2);
            }

        virtual void writeExternal(PofWriter::Handle hOut) const
            {
            hOut->writeInt32(PROP0, getINTEGER());

            PofWriter::Handle hNested1 = hOut->createNestedPofWriter(PROP1);

            PofWriter::Handle hNested2 = hNested1->createNestedPofWriter(PROP0);
            hNested2->writeString(PROP0, getSTRING());
            hNested2->writeObject(PROP1, getPerson());
            hNested2->writeFloat32Array(PROP2, getFLOAT_ARRAY());

            PofWriter::Handle hNested3 = hNested2->createNestedPofWriter(PROP3);
            hNested3->writeObjectArray(PROP0, getSTRING_ARRAY());

            hNested2->writeBoolean(PROP4, false);
            hNested2->writeRemainder(NULL);

            hNested1->writeCollection(PROP1, getList());
            hNested1->writeFloat64(PROP2, 2.0);
            hNested1->writeInt32(PROP3, 5);
            hNested1->writeObject(PROP5, getPerson());
            hNested1->writeFloat64(PROP10, 2.222);

            hOut->writeFloat64(PROP2, 4.444);
            hOut->writeInt32(PROP3, 15);
            hOut->writeObject(PROP4, getPerson());
            }


    // ----- constants ------------------------------------------------------

    public:
        static const size32_t PROP0  = 0;
        static const size32_t PROP1  = 1;
        static const size32_t PROP2  = 2;
        static const size32_t PROP3  = 3;
        static const size32_t PROP4  = 4;
        static const size32_t PROP5  = 5;
        static const size32_t PROP10 = 10;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NESTED_TYPE_WITH_REFERENCE_HPP
