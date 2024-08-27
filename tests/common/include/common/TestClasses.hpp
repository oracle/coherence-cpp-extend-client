/*
 * Copyright (c) 2000, 2024, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TEST_CLASSES_HPP
#define COH_TEST_CLASSES_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"

#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/DefaultConfigurableCacheFactory.hpp"
#include "coherence/net/AbstractInvocable.hpp"
#include "coherence/net/InvocationService.hpp"

#include "coherence/net/cache/KeyAssociation.hpp"

#include "coherence/util/Filter.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/QueryMap.hpp"
#include "coherence/util/ValueExtractor.hpp"

#include "coherence/util/processor/AbstractProcessor.hpp"

#include "common/TestUtils.hpp"

COH_OPEN_NAMESPACE2(common,test)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::net::AbstractInvocable;
using coherence::net::CacheFactory;
using coherence::net::DefaultConfigurableCacheFactory;
using coherence::net::InvocationService;
using coherence::net::cache::KeyAssociation;
using coherence::util::Filter;
using coherence::util::Iterator;
using coherence::util::QueryMap;
using coherence::util::ValueExtractor;
using coherence::util::processor::AbstractProcessor;

class TestQueryMapEntry :
    public class_spec<TestQueryMapEntry,
        extends<Object>,
        implements<QueryMap::Entry> >
    {
    friend class factory<TestQueryMapEntry>;

    public:
        TestQueryMapEntry(Object::View vKey, Object::View vValue)
            : m_vKey(self()), m_vValue(self())
        {
            m_vKey   = vKey;
            m_vValue = vValue;
        }

// ----- QueryMap::Entry interface ------------------------------------------

    public:
        virtual Object::Holder extract(ValueExtractor::View vExtractor) const
            {
            return vExtractor->extract(m_vKey);
            }

// ----- Accessors ----------------------------------------------------------

    public:
        Object::Holder getValue() const
            {
            return m_vValue;
            }

        Object::Holder getValue()
            {
            return m_vValue;
            }

        Object::Holder setValue(Object::Holder vValue)
            {
            m_vValue = vValue;
            return m_vValue;
            }

        Object::View getKey() const
            {
            return m_vKey;
            }

        void setKey(Object::View vKey)
            {
            m_vKey = vKey;
            }

    MemberView<Object> m_vKey;
    MemberView<Object> m_vValue;
    };


    class TestPart :
        public class_spec<TestPart,
            extends<Object>,
            implements<PortableObject> >
        {
        friend class factory<TestPart>;

        public:
            TestPart()
                : m_nPartId(42), m_vName(self())
                { }

            TestPart(int32_t nPartId, String::View vsName)
                : m_nPartId(nPartId), m_vName(self(), vsName)
                { }


            // ----- PortableObject interface -------------------------------------------

            void readExternal(PofReader::Handle hIn)
                {
                m_nPartId = hIn->readInt32(0);
                m_vName   = hIn->readString(1);
                }

            void writeExternal(PofWriter::Handle hOut) const
                {
                hOut->writeInt32(0, m_nPartId);
                hOut->writeString(1, m_vName);
                }


            // ----- Object interface -----------------------------------------------

            public:
                virtual bool equals(Object::View v) const
                    {
                    if (this == v)
                        {
                        return true;
                        }
                    TestPart::View that = cast<TestPart::View>(v, false);
                    if (that != NULL)
                        {
                        return (this->m_nPartId == that->m_nPartId) &&
                               (this->m_vName->equals(that->m_vName));
                        }
                    return false;
                    }

                virtual size32_t hashCode() const
                    {
                    return m_nPartId + m_vName->hashCode();
                    }


    // ----- Accessors ----------------------------------------------------------

        public:
            int32_t getPartId()
                {
                return m_nPartId;
                }

            int32_t getPartId() const
                {
                return m_nPartId;
                }

            String::View getName()
                {
                return m_vName;
                }

        private:
            int32_t m_nPartId;
            MemberView<String> m_vName;
        };
COH_REGISTER_PORTABLE_CLASS(4006, TestPart);

class TestOrder :
    public class_spec<TestOrder,
        extends<Object>,
        implements<PortableObject> >
    {
    friend class factory<TestOrder>;

    public:
        TestOrder()
            : m_vOrderId(self()), m_vName(self()), m_nQty(42),
              m_vPart(self(), TestPart::create(42, String::create("widget")))
            { }

        TestOrder(Object::View voOrderId, String::View vsName)
            : m_vOrderId(self(), voOrderId), m_vName(self(), vsName),
              m_nQty(42), m_vPart(self(), TestPart::create(42, String::create("widget")))

            { }

        TestOrder(Object::View voOrderId, String::View vsName, int32_t nQty)
            : m_vOrderId(self(), voOrderId), m_vName(self(), vsName),
              m_nQty(nQty), m_vPart(self(), TestPart::create(nQty,
                      String::create("widget")))

            { }


        // ----- PortableObject interface -------------------------------------------

        void readExternal(PofReader::Handle hIn)
            {
            m_vOrderId = hIn->readObject(0);
            m_vName    = hIn->readString(1);
            m_nQty     = hIn->readInt32(2);
            m_vPart    = hIn->readObject(3);
            }

        void writeExternal(PofWriter::Handle hOut) const
            {
            hOut->writeObject(0, m_vOrderId);
            hOut->writeString(1, m_vName);
            hOut->writeInt32(2, m_nQty);
            hOut->writeObject(3, m_vPart);
            }

// ----- Accessors ----------------------------------------------------------

    public:
        Object::View getOrderId()
            {
            return m_vOrderId;
            }

        Object::View getOrderId() const
            {
            return m_vOrderId;
            }

        String::View getName()
            {
            return m_vName;
            }

        int32_t getQty()
            {
            return m_nQty;
            }

    private:
        MemberView<Object> m_vOrderId;
        MemberView<String> m_vName;
        int32_t            m_nQty;
        MemberView<Object> m_vPart;
    };
COH_REGISTER_PORTABLE_CLASS(4001, TestOrder);

class TestOrderKey :
    public class_spec<TestOrderKey,
        extends<Object>,
        implements<KeyAssociation, PortableObject> >
    {
    friend class factory<TestOrderKey>;

    public:
        TestOrderKey()
            : m_vId(self())
            { }

        TestOrderKey(Object::View voId)
            : m_vId(self(), voId)
            { }

// ----- KeyAssociation interface -------------------------------------------

    public:
        Object::View getAssociatedKey() const
            {
            return m_vId;
            }

// ----- PortableObject interface -------------------------------------------

    public:
        void readExternal(PofReader::Handle hIn)
            {
            m_vId = hIn->readObject(0);
            }

        void writeExternal(PofWriter::Handle hOut) const
            {
            hOut->writeObject(0, m_vId);
            }

    private:
        MemberView<Object> m_vId;
    };
COH_REGISTER_PORTABLE_CLASS(4002, TestOrderKey);

class TestItem :
    public class_spec<TestItem,
        extends<Object>,
        implements<PortableObject> >
    {
    friend class factory<TestItem>;

    public:
        TestItem()
            : m_vItemId(self()), m_sum(0)
            { }

        TestItem(Object::View voItemId, float32_t fSum)
            : m_vItemId(self(), voItemId), m_sum(fSum)
            { }

// ----- PortableObject interface -------------------------------------------

    public:
        void readExternal(PofReader::Handle hIn)
            {
            m_vItemId = hIn->readObject(0);
            m_sum    = hIn->readFloat32(1);
            }

        void writeExternal(PofWriter::Handle hOut) const
            {
            hOut->writeObject(0, m_vItemId);
            hOut->writeFloat32(1, m_sum);
            }

// ----- Accessors ----------------------------------------------------------

    public:
        Object::View getItemId()
            {
            return m_vItemId;
            }

        float32_t getSum()
            {
            return m_sum;
            }

    private:
        MemberView<Object>  m_vItemId;
        float32_t m_sum;
    };
COH_REGISTER_PORTABLE_CLASS(4003, TestItem);

class TestItemKey :
    public class_spec<TestItemKey,
        extends<Object>,
        implements<KeyAssociation, PortableObject> >
    {
    friend class factory<TestItemKey>;

    public:
        TestItemKey()
            : m_vId(self()), m_vParentId(self())
            { }

        TestItemKey(Object::View voId, Object::View voParentId)
            : m_vId(self(), voId), m_vParentId(self(), voParentId)
            { }

// ----- KeyAssociation interface -------------------------------------------

    public:
        Object::View getAssociatedKey() const
            {
            return m_vParentId;
            }

// ----- PortableObject interface -------------------------------------------

    public:
        void readExternal(PofReader::Handle hIn)
            {
            m_vId = hIn->readObject(0);
            m_vParentId = hIn->readObject(1);
            }

        void writeExternal(PofWriter::Handle hOut) const
            {
            hOut->writeObject(0, m_vId);
            hOut->writeObject(1, m_vParentId);
            }

    private:
        MemberView<Object> m_vId;
        MemberView<Object> m_vParentId;
    };
COH_REGISTER_PORTABLE_CLASS(4004, TestItemKey);

class KAFValidationInvocable
    : public class_spec<KAFValidationInvocable,
        extends<AbstractInvocable>,
        implements<PortableObject> >
    {
    friend class factory<KAFValidationInvocable>;

    public:
        KAFValidationInvocable()
            : f_voaKeys(self())
            { }

        KAFValidationInvocable(ObjectArray::View voaKeys)
            : f_voaKeys(self(), voaKeys)
            { }

// ----- PortableObject interface -------------------------------------------

        void readExternal(PofReader::Handle /*hIn*/)
        { }

        void writeExternal(PofWriter::Handle hOut) const
        {
            hOut->writeObjectArray(0, f_voaKeys);
        }

