/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_EVOLVABLE_HPP
#define COH_ABSTRACT_EVOLVABLE_HPP

#include "coherence/lang.ns"

#include "coherence/io/Evolvable.hpp"
#include "coherence/util/Binary.hpp"

COH_OPEN_NAMESPACE2(coherence,io)


using coherence::util::Binary;

/**
* Abstract base implementation of the Evolvable interface.
*
* @author wl 2010.08.11
*/
class COH_EXPORT AbstractEvolvable
    : public abstract_spec<AbstractEvolvable,
        extends <Object>,
        implements<Evolvable> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        AbstractEvolvable();

        /**
        * @internal
        */
        AbstractEvolvable(const AbstractEvolvable& that);


    // ----- Evolvable interface --------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t getImplVersion() const = 0;

        /**
        * {@inheritDoc}
        */
        virtual int32_t getDataVersion() const;

        /**
        * {@inheritDoc}
        */
        virtual void setDataVersion(int32_t nVersion);

        /**
        * {@inheritDoc}
        */
        virtual Binary::View getFutureData() const;

        /**
        * {@inheritDoc}
        */
        virtual void setFutureData(Binary::View vBinFuture);


      // ----- data members -------------------------------------------------

      private:
        /**
        * The version of the data stream that this object was deserialized from.
        */
        int32_t m_nDataVersion;

        /**
        * The "unknown future data" from the data stream that this object was
        * deserialized from.
        */
        FinalView<Binary> f_vBinFuture;
      
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ABSTRACT_EVOLVABLE_HPP

