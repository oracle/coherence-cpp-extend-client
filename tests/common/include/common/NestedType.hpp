/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NESTED_TYPE_HPP
#define COH_NESTED_TYPE_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"

COH_OPEN_NAMESPACE2(common,test)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


// ----- constants ----------------------------------------------------------

static int32_t getINTEGER()
    {
    static int32_t nINTEGER = 100;
    return nINTEGER;
    }
COH_STATIC_INIT(getINTEGER());

static String::Handle getSTRING()
    {
    static String::Handle hSTRING = "Hello World";
    return hSTRING;
    }
COH_STATIC_INIT(getSTRING());

static ObjectArray::Handle getSTRING_ARRAY()
    {
    static ObjectArray::Handle hSTRING_ARRAY = NULL;
    if (NULL == hSTRING_ARRAY)
        {
	hSTRING_ARRAY    = ObjectArray::create(3);
	hSTRING_ARRAY[0] = String::create("one");
	hSTRING_ARRAY[1] = String::create("two");
	hSTRING_ARRAY[2] = String::create("three");
	}
    return hSTRING_ARRAY;
    }
COH_STATIC_INIT(getSTRING_ARRAY());

static Array<float32_t>::Handle getFLOAT_ARRAY()
    {
    static Array<float32_t>::Handle hFLOAT_ARRAY = NULL;
    if (NULL == hFLOAT_ARRAY)
        {
	hFLOAT_ARRAY    = Array<float32_t>::create(4);
	hFLOAT_ARRAY[0] = 1.0f;
	hFLOAT_ARRAY[1] = 2.0f;
	hFLOAT_ARRAY[2] = 3.3f;
	hFLOAT_ARRAY[3] = 4.4f;
	}
    return hFLOAT_ARRAY;
    }
COH_STATIC_INIT(getFLOAT_ARRAY());

static Collection::Handle getList()
    {
    static Collection::Handle list = ArrayList::create();
    if (list->size() == 0)
        {
	list->add(String::create("four"));
	list->add(String::create("five"));
	list->add(String::create("six"));
	list->add(String::create("seven"));
	list->add(String::create("eight"));
	}
    return list;
    }
COH_STATIC_INIT(getList());

class NestedType:
    public class_spec<NestedType,
        extends<Object>,
        implements<PortableObject> >
    {
    friend class factory<NestedType>;

    // ----- constructors ---------------------------------------------------

    protected:
        NestedType()
            {
            }


    // ----- PortableObject interface ---------------------------------------

    public:
        virtual void readExternal(PofReader::Handle hIn)
            {
	    TS_ASSERT(getINTEGER() == (hIn->readInt32(0)));
            PofReader::Handle hNested1 = hIn->createNestedPofReader(1);

            PofReader::Handle hNested2 = hNested1->createNestedPofReader(0);
            TS_ASSERT(hNested2->readString(0)->equals(getSTRING()));
            Array<float32_t>::View hFloatArray = hNested2->readFloat32Array(2);
            TS_ASSERT(getFLOAT_ARRAY()->equals(hFloatArray));

            PofReader::Handle hNested3 = hNested2->createNestedPofReader(3);
            ObjectArray::View hStringArray = hNested3->readObjectArray(0);
            TS_ASSERT(getSTRING_ARRAY()->equals(hStringArray));
            hNested3->readRemainder();

            // close nested3 and continue to nested2
            bool b = hNested2->readBoolean(4);
            TS_ASSERT(! b);

            // nested1
            Collection::View hCol = hNested1->readCollection(1, NULL);
            TS_ASSERT(getList()->equals(hCol));

            TS_ASSERT(Float64::compare((float64_t) 2.0, hNested1->readFloat64(2)) == 0);
            TS_ASSERT(5 == hNested1->readInt32(3));
            TS_ASSERT(Float64::compare((float64_t) 2.222, hNested1->readFloat64(10)) == 0);

            hNested1->readRemainder();

            TS_ASSERT(Float64::compare((float64_t) 4.444, hIn->readFloat64(2)) == 0);
            TS_ASSERT(15 == hIn->readInt32(3));
            }

        virtual void writeExternal(PofWriter::Handle hOut) const
            {
	    hOut->writeInt32(0, getINTEGER());

            PofWriter::Handle hNested1 = hOut->createNestedPofWriter(1);

            PofWriter::Handle hNested2 = hNested1->createNestedPofWriter(0);
            hNested2->writeString(0, getSTRING());
            hNested2->writeFloat32Array(2, getFLOAT_ARRAY());

            PofWriter::Handle hNested3 = hNested2->createNestedPofWriter(3);
            hNested3->writeObjectArray(0, getSTRING_ARRAY());

            hNested2->writeBoolean(4, false);
            hNested2->writeRemainder(NULL);

            hNested1->writeCollection(1, getList());
            hNested1->writeFloat64(2, 2.0);
            hNested1->writeInt32(3, 5);
            hNested1->writeFloat64(10, 2.222);

            hOut->writeFloat64(2, 4.444);
            hOut->writeInt32(3, 15);
            }
};


COH_CLOSE_NAMESPACE2

#endif // COH_NESTED_TYPE_HPP