// ----- Accessors ----------------------------------------------------------

    private:
        FinalView<ObjectArray> f_voaKeys;
    };
COH_REGISTER_PORTABLE_CLASS(4005, KAFValidationInvocable);

class Bird
    : public class_spec<Bird,
        extends<Object>,
        implements<PortableObject> >
    {
    friend class factory<Bird>;

    public:
        virtual String::View getType() const
            {
            return "bird";
            }

        virtual int32_t getWings() const
            {
            return 2;
            }


    // ----- PortableObject interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void readExternal(PofReader::Handle /* hIn */)
            {
            }

        /**
        * {@inheritDoc}
        */
        virtual void writeExternal(PofWriter::Handle /* hOut */) const
            {
            }
    };
    COH_REGISTER_PORTABLE_CLASS(4010, Bird);
    COH_REGISTER_METHOD(Bird, COH_METHOD0(String::View, Bird::View, getType));
    COH_REGISTER_METHOD(Bird, COH_METHOD0(BoxHandle<const Integer32>, Bird::View, getWings));

class Fish
    : public class_spec<Fish,
        extends<Object>,
        implements<PortableObject> >
    {
    friend class factory<Fish>;

    public:
        virtual String::View getType() const
            {
            return "fish";
            }


    // ----- PortableObject interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void readExternal(PofReader::Handle /* hIn */)
            {
            }

        /**
        * {@inheritDoc}
        */
        virtual void writeExternal(PofWriter::Handle /* hOut */) const
            {
            }
    };
    COH_REGISTER_PORTABLE_CLASS(4011, Fish);
    COH_REGISTER_METHOD(Fish, COH_METHOD0(String::View, Fish::View, getType));

