/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_EVOLVABLE_PORTABLE_PERSON_HPP
#define COH_EVOLVABLE_PORTABLE_PERSON_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/EvolvablePortableObject.hpp"
#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/util/Binary.hpp"

#include "common/PortablePerson.hpp"

COH_OPEN_NAMESPACE2(common, test)

using coherence::io::pof::EvolvablePortableObject;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::util::Binary;


/**
* Evolvable Portable Person used for testing
*
* @author lh  2011.07.15
*/
class EvolvablePortablePerson
    : public class_spec<EvolvablePortablePerson,
        extends<PortablePerson>,
        implements<EvolvablePortableObject> >
    {
    friend class factory<EvolvablePortablePerson>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new EvolvablePortablePerson
        */
        EvolvablePortablePerson(String::View vsName = String::null_string,
                int32_t nAge = 0)
            : super(vsName, nAge),
              m_binFuture(self()), m_nVersion(0)
            {
            }


    // ----- Evolvable interface --------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t getImplVersion() const
            {
            return 1;
            }

        /**
        * {@inheritDoc}
        */
        virtual int32_t getDataVersion() const
            {
            return m_nVersion;
            }

        /**
        * {@inheritDoc}
        */
        virtual void setDataVersion(int32_t nVersion)
            {
            m_nVersion = nVersion;
            }

        /**
        * {@inheritDoc}
        */
        virtual Binary::View getFutureData() const
            {
            return m_binFuture;
            }

        /**
        * {@inheritDoc}
        */
        virtual void setFutureData(Binary::View binFuture)
            {
            m_binFuture = binFuture;
            }


    // ----- data members ---------------------------------------------------

    protected:
        MemberView<Binary> m_binFuture;
        int32_t            m_nVersion;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_EVOLVABLE_PORTABLE_PERSON_HPP
