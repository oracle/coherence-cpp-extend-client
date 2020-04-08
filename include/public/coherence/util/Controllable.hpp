/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONTROLLABLE_HPP
#define COH_CONTROLLABLE_HPP

#include "coherence/lang.ns"

#include "coherence/run/xml/XmlElement.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

using coherence::run::xml::XmlElement;


/**
* The Controllable interface represents a configurable dameon-like object,
* quite oftenly referred to as a <i>service</i>, that usually operates on its
* own thread and has a controllable life cycle.
*
* @author jh  2007.12.12
*/
class COH_EXPORT Controllable
    : public interface_spec<Controllable>
    {
    // ----- Controllable interface -----------------------------------------

    public:
        /**
        * Configure the controllable service.
        *
        * This method can only be called before the controllable
        * service is started.
        *
        * @param vXml  an XmlElement carrying configuration information
        *              specific to the Controllable object
        *
        *        virtual void IllegalStateException thrown if the service is
        *            already running
        *        virtual void IllegalArgumentException thrown if the configuration
        *            information is invalid
        */
        virtual void configure(XmlElement::View vXml) = 0;

        /**
        * Determine whether or not the controllable service is running.
        * This method returns false before a service is started, while
        * the service is starting, while a service is shutting down and
        * after the service has stopped. It only returns true after
        * completing its start processing and before beginning its
        * shutdown processing.
        *
        * @return true if the service is running; false otherwise
        */
        virtual bool isRunning() const = 0;

        /**
        * Start the controllable service.
        *
        * This method should only be called once per the life cycle
        * of the Controllable service. This method has no affect if the
        * service is already running.
        *
        *        virtual void IllegalStateException thrown if a service does not
        *            support being re-started, and the service was
        *            already started and subsequently stopped and then
        *            an attempt is made to start the service again; also
        *            thrown if the Controllable service has not been
        *            configured
        */
        virtual void start() = 0;

        /**
        * Stop the controllable service. This is a controlled shut-down,
        * and is preferred to the {@link #stop()} method.
        *
        * This method should only be called once per the life cycle of the
        * controllable service. Calling this method for a service that has
        * already stopped has no effect.
        */
        virtual void shutdown() = 0;

        /**
        * Hard-stop the controllable service. Use {@link #shutdown()}
        * for normal service termination. Calling this method for a service
        * that has already stopped has no effect.
        */
        virtual void stop() = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_CONTROLLABLE_HPP
