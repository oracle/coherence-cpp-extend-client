/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_AIRDEAL_HPP
#define COH_AIRDEAL_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"

COH_OPEN_NAMESPACE2(common, test)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;

/**
* AirDeal used for testing LimitFilter.
*
* @author lh  2012.09.06.
*/
class AirDeal
    : public class_spec<AirDeal,
        extends<Object>,
        implements<PortableObject> >
    {
    friend class factory<AirDeal>;

    // ----- constructors -------------------------------------------

    protected:
        AirDeal()
            : m_oid(0),
              m_vOrigAirport(self()),
              m_vDestAirport(self()),
              m_dealAppeal(0.0)
            {
            }

        AirDeal(int32_t oid, String::View vOrigAirport, String::View vDestAirport, float32_t dealAppeal)
            : m_oid(oid),
              m_vOrigAirport(self(), vOrigAirport),
              m_vDestAirport(self(), vDestAirport),
              m_dealAppeal(dealAppeal)
            {
            }

    // ----- accessors ----------------------------------------------

    public:
        int32_t getOid() const
            {
            return m_oid;
            }

        String::View getOrigAirport() const
            {
            return m_vOrigAirport;
            }

        String::View getDestAirport() const
            {
            return m_vDestAirport;
            }

        float32_t getDealAppeal() const
            {
            return m_dealAppeal;
            }

    // ----- PortableObject interface -----------------------------------

    public:
        /**
         * {@inheritDoc}
         */
        virtual void readExternal(PofReader::Handle hReader)
            {
            m_oid          = hReader->readInt32(0);
            m_vOrigAirport = hReader->readString(1);
            m_vDestAirport = hReader->readString(2);
            m_dealAppeal   = hReader->readFloat32(3);
            }

        /**
         * {@inheritDoc}
         */
        virtual void writeExternal(PofWriter::Handle hWriter) const
            {
            hWriter->writeInt32(0, m_oid);
            hWriter->writeString(1, m_vOrigAirport);
            hWriter->writeString(2, m_vDestAirport);
            hWriter->writeFloat32(3, m_dealAppeal);
            }

    private:
        int32_t            m_oid;
        MemberView<String> m_vOrigAirport;
        MemberView<String> m_vDestAirport;
        float32_t          m_dealAppeal;
    };

COH_REGISTER_PORTABLE_CLASS(1014, AirDeal);
COH_REGISTER_METHOD(AirDeal, COH_METHOD0(String::View, AirDeal::View, getOrigAirport));

COH_CLOSE_NAMESPACE2

#endif // COH_AIRDEAL_HPP
