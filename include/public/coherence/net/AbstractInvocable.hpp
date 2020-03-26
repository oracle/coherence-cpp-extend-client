/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_INVOCABLE_HPP
#define COH_ABSTRACT_INVOCABLE_HPP

#include "coherence/lang.ns"

#include "coherence/net/Invocable.hpp"
#include "coherence/net/InvocationService.hpp"
#include "coherence/net/PriorityTask.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

/**
 * An abstract base for Invocable and PriorityTask implementations.
 *
 * For Invocables which only run within the Coherence cluster (most common case),
 * the C++ init, getResult and run methods can be left unimplemented.
 *
 * @author mf 2008.08.26
 */
class COH_EXPORT AbstractInvocable
    : public abstract_spec<AbstractInvocable,
        extends<Object>,
        implements<Invocable, PriorityTask> >
    {
    // ----- Invocable interface --------------------------------------------

    public:
        /**
         * {@inheritDoc}
         *
         * This implementation throws an UnsupportedOperationException.
         */
        virtual void init(InvocationService::Handle hService);

        /**
         * {@inheritDoc}
         *
         * This implementation throws an UnsupportedOperationException.
         */
        virtual Object::Holder getResult() const;

        /**
         * {@inheritDoc}
         *
         * This implementation throws an UnsupportedOperationException.
         */
        virtual void run();

    // ----- PriorityTask interface -----------------------------------------

    public:
        /**
         * {@inheritDoc}
         *
         * This implementation returns schedule_standard.
         */
        virtual int32_t getSchedulingPriority() const;

        /**
         * {@inheritDoc}
         *
         * This implementation returns timeout_default.
         */
        virtual int64_t getExecutionTimeoutMillis() const;

        /**
         * {@inheritDoc}
         *
         * This implementation is a no-op.
         */
        virtual void runCanceled(bool fAbandoned);

        /**
         * {@inheritDoc}
         *
         * This implementation returns timeout_default.
         */
        virtual int64_t getRequestTimeoutMillis() const;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ABSTRACT_INVOCABLE_HPP
