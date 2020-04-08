/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PORTABLE_HPP
#define COH_PORTABLE_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE4(coherence,io,pof,annotation)

/**
* Portable marks a class as being eligible for use by a
* PofAnnotationSerializer. This annotation is only permitted at class level
* and is a marker annotation with no members. The following class illustrates
* how to use Portable and PortableProperty annotations.
*
* @code
* class Person
*     : public class_spec<Person>
*        {
*        friend class factory<Person>;
*
*        public:
*            String::View getFirstName() const
*                {
*                return m_vsFirstName;
*                }
*
*            void setFirstName(String::View vsFirstName)
*                {
*                m_vsFirstName = vsFirstName;
*                }
*
*            String::View getLastName() const
*                {
*                return m_vsLastName;
*                }
*
*            void setLastName(String::View vsLastName)
*                {
*                m_vsLastName = vsLastName;
*                }
*
*        private:
*            MemberView<String> m_vsFirstName;
*            MemberView<String> m_vsLastName;
*        };
*
*        COH_REGISTER_CLASS(TypedClass<Person>::create()
*            ->annotate(Portable::create(1001))
*            ->declare(COH_PROPERTY(Person, FirstName, String::View)->annotate(PortableProperty::create(0)))
*            ->declare(COH_PROPERTY(Person, LastName,  String::View)->annotate(PortableProperty::create(1))));
*
* @endcode
*
* @author hr 2011.06.29
*
* @since 3.7.1
*
* @see PortableProperty
*/
class COH_EXPORT Portable
    : public class_spec<Portable,
        extends<Object>,
        implements<Annotation> >
    {
    friend class factory<Portable>;

    // ----- Annotation interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Class::View annotationType() const;

    // ----- constants ------------------------------------------------------

    public:
        /**
        * A convenience static method to return a reference to the Class
        * definition for this Portable annotation class.
        *
        * @return a Class representing the Portable definition
        */
        static Class::View getStaticClass();
    };

COH_CLOSE_NAMESPACE4

#endif /* COH_PORTABLE_HPP */
