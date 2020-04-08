/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_MODULE_SERIALIZER_HPP
#define COH_MODULE_SERIALIZER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofSerializer.hpp"
#include "coherence/io/pof/PofWriter.hpp"

#include "common/SerializerTestData.hpp"

#include <typeinfo>

COH_OPEN_NAMESPACE2(common, test)

using namespace coherence::lang;

using coherence::io::pof::PofReader;
using coherence::io::pof::PofSerializer;
using coherence::io::pof::PofWriter;


/**
* Custom serializer for SerializerTestData::Balance,
* SerializerTestData::Customer, and SerializerTestData::Product used by
* SerializerTest.
*
* @author lh  2011.07.15
*/
class ModuleSerializer
    : public class_spec<ModuleSerializer,
        extends<Object> >
    {
    friend class factory<ModuleSerializer>;

    protected:
        ModuleSerializer()
            {
            }

    public:
        class BalanceSerializer
            : public class_spec<BalanceSerializer,
                extends<Object>,
                implements<PofSerializer> >
            {
            friend class factory<BalanceSerializer>;

            public:
                virtual void serialize(PofWriter::Handle hOut, Object::View v) const
                    {
                    SerializerTestData::Balance::View vBal = cast<SerializerTestData::Balance::View>(v);
                    hOut->writeFloat32(0, vBal->getBalance());
                    hOut->writeRemainder(NULL);
                    }

                virtual Object::Holder deserialize(PofReader::Handle hIn) const
                    {
                    SerializerTestData::Balance::Handle hBal = SerializerTestData::Balance::create();
                    hIn->registerIdentity(hBal);
                    hBal->setBalance(hIn->readFloat32(0));
                    hIn->readRemainder();
                    return hBal;
                    }
            };

    public:
        class ProductSerializer
            : public class_spec<ProductSerializer,
                extends<Object>,
                implements<PofSerializer> >
            {
            friend class factory<ProductSerializer>;

            public:
                void serialize(PofWriter::Handle hOut, Object::View v) const
                    {
                    SerializerTestData::Product::View vP = cast<SerializerTestData::Product::View>(v);
                    hOut->writeObject(0, vP->getBalance());
                    hOut->writeRemainder(NULL);
                    }

                Object::Holder deserialize(PofReader::Handle hIn) const
                    {
                    SerializerTestData::Balance::View vBal =
                            cast<SerializerTestData::Balance::View>(hIn->readObject(0));
                    hIn->readRemainder();
                    return SerializerTestData::Product::create(vBal);
                    }
            };

    public:
        class CustomerSerializer
            : public class_spec<CustomerSerializer,
                extends<Object>,
                implements<PofSerializer> >
            {
            friend class factory<CustomerSerializer>;

            public:
                void serialize(PofWriter::Handle hOut, Object::View v) const
                    {
                    SerializerTestData::Customer::View vC = cast<SerializerTestData::Customer::View>(v);
                    hOut->writeString(0, vC->getName());
                    hOut->writeObject(1, vC->getProduct());
                    hOut->writeObject(2, vC->getBalance());
                    hOut->writeRemainder(NULL);
                    }

                Object::Holder deserialize(PofReader::Handle hIn) const
                    {
                    String::View vsName = cast<String::View>(hIn->readString(0));
                    SerializerTestData::Customer::Handle hC = SerializerTestData::Customer::create(vsName);
                    hIn->registerIdentity(hC);
                    hC->setProduct(cast<SerializerTestData::Product::View>(hIn->readObject(1)));
                    hC->setBalance(cast<SerializerTestData::Balance::View>(hIn->readObject(2)));
                    hIn->readRemainder();
                    return hC;
                    }
            };
    };

COH_CLOSE_NAMESPACE2

#endif // COH_MODULE_SERIALIZER_HPP