/**
* ExampleAddress provides serializable class with accessors.
*
* @since Coherence 3.7.1.10
* @par 7/24/13
*/
class ExampleAddress
    : public class_spec<ExampleAddress,
        extends<Object>,
        implements<PortableObject, Comparable> >
    {
    friend class factory<ExampleAddress>;

    protected:

        /**
        * Construct an ExampleAddress.
        *
        * @param vsStreet  the street address
        * @param vsCity    city name
        * @param vsState   State name
        * @param vsZip     Zip (postal) code
        */
        ExampleAddress(String::View vsStreet1 = String::null_string,
                       String::View vsStreet2 = String::null_string,
                       String::View vsCity = String::null_string,
                       String::View vsState = String::null_string,
                       String::View vsZip = String::null_string,
                       String::View vsCountry = String::null_string)
            : m_vsStreet1(self(), vsStreet1),
              m_vsStreet2(self(), vsStreet2),
              m_vsCity(self(), vsCity),
              m_vsState(self(), vsState),
              m_vsZip(self(), vsZip),
              m_vsCountry(self(), vsCountry)
            {
            }


    // ----- PortableObject interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void readExternal(PofReader::Handle hIn)
            {
            setStreet1(hIn->readString(0));
            setStreet2(hIn->readString(1));
            setCity(hIn->readString(2));
            setState(hIn->readString(3));
            setZipCode(hIn->readString(4));
            setCountry(hIn->readString(5));
            }

        /**
        * {@inheritDoc}
        */
        virtual void writeExternal(PofWriter::Handle hOut) const
            {
            hOut->writeString(0, getStreet1());
            hOut->writeString(1, getStreet2());
            hOut->writeString(2, getCity());
            hOut->writeString(3, getState());
            hOut->writeString(4, getZipCode());
            hOut->writeString(5, getCountry());
            }

    // ----- Comparable -----------------------------------------------------

        /**
        * {@inheritDoc}
        */
        virtual int32_t compareTo(Object::View vObj) const
            {
            int32_t nRetValue = 0;
            if (vObj == NULL)
                {
                return 1;
                }

            if (!instanceof<ExampleAddress::View>(vObj))
                {
                return 1;
                }
            ExampleAddress::View vAd = cast<ExampleAddress::View>(vObj);

            if (vAd->getStreet1()  == NULL)
                {
                nRetValue = (this->getStreet1() == NULL ? 0 : 1);
                }
            else
                {
                nRetValue = vAd->getStreet1()->compareTo(this->getStreet1());
                }
            if (nRetValue != 0)
                {
                return nRetValue;
                }

            if (vAd->getStreet2()  == NULL)
                {
                nRetValue = (this->getStreet2() == NULL ? 0 : 1);
                }
            else
                {
                nRetValue = vAd->getStreet2()->compareTo(this->getStreet2());
                }
            if (nRetValue != 0)
                {
                return nRetValue;
                }

            if (vAd->getCity() == NULL)
                {
                nRetValue = (this->getCity() == NULL ? 0 : 1);
                }
            else
                {
                nRetValue = vAd->getCity()->compareTo(this->getCity());
                }
            if (nRetValue != 0)
                {
                return nRetValue;
                }

            if (vAd->getState() == NULL)
                {
                nRetValue = (this->getState() == NULL ? 0 : 1);
                }
            else
                {
                nRetValue = vAd->getState()->compareTo(this->getState());
                }
            if (nRetValue != 0)
                {
                return nRetValue;
                }

            if (vAd->getCountry() == NULL)
                {
                nRetValue = (this->getCountry() == NULL ? 0 : 1);
                }
            else
                {
                nRetValue = vAd->getCountry()->compareTo(this->getCountry());
                }
            if (nRetValue != 0)
                {
                return nRetValue;
                }

            if (vAd->getZipCode()  == NULL)
                {
                return (this->getZipCode() == NULL ? 0 : 1);
                }
            return vAd->getZipCode()->compareTo(this->getZipCode());
        }


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        bool equals(Object::View v) const
            {
            if (this == v)
                {
                return true;
                }

            if (instanceof<ExampleAddress::View>(v))
                {
                ExampleAddress::View vThat = cast<ExampleAddress::View>(v);
                return Object::equals(this->getStreet1(), vThat->getStreet1())
                    && Object::equals(this->getStreet2(), vThat->getStreet2())
                    && Object::equals(this->getCity(),    vThat->getCity())
                    && Object::equals(this->getState(),   vThat->getState())
                    && Object::equals(this->getZipCode(), vThat->getZipCode())
                    && Object::equals(this->getCountry(), vThat->getCountry());
                }
            return false;
            }

        /**
        * {@inheritDoc}
        */
        int getHashCode()
            {
            return (getStreet1() == NULL ? 0 : getStreet1()->hashCode()) ^
                   (getStreet2() == NULL ? 0 : getStreet2()->hashCode()) ^
                   (getZipCode() == NULL ? 0 : getZipCode()->hashCode());
            }


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Return the first line of street address.
        *
        * @return the first line of street address
        */
        String::View getStreet1() const
            {
            return m_vsStreet1;
            }

        /**
        * Set the first line of street address.
        *
        * @param sStreet  the first line of street address
        */
        void setStreet1(String::View vsStreet)
            {
            m_vsStreet1 = vsStreet;
            }

        /**
        * Return the second line of street address.
        *
        * @return the second line of street address
        */
        String::View getStreet2() const
            {
            return m_vsStreet2;
            }

        /**
        * Set the second line of street address.
        *
        * @param sStreet  the second line of street address
        */
        void setStreet2(String::View vsStreet)
            {
            m_vsStreet2 = vsStreet;
            }

        /**
        * Return the city name.
        *
        * @return the city name
        */
        String::View getCity() const
            {
            return m_vsCity;
            }

        /**
        * Set the city name.
        *
        * @param sCity  the city name
        */
        void setCity(String::View vsCity)
            {
            m_vsCity = vsCity;
            }

        /**
        * Return the state or territory name.
        *
        * @return the state or territory name
        */
        String::View getState() const
            {
            return m_vsState;
            }

        /**
        * Set the State or Province name.
        *
        * @param sState  the State or Province name
        */
        void setState(String::View vsState)
             {
             m_vsState = vsState;
             }

        /**
        * Return the Zip code.
        *
        * @return the Zip code
        */
        String::View getZipCode() const
            {
            return m_vsZip;
            }

        /**
        * Set the Zip code.
        *
        * @param sZip  the Zip code
        */
        void setZipCode(String::View vsZip)
            {
            m_vsZip = vsZip;
            }

        /**
        * Return the Country.
        *
        * @return the country
        */
        String::View getCountry() const
            {
            return m_vsCountry;
            }

        /**
        * Set the Country.
        *
        * @param sCountry  the Country
        */
        void setCountry(String::View vsCountry)
            {
            m_vsCountry = vsCountry;
            }


    // ----- data members ---------------------------------------------------

    private:
        /**
        * First line of Street address.
        */
        MemberView<String> m_vsStreet1;

        /**
        * Second line of Street address.
        */
        MemberView<String> m_vsStreet2;

        /**
        * City.
        */
        MemberView<String> m_vsCity;

        /**
        * State or Province.
        */
        MemberView<String> m_vsState;

        /**
        * Zip or other postal code.
        */
        MemberView<String> m_vsZip;

        /**
        * Country.
        */
        MemberView<String> m_vsCountry;
    };

    COH_REGISTER_PORTABLE_CLASS(3006, ExampleAddress);


