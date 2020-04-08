/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_LOCAL_MEMBER_HPP
#define COH_LOCAL_MEMBER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/net/Member.hpp"
#include "coherence/util/UUID.hpp"



COH_OPEN_NAMESPACE2(coherence,net)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::util::UUID;


/**
* Simple Member implementation used as "local" member for RemoteService
* instances started by ConfigurableCacheFactory.
*
* @author js  2008.03.03
*/
class COH_EXPORT LocalMember
    : public class_spec<LocalMember,
        extends<Object>,
        implements<Member, PortableObject> >
    {
    friend class factory<LocalMember>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a LocalMember.
        *
        * @return the new LocalMember
        */
        LocalMember();


    // ----- Member interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getClusterName() const;

        /**
        * {@inheritDoc}
        */
        virtual String::View getSiteName() const;

        /**
        * {@inheritDoc}
        */
        virtual String::View getRackName() const;

        /**
        * {@inheritDoc}
        */
        virtual String::View getMachineName() const;

        /**
        * {@inheritDoc}
        */
        virtual String::View getProcessName() const;

        /**
        * {@inheritDoc}
        */
        virtual String::View getMemberName() const;

        /**
        * {@inheritDoc}
        */
        virtual String::View getRoleName() const;


    // ----- LocalMember interface ------------------------------------------

    public:
        /**
        * Set the local member cluster name.
        *
        * @param vsClusterName  the local member cluster name
        */
        virtual void setClusterName(String::View vsClusterName);

        /**
        * Set the local member site name.
        *
        * @param vsSiteName  the local member site name
        */
        virtual void setSiteName(String::View vsSiteName);

        /**
        * Set the local member rack name.
        *
        * @param vsRackName  the local member rack name
        */
        virtual void setRackName(String::View vsRackName);

        /**
        * Set the local member machine name.
        *
        * @param vsMachineName  the local member machine name
        */
        virtual void setMachineName(String::View vsMachineName);

        /**
        * Set the local member process name.
        *
        * @param vsProcessName  the local member process name
        */
        virtual void setProcessName(String::View vsProcessName);

        /**
        * Set the local member name.
        *
        * @param vsMemberName  the local member name
        */
        virtual void setMemberName(String::View vsMemberName);

        /**
        * Set the local member role name.
        *
        * @param vsRoleName  the local member role name
        */
        virtual void setRoleName(String::View vsRoleName);

    private:
        /*
        * Returns a string representation of the location information
        * for this local member.
        *
        * @return a string representation of the location information for this
        * local member
        */
        virtual String::View getLocationInfo() const;


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


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The local member uuid.
        */
        MemberView<UUID> m_vUUID;

        /**
        * The local member cluster name.
        */
        FinalView<String> f_vsClusterName;

        /**
        * The local member site name.
        */
        FinalView<String> f_vsSiteName;

        /**
        * The local member rack name.
        */
        FinalView<String> f_vsRackName;

        /**
        * The local member machine name.
        */
        FinalView<String> f_vsMachineName;

        /**
        * The local member process name.
        */
        FinalView<String> f_vsProcessName;

        /**
        * The local member name.
        */
        FinalView<String> f_vsMemberName;

        /**
        * The local member role name.
        */
        FinalView<String> f_vsRoleName;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_LOCAL_MEMBER_HPP
