/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SIMPLE_EVOLVABLE_HPP
#define COH_SIMPLE_EVOLVABLE_HPP

#include "coherence/lang.ns"

#include "coherence/io/Evolvable.hpp"

#include "coherence/util/Binary.hpp"

COH_OPEN_NAMESPACE2(coherence,io)

using coherence::util::Binary;


/**
 * Simple implementation of Evolvable interface.
 *
 * @author as  2013.04.19
 *
 * @since Coherence 12.1.3
 */
class COH_EXPORT SimpleEvolvable
    : public class_spec<SimpleEvolvable,
        extends<Object>,
        implements<Evolvable> >
    {
    friend class factory<SimpleEvolvable>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
         * Create SimpleEvolvable instance.
         *
         * @param nImplVersion  the implementation version of the class this
         *                      Evolvable is for
         */
        SimpleEvolvable(int32_t nImplVersion);

    // ---- Evolvable implementation ----------------------------------------

    public:
        /**
         * {@inheritDoc}
         */
        virtual int32_t getImplVersion() const;

        /**
         * {@inheritDoc}
         */
        virtual int32_t getDataVersion() const;

        /**
         * {@inheritDoc}
         */
        virtual void setDataVersion(int32_t nDataVersion);

        /**
         * {@inheritDoc}
         */
        virtual Binary::View getFutureData() const;

        /**
         * {@inheritDoc}
         */
        virtual void setFutureData(Binary::View vBinFutureData);

    // ---- data members ----------------------------------------------------

    private:
        /**
         * Implementation version.
         */
        int32_t m_nImplVersion;

        /**
         * Data version.
         */
        int32_t m_nDataVersion;

        /**
         * Future data.
         */
        MemberView<Binary> m_vBinFutureData;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SIMPLE_EVOLVABLE_HPP
