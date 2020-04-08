/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NAME_SERVICE_HPP
#define COH_NAME_SERVICE_HPP

#include "coherence/lang.ns"

#include "coherence/net/Service.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

/**
 * A NameService is a clustered service that accepts connections from
 * external clients (e.g. Coherence*Extend) and provides a name lookup
 * service.
 *
 * @author Wei Lin 2012.06.18
 *
 * @since Coherence 12.1.2
 */
class COH_EXPORT NameService
    : public interface_spec<NameService,
        implements<Service> >
    {
    // ----- NameService interface ------------------------------------------

    public:
        /**
        * Binds a name to an object.
        *
        * @param vsName  the name to bind; may not be empty
        * @param o       the object to bind; possibly null
        */
        virtual void bind(String::View vsName, Object::View o) = 0;

        /**
        * Retrieves the named object.
        *
        * @param vsName  the name of the object to look up
        * 
        * @return the object bound to sName
        *
        * @throws NamingException if a naming exception is encountered
        */
        virtual Object::View lookup(String::View vsName) = 0;

        /**
        * Unbinds the named object.
        *
        * @param vsName  the name to bind; may not be empty
        */
        virtual void unbind(String::View vsName) = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NAME_SERVICE_HPP