/**
* TestContact class encapsulates common contact information for a person.
*
* This serves as an example data object which does not have direct knowledge
* of Coherence but can be stored in the data grid.
*
* @since Coherence 3.7.1.10
* @author par 7/24/13
*/
class TestContact
    : public class_spec<TestContact,
        extends<Object>,
        implements<PortableObject, Comparable> >
    {
    friend class factory<TestContact>;

    // ----- constructors ---------------------------------------------------

    public:
        /**
        * Create a new TestContact object.
        *
        * @param sName    the name of the person
        * @param sStreet  the street on which the person lives
        * @param sCity    the city where the person lives
        * @param sState   the state where the person lives
        * @param sZip     the zip code of the city where the person lives
        */
        TestContact(String::View vsFirstName,
                    String::View vsLastName,
                    ExampleAddress::View vHomeAddress)
            : m_vsFirstName(self(), vsFirstName),
              m_vsLastName(self(), vsLastName),
              m_vHomeAddress(self(), vHomeAddress)
            { }

    protected:
        /**
        * Default constructor.
        */

        TestContact()
            : m_vsFirstName(self()),
              m_vsLastName(self()),
              m_vHomeAddress(self())
            { }


    // ---- PortableObject Interface -----------------------------------------
    public:
        /**
        * {@inheritDoc}
        */
        virtual void readExternal(PofReader::Handle hReader)
        {
            setFirstName(hReader->readString(0));
            setLastName(hReader->readString(1));
            setHomeAddress(cast<ExampleAddress::View>(hReader->readObject(2)));
        }

        /**
        * {@inheritDoc}
        */
        virtual void writeExternal(PofWriter::Handle hWriter) const
        {
            hWriter->writeString(0, getFirstName());
            hWriter->writeString(1, getLastName());
            hWriter->writeObject(2, getHomeAddress());
        }


    // ---- Comparable methods ----------------------------------------------

    public:
        /**
        * Compare two objects.
        */
        virtual int32_t compareTo(Object::View vObj) const
            {
            int32_t nRetValue = 0;
            if (vObj == NULL)
                {
                return 1;
                }

            if (!instanceof<TestContact::View>(vObj))
                {
                return 1;
                }

            TestContact::View vTc = cast<TestContact::View>(vObj);
            if (vTc->getFirstName() == NULL)
                {
                nRetValue = (this->getFirstName() == NULL ? 0 : 1);
                }
            else
                {
                nRetValue = this->getFirstName()->compareTo(vTc->getFirstName());
                }
            if (nRetValue != 0)
                {
                return nRetValue;
                }

            if (vTc->getLastName() == NULL)
                {
                nRetValue = (this->getLastName() == NULL ? 0 : 1);
                }
            else
                {
                nRetValue = this->getLastName()->compareTo(vTc->getLastName());
                }
            if (nRetValue != 0)
                {
                return nRetValue;
                }

            if (vTc->getHomeAddress() == NULL)
                {
                return (this->getHomeAddress() == NULL ? 0 : 1);
                }
            return this->getHomeAddress()->compareTo(vTc->getHomeAddress());

            }


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Determine the first name of the person.
        *
        * @return the person's first name
        */
        String::View getFirstName() const
            {
            return m_vsFirstName;
            }

        /**
        * Configure the first name of the person.
        *
        * @param vsName  the person's first name
        */

        void setFirstName(String::View vsName)
            {
            m_vsFirstName = vsName;
            }

        /**
        * Determine the last name of the person.
        *
        * @return the person's first name
        */
        String::View getLastName() const
            {
            return m_vsLastName;
            }

        /**
        * Configure the last name of the person.
        *
        * @param vsName  the person's last name
        */

        void setLastName(String::View vsName)
            {
            m_vsLastName = vsName;
            }

        /**
        * Determine the home address of the person.
        *
        * @return the home address
        */
        ExampleAddress::View getHomeAddress() const
            {
            return m_vHomeAddress;
            }

        /**
        * Configure the home address of the person.
        *
        * @param vsAddress  the home address
        */
        void setHomeAddress(ExampleAddress::View vAddress)
            {
            m_vHomeAddress = vAddress;
            }


    // ----- data members ---------------------------------------------------

    private:
        /**
        * The person's first name.
        */
        MemberView<String> m_vsFirstName;

        /**
        * The person's last name.
        */
        MemberView<String> m_vsLastName;

        /**
        * The person's home address.
        */
        MemberView<ExampleAddress> m_vHomeAddress;
    };

    COH_REGISTER_PORTABLE_CLASS(3004, TestContact);


