/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_AIRDEAL_COMPARATOR_HPP
#define COH_AIRDEAL_COMPARATOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"

#include "common/AirDeal.hpp"

#include "coherence/util/Comparator.hpp"

COH_OPEN_NAMESPACE2(common, test)

using namespace std;

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::util::Comparator;

/**
* Comparator used for testing LimitFilter.
*
* @author lh  2012.09.06.
*/
class AirDealComparator
    : public class_spec<AirDealComparator,
        extends<Object>,
        implements<Comparator, PortableObject> >
    {
    friend class factory<AirDealComparator>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new AirDealComparator
        */
        AirDealComparator()
            {
            }

    // ---- Comparable interface --------------------------------------------

    public:
        /**
        * {@inheritDoc}
        *
        * @throws NullPointerException if either argument is null
        */
        virtual int32_t compare(Object::View v1, Object::View v2) const
            {
            return (int32_t) (cast<AirDeal::View>(v1)->getDealAppeal() - cast<AirDeal::View>(v2)->getDealAppeal());
            }

    // ----- PortableObject interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void readExternal(PofReader::Handle /*hIn*/)
            {
            }

        /**
        * {@inheritDoc}
        */
        virtual void writeExternal(PofWriter::Handle /*hOut*/) const
            {
            }
    };

COH_REGISTER_PORTABLE_CLASS(1013, AirDealComparator);

COH_CLOSE_NAMESPACE2

#endif // COH_AIRDEAL_COMPARATOR_HPP
