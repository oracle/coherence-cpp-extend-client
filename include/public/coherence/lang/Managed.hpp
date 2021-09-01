/*
 * Copyright (c) 2000, 2021, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_MANAGED_HPP
#define COH_MANAGED_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Object.hpp"
#include "coherence/lang/String.hpp"

#include <cstddef>
#include <functional>
#include <ostream>

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* Managed is an adaptor class which transforms a pre-existing class into a
* Coherence managed Object.
*
* The resulting object will be usable both as the supplied data type and as a
* Coherence managed object. As a managed object it is suitable for storage in
* Coherence caches.
*
* The managed object must be created using its associated static create
* methods, which support either default construction, or copy construction
* from the custom type. The managed object's life-cycle is dictated by reference
* counting, and it may not be manually deleted.
*
* To be compatible with the Managed template the following set of functions
* must be defined for the supplied type:
* <ul>
*   <li>zero parameter constructor (public or protected):
*       <code>CustomType::CustomType()</code></li>
*   <li>copy constructor (public or protected):
*       <code>CustomType::CustomType(const CustomType&)</code></li>
*   <li>equality comparison operator:
*       <code>bool operator==(const CustomType&, const CustomType&)</code>
*   <li>basic_stream output function:
*       <code>template<typename Char, typename Traits> std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const CustomType&)</code></li>
*   <li>hash function:
*       <code>size_t hash_value(const CustomType&)</code></li>
* </ul>
*
* A example of a conforming class would be:
* @code
* class Address
*   {
*   public:
*     Address(const std::string& sCity, const std::String& sState, int nZip)
*       : m_sCity(sCity), m_sState(sState), m_nZip(nZip) {}
*
*     Address(const Address& that)
*       : m_sCity(that.m_sCity), m_sState(that.m_sState), m_nZip(that.m_nZip) {}
*
*   protected:
*     Address()
*       : m_nZip(0) {}
*
*   public:
*     std::string  getCity()  const {return m_sCity;}
*     std::string  getState() const {return m_sState;}
*     int          getZip()   const {return m_nZip;}
*
*   private:
*     const std::string m_sCity;
*     const std::string m_sState;
*     const int         m_nZip;
*   };
*
* bool operator==(const Address& addra, const Address& addrb)
*   {
*   return addra.getZip()   == addrb.getZip() &&
*          addra.getState() == addrb.getState() &&
*          addra.getCity()  == addrb.getCity();
*   }
*
* template<typename Char, typename Traits, class T> std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const Address& addr)
*   {
*   out << addr.getCity() << ", " << addr.getState() << "  " << addr.getZip();
*   return out;
*   }
*
* size_t hash_value(const Address& addr)
*   {
*   return (size_t) addr.getZip();
*   }
* @endcode
*
* Serialization support may be added by specializing the following free-functions:
* <ul>
*   <li>serializer:
*       <code>void serialize<CustomType>(coherence::io::pof::PofWriter::Handle, const CustomType&)</code></li>
*   <li>deserializer:
*       <code>CustomType deserialize<CustomType>(coherence::io::pof::PofReader::Handle)</code></li>
* </ul>
*
* The serialization functions do not need to be defined within the source file
* of the original data type. They only need to be linked into the application,
* and registered with the SystemPofContext via the COH_REGISTER_MANAGED_CLASS
* macro.
*
* @code
* #include "coherence/io/pof/SystemPofContext.hpp"
*
* #include "Address.hpp"
*
* using namespace coherence::io::pof;
*
* COH_REGISTER_MANAGED_CLASS(1234, Address);
*
* template<> void serialize<Address>(PofWriter::Handle hOut, const Address& addr)
*   {
*   hOut->writeString(0, addr.getCity());
*   hOut->writeString(1, addr.getState());
*   hOut->writeInt32 (2, addr.getZip());
*   }
*
* template<> Address deserialize<Address>(PofReader::Handle hIn)
*   {
*   std::string sCity  = hIn->readString(0);
*   std::string sState = hIn->readString(1);
*   int         nZip   = hIn->readInt32 (2);
*
*   return Address(sCity, sState, nZip);
*   }
* @endcode
*
* An example usage of the resulting managed type would be:
* @code
* // construct the non-managed version as usual
* Address office("Redwood Shores", "CA", 94065);
*
* // the managed version can be initialized from the non-managed version
* // the result is a new object, which does not reference the original
* Managed<Address>::View vOffice = Managed<Address>::create(office);
* String::View           vKey    = "Oracle";
*
* // the managed version is suitable for use with caches
* hCache->put(vKey, vAddr);
* vOffice = cast<Managed<Address>::View>(hCache->get(vKey));
*
* // the non-managed class's public methods/fields remain accessible
* assert(vOffice->getCity()  == office.getCity());
* assert(vOffice->getState() == office.getState());
* assert(vOffice->getZip()   == office.getZip());
*
* // conversion back to the non-managed type may be performed using the
* // non-managed class's copy constructor.
* Address officeOut = *vOffice;
* @endcode
*
* @see coherence::io::pof::PofReader
* @see coherence::io::pof::PofWriter
* @see coherence::io::pof::SystemPofContext
*
* @author mf  2007.07.05
*/
template<class T>
class Managed
    : public cloneable_spec<Managed<T> >,
      public T
    {
    friend class factory<Managed<T> >;

    // ----- typedefs -------------------------------------------------------

    public:
        /**
        * The boxed class type.
        */
        typedef T BoxedType;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new Managed<T> instance with the default initial T value.
        *
        * @return the new Managed<T>
        */
        Managed()
            : T()
            {
            // NOTE: a build error here indicates a zero-parameter constructor
            // has not been defined for T
            }

        /**
        * Create a new Managed<T> instance.
        *
        * @param t  the initial value for the templated type
        */
        Managed(const T& t)
            : T(t)
            {
            // NOTE: a build error here indicates a copy constructor has not
            // been defined for T
            }

        /**
        * Copy constructor.
        */
        Managed(const Managed<T>& that)
            : Object(that), cloneable_spec<Managed<T> >(that), T(that)
            {
            // NOTE: a build error here indicates a copy constructor has not
            // been defined for T
            }

        /**
        * @internal
        */
        virtual ~Managed()
            {
            }


    // ----- Managed interface ----------------------------------------------

    protected:
        /**
        * Return the reference to the managed T object.
        *
        * @return the managed object
        */
        virtual T& getManagedObject()
            {
            return *this;
            }

        /**
        * Return the constant reference to managed T object.
        *
        * @return the managed object
        */
        virtual const T& getManagedObject() const
            {
            return *this;
            }


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        *
        * @param v  the object to compare against
        *
        * This method delegates to the custom types equality operator.
        */
        virtual bool equals(Object::View v) const
            {
            // NOTE: a build error here indicates that the following function
            // has not been defined for T:
            //   bool operator==(const T&, const T&)
            return this == v ||
                (instanceof<typename cloneable_spec<Managed<T> >::View>(v) &&
                    getManagedObject() ==
                        cast<typename cloneable_spec<Managed<T> >::View>(v)
                            ->getManagedObject());
            }

        /**
        * {@inheritDoc}
        *
        * @param out  the stream to write to
        *
        * This method delegates to the custom types stream operator. (operator<<)
        */
        virtual TypedHandle<const String> toString() const
            {
            // NOTE: a build error here indicates that the following function
            // has not been defined for your type <T>:
            //   template<typename Char, typename Traits> std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const CustomType& o)
            // alternatively if operator<< has only be defined for std::ostream then
            // build your code defining COH_DEFAULT_NARROW_TO_STRING
            return COH_TO_STRING(getManagedObject());
            }

        /**
        * {@inheritDoc}
        *
        * This method delegates to the global hash function specialized
        * for the custom type.
        */
        virtual size32_t hashCode() const
            {
            // NOTE: a build error here indicates that the following function
            // has not been defined for T:
            //   size_t hash_value(const T&)
            return size32_t(hash_value(*this));
            }


    // ----- memory management ----------------------------------------------

    protected:
        /**
        * @internal
        *
        * Explicitly delegate to Coherence Object to remove ambiguity 
        * for managed classes which inherit some other new().
        */
        static void* operator new(size_t cb)
            {
            return Object::operator new(cb);
            }

        /**
        * @internal
        *
        * Explicitly delegate to Coherence Object to remove ambiguity 
        * for managed classes which inherit some other delete().
        */
        static void operator delete(void* po)
            {
            Object::operator delete(po);
            }
    };


// ----- non-member operators and functions ---------------------------------

/**
* Output a human-readable description of the specified Managed<T> to the given
* stream.
*
* @param out  the stream used to output the description
* @param aT   the Managed<T> to describe
*
* @return the supplied stream
*/
template <typename Char, typename Traits, class T>
COH_INLINE std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const Managed<T>& aT)
    {
    return coherence::lang::operator<<(out, (const Object&) aT);
    }

COH_CLOSE_NAMESPACE2

#endif // COH_MANAGED_HPP