/**
* FilterFetcher is an invocable object that will create
* a filter or ValueExtractor within the cache.
*
* @since Coherence 3.7.1.10
* @author par 7/24/13
*/
class FilterFetcher
    : public class_spec<FilterFetcher,
        extends<AbstractInvocable>,
        implements<PortableObject> >
    {
    friend class factory<FilterFetcher>;

    // ----- constructors ---------------------------------------------------

    public:
        /**
        * Default constructor
        */
        FilterFetcher()
            : m_fFetchExtractor(false),
              m_vQuery(self())
            { }

        /**
        * Construct a new FilterFetcher that will return a Filter
        * based on the given string.
        */
        FilterFetcher(String::View vQuery)
            : m_fFetchExtractor(false),
              m_vQuery(self(), vQuery)
           { }


        /**
        * Construct a new FilterFetcher that will return a Filter
        * based on the given string. Given flag controls whether Filter
        * or ValueExtractor is retrieved.
        */
        FilterFetcher(String::View vQuery, bool fFetchExtractor)
            : m_fFetchExtractor(fFetchExtractor),
              m_vQuery(self(), vQuery)
            { }


    // ----- PortableObject interface -------------------------------------------

        /**
        * @inheritDoc
        */
        void readExternal(PofReader::Handle hIn)
            {
            m_fFetchExtractor = hIn->readBoolean(0);
            m_vQuery          = hIn->readString(1);
            }

        /**
        * Save the contents of a user type by writinging its state.
        */
        void writeExternal(PofWriter::Handle hOut) const
            {
            hOut->writeBoolean    (0, m_fFetchExtractor);
            hOut->writeString     (1, m_vQuery);
            }


    // ----- Data Members ---------------------------------------------------

    private:
        /**
        * Flag whether to get a ValueExtractor or a Filter.
        */
        bool m_fFetchExtractor;

        /**
        * Query string for request.
        */
        MemberView<String> m_vQuery;
    };
    COH_REGISTER_PORTABLE_CLASS(3005, FilterFetcher);


