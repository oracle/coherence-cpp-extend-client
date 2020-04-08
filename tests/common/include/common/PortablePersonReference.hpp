/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PORTABLE_PERSON_REFERENCE_HPP
#define COH_PORTABLE_PERSON_REFERENCE_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"

#include "common/PortablePerson.hpp"

COH_OPEN_NAMESPACE2(common, test)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;


/**
* Used for Identity/Reference and complex object graph testing.
*
* @author lh  2011.07.14
*/
class PortablePersonReference
    : public class_spec<PortablePersonReference,
        extends<PortablePerson> >
    {
    friend class factory<PortablePersonReference>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new PortablePersonReference
        */
        PortablePersonReference(String::View vsName = String::null_string,
                int32_t nAge = 0)
            : super(vsName, nAge),
              m_vaSiblings(self(), NULL), m_vFriend(self(), NULL)
            {
            }


    // ----- PortableObject interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void readExternal(PofReader::Handle hIn)
            {
            super::readExternal(hIn);
            setSiblings(hIn->readObjectArray(5));
            setFriend(cast<PortablePersonReference::View>(hIn->readObject(6)));
            }

        /**
        * {@inheritDoc}
        */
        virtual void writeExternal(PofWriter::Handle hOut) const
            {
            super::writeExternal(hOut);
            hOut->writeObjectArray(5, getSiblings());
            hOut->writeObject(6, getFriend());
            }


    // ----- accessors ------------------------------------------------------

    public:
        virtual PortablePersonReference::View getFriend() const
            {
            return m_vFriend;
            }

        virtual void setFriend(PortablePersonReference::View vFriend)
            {
            m_vFriend = vFriend;
            }

        virtual ObjectArray::View getSiblings() const
            {
            return m_vaSiblings;
            }

        virtual void setSiblings(ObjectArray::View vaSiblings)
            {
            m_vaSiblings = vaSiblings;
            }


    // ----- data members ---------------------------------------------------

    protected:
        MemberView<ObjectArray>             m_vaSiblings;
        MemberView<PortablePersonReference> m_vFriend;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_PORTABLE_PERSON_REFERENCE_HPP
