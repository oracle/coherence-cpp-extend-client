/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PROXYCONTROL_INVOCABLE_HPP
#define COH_PROXYCONTROL_INVOCABLE_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"

#include "coherence/net/AbstractInvocable.hpp"

COH_OPEN_NAMESPACE2(coherence,tests)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::net::AbstractInvocable;


/**
* Invocable implementation that starts/stops the proxy service
*
* @author par  2012.12.12
*/
class ProxyControlInvocable
    : public class_spec<ProxyControlInvocable,
        extends<AbstractInvocable>,
        implements<PortableObject> >
    {
    friend class factory<ProxyControlInvocable>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new ProxyControlInvocable instance.
        */
        ProxyControlInvocable();

        /**
        * Create a new ProxyControlInvocable instance.
        *
        * @param vsService  the name of the service to stop
        */
        ProxyControlInvocable(String::View vsService, int32_t nOperation);

    // ----- PortableObject interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void readExternal(PofReader::Handle hIn);

        /**
        * {@inheritDoc}
        */
        virtual void writeExternal(PofWriter::Handle hOut) const;

    // ----- data members ---------------------------------------------------

    private:
        /**
        * The service name.
        */
        FinalView<String> f_vsService;

        /**
        * The operation to perform.
        */
        int32_t           m_nOperation;

    // ----- constants ------------------------------------------------------

    public:
        /**
         * Start the service.
         */
        static const int32_t start_operation = 1;

        /**
         * Stop the service.
         */
        static const int32_t stop_operation = 2;

        /**
         * Restart the service.
         */
        static const int32_t restart_operation = 3;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_PROXYCONTROL_INVOCABLE_HPP