/**
* The FilterFactory class is a utility class that provides a set
* of factory methods used for building instances of Filter or
* ValueExtractor.
*
* @since Coherence 3.7.1.10
* @par 7/24/13
*/
class FilterFactory
    : public class_spec<FilterFactory,
        extends<Object> >
    {
    friend class factory<FilterFactory>;

    // ----- constructors ---------------------------------------------------

    private:
        /**
        * DefaultConstructor-blocks construction as this class has only
        * static methods
        */
        FilterFactory()
            { }


    // ----- FilterBuilding methods ------------------------------------------

    public:
        /**
        * Make a new Filter from the given string.
        *
        * @param sQuery  the query string.
        */
        static Filter::View createFilter(String::View vsQuery, String::View vsServiceName)
            {
            InvocationService::Handle hService = cast<InvocationService::Handle>
                    (CacheFactory::getService(vsServiceName));
            Map::View vResult = hService->query(FilterFetcher::create(vsQuery), NULL);
            if (vResult->size() == 0)
                {
                return NULL;
                }

            Filter::View vFilter = cast<Filter::View>(vResult->get(cast<DefaultConfigurableCacheFactory::Handle>
                        (CacheFactory::getConfigurableCacheFactory())
                        ->ensureOperationalContext()->getLocalMember()));
            return vFilter;
            }

        /**
        * Make a new ValueExtractor from the given String.
        *
        * @param sQuery     the query string.
        */
        static ValueExtractor::View createExtractor(String::View vsQuery, String::View vsServiceName)
            {
            InvocationService::Handle hService = cast<InvocationService::Handle>
                    (CacheFactory::getService(vsServiceName));
            Map::View vResult = hService->query(FilterFetcher::create(vsQuery, true), NULL);
            if (vResult->size() == 0)
                {
                return NULL;
                }

            Iterator::Handle hIterator = vResult->keySet()->iterator();
            if (hIterator->hasNext())
                {
                return cast<ValueExtractor::View>(hIterator->next());
                }
            return NULL;
            }
    };

/**
 * The TestUtf8Processor is used to validate UTF-8 serialization.
 * <p>
 * The validation logic is in the corresponding Java class.
 *
 * @since 14.1.2.0.0
 **/
class TestUtf8Processor
    : public class_spec<TestUtf8Processor,
        extends<AbstractProcessor>,
        implements<PortableObject> >
    {
    friend class factory<TestUtf8Processor>;

    // ----- constructors ---------------------------------------------------

    public:
        /**
         * Default constructor.
         */
        TestUtf8Processor()
            { }

    // ----- PortableObject interface -------------------------------------------

    public:
        /**
         * @inheritDoc
         */
        void readExternal(PofReader::Handle /* hIn */)
            {
            }

        /**
         * @inheritDoc
         */
        void writeExternal(PofWriter::Handle /* hOut */) const
            {
            }
    };
    COH_REGISTER_PORTABLE_CLASS(1513, TestUtf8Processor);

COH_CLOSE_NAMESPACE2

#endif
