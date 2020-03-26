/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_MEMBER_HPP
#define COH_MEMBER_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE2(coherence,net)


/**
* The Member interface represents a cluster member.
*
* @author jh  2007.12.20
*/
class COH_EXPORT Member
    : public interface_spec<Member>
    {
    // ----- Member interface -----------------------------------------------

    public:
        /**
         * Return the name of the cluster with which this member is associated.
         *
         * @return the cluster name
         *
         * @since Coherence 12.2.1
         */
        virtual String::View getClusterName() const = 0;

        /**
        * Determine the configured name for the Site (such as a data center)
        * in which this Member resides. This name is used for logging
        * purposes and to differentiate among multiple geographic sites.
        *
        * @return the configured Site name or null
        */
        virtual String::View getSiteName() const = 0;

        /**
        * Determine the configured name for the Rack (such as a physical
        * rack, cage or blade frame) in which this Member resides. This name
        * is used for logging purposes and to differentiate among multiple
        * racks within a particular data center, for example.
        *
        * @return the configured Rack name or null
        */
        virtual String::View getRackName() const = 0;

        /**
        * Determine the configured name for the Machine (such as a host name)
        * in which this Member resides. This name is used for logging
        * purposes and to differentiate among multiple servers.
        *
        * @return the configured Machine name or null
        */
        virtual String::View getMachineName() const = 0;

        /**
        * Determine the configured name for the Process (such as a JVM) in
        * which this Member resides. This name is used for logging purposes
        * and to differentiate among multiple processes on a a single
        * machine.
        *
        * @return the configured Process name or null
        */
        virtual String::View getProcessName() const = 0;

        /**
        * Determine the configured name for the Member. This name is used for
        * logging purposes and to differentiate among Members running within
        * a particular process.
        *
        * @return the configured Member name or null
        */
        virtual String::View getMemberName() const = 0;

        /**
        * Determine the configured role name for the Member. This role is
        * completely definable by the application, and can be used to
        * determine what Members to use for specific purposes, such as to
        * send particular types of work to.
        *
        * @return the configured role name for the Member or null
        */
        virtual String::View getRoleName() const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_MEMBER_HPP
