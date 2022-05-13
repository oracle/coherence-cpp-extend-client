/*
 * Copyright (c) 2022, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NO_STORAGE_MEMBERS_HPP
#define COH_NO_STORAGE_MEMBERS_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/protocol/cache/NamedCacheRequest.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

/**
 * Message indicating no storage members are available to service a request.
 *
 * @author rl  2022.5.12
 * @since 14.1.2.0.0
 */
class COH_EXPORT NoStorageMembers
    : public class_spec<NoStorageMembers,
        extends<NamedCacheRequest> >
    {
    friend class factory<NoStorageMembers>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
         * Create a new NoStorageMembers instance.
         */
        NoStorageMembers();

    private:
        /**
        * Blocked copy constructor.
        */
        NoStorageMembers(const NoStorageMembers&);

    // ----- Message interface ----------------------------------------------

    public:
        /**
         * {@inheritDoc}
         */
        virtual int32_t getTypeId() const;

    // ----- internal methods -----------------------------------------------

    protected:
        /**
         * {@inheritDoc}
         */
        virtual void onRun(AbstractPofResponse::Handle hResponse);

    // ----- constants ------------------------------------------------------

    public:
        /**
         * The type identifier of this Message class.
         */
        static const int32_t type_id = 56;
    };

COH_CLOSE_NAMESPACE6

#endif // COH_NO_STORAGE_MEMBERS_HPP
