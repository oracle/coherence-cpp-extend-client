/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_PRIORITY_TASK_HPP
#define COH_ABSTRACT_PRIORITY_TASK_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/net/PriorityTask.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


/**
* An abstract base for AbstractPriorityTask implementations. It implements
* all AbstractPriorityTask interface methods and is intended to be extended
* for concrete uses.
*
* @author djl  2008.05.15
*/
class COH_EXPORT AbstractPriorityTask
    : public abstract_spec<AbstractPriorityTask,
        extends<Object>,
        implements<PortableObject, PriorityTask> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        AbstractPriorityTask();


    // ----- PriorityTask interface -----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t getSchedulingPriority() const;

        /**
        * {@inheritDoc}
        */
        virtual int64_t getExecutionTimeoutMillis() const;

        /**
        * {@inheritDoc}
        */
        virtual int64_t getRequestTimeoutMillis() const;

        /**
        * {@inheritDoc}
        * <p>
        * This implementation is a no-op.
        */
        virtual void runCanceled(bool fAbandoned);


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


    // ----- Data member mutators -------------------------------------------

    public:
        /**
        * Specify this task's scheduling priority. Valid values are one of
        * the SCHEDULE_* constants.
        *
        * @param iPriority  this task's scheduling priority
        */
        virtual void setSchedulingPriority(int32_t iPriority);

        /**
        * Specify the maximum amount of time this task is allowed to run
        * before the corresponding service will attempt to stop it.
        *
        * @param lTimeout the task timeout value in milliseconds
        */
        virtual void setExecutionTimeoutMillis(int64_t lTimeout);

        /**
        * Specify the maximum amount of time a calling thread is willing to
        * wait for a result of the request execution.
        *
        * @param lTimeout  the request timeout value in milliseconds
        */
        virtual void setRequestTimeoutMillis(int64_t lTimeout);


    // ----- data members ---------------------------------------------------

    private:
        /**
        * The scheduling priority value.
        *
        * Default value PriorityTask::schedule_standard
        */
        int32_t m_iSchedulingPriority;

        /**
        * The task execution timeout value.
        *
        * Default value PriorityTask::timeout_default
        */
        int64_t m_lExecutionTimeout;

        /**
        * The request timeout value.
        *
        * Default value PriorityTask::timeout_default
        */
        int64_t m_lRequestTimeout;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ABSTRACT_PRIORITY_TASK_HPP
