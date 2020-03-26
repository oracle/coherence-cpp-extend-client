/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/net/LocalMember.hpp"

#include "private/coherence/util/StringHelper.hpp"

using coherence::util::StringHelper;

COH_OPEN_NAMESPACE2(coherence,net)

COH_REGISTER_PORTABLE_CLASS(160, LocalMember);


// ----- constructors/destructor  -------------------------------------------

LocalMember::LocalMember()
        : m_vUUID(self()),
          f_vsClusterName(self()),
          f_vsSiteName(self()),
          f_vsRackName(self()),
          f_vsMachineName(self()),
          f_vsProcessName(self()),
          f_vsMemberName(self()),
          f_vsRoleName(self())
    {
    m_vUUID = UUID::create(System::currentTimeMillis(), InetAddress::getLocalHost(), 0, 0);
    }


// ----- Member interface ---------------------------------------------------

String::View LocalMember::getClusterName() const
    {
    return f_vsClusterName;
    }

String::View LocalMember::getSiteName() const
    {
    return f_vsSiteName;
    }

String::View LocalMember::getRackName() const
    {
    return f_vsRackName;
    }

String::View LocalMember::getMachineName() const
    {
    return f_vsMachineName;
    }

String::View LocalMember::getProcessName() const
    {
    return f_vsProcessName;
    }

String::View LocalMember::getMemberName() const
    {
    return f_vsMemberName;
    }

String::View LocalMember::getRoleName() const
    {
    return f_vsRoleName;
    }


// ----- PortableObject interface -------------------------------------------

void LocalMember::readExternal(PofReader::Handle hIn)
    {
    m_vUUID = cast<UUID::View>(hIn->readObject(0));
    setClusterName(hIn->readString(6));
    setSiteName(hIn->readString(7));
    setRackName(hIn->readString(8));
    setMachineName(hIn->readString(9));
    setProcessName(hIn->readString(10));
    setMemberName(hIn->readString(11));
    setRoleName(hIn->readString(12));
    }

void LocalMember::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, m_vUUID);
    hOut->writeString(6, f_vsClusterName);
    hOut->writeString(7, f_vsSiteName);
    hOut->writeString(8, f_vsRackName);
    hOut->writeString(9, f_vsMachineName);
    hOut->writeString(10, f_vsProcessName);
    hOut->writeString(11, f_vsMemberName);
    hOut->writeString(12, f_vsRoleName);
    }


// ----- LocalMember interface ----------------------------------------------

void LocalMember::setClusterName(String::View vsClusterName)
    {
    initialize(f_vsClusterName, vsClusterName);
    }

void LocalMember::setSiteName(String::View vsSiteName)
    {
    initialize(f_vsSiteName, vsSiteName);
    }

void LocalMember::setRackName(String::View vsRackName)
    {
    initialize(f_vsRackName, vsRackName);
    }

void LocalMember::setMachineName(String::View vsMachineName)
    {
    initialize(f_vsMachineName, vsMachineName);
    }

void LocalMember::setProcessName(String::View vsProcessName)
    {
    initialize(f_vsProcessName, vsProcessName);
    }

void LocalMember::setMemberName(String::View vsMemberName)
    {
    initialize(f_vsMemberName, vsMemberName);
    }

void LocalMember::setRoleName(String::View vsRoleName)
    {
    initialize(f_vsRoleName, vsRoleName);
    }

String::View LocalMember::getLocationInfo() const
    {
    String::View vsLocationInfo;

    String::View vsSiteName = getSiteName();
    if (vsSiteName != NULL && !vsSiteName->isEmpty())
        {
        vsLocationInfo = COH_TO_STRING(",site:" << vsSiteName);
        }
    else
        {
        // COH-16347
        vsLocationInfo = StringHelper::getEmptyString();
        }

    String::View vsRackName = getRackName();
    if (vsRackName != NULL && !vsRackName->isEmpty())
        {
        vsLocationInfo = COH_TO_STRING(vsLocationInfo << ",rack:" << vsRackName);
        }

    String::View vsMachineName = getMachineName();
    if (vsMachineName != NULL && !vsMachineName->isEmpty())
        {
        vsLocationInfo = COH_TO_STRING(vsLocationInfo << ",machine:" << vsMachineName);
        }

    String::View vsProcessName = getProcessName();
    if (vsProcessName != NULL && !vsProcessName->isEmpty())
        {
        vsLocationInfo = COH_TO_STRING(vsLocationInfo << ",process:" << vsProcessName);
        }

    String::View vsMemberName = getMemberName();
    if (vsMemberName != NULL && !vsMemberName->isEmpty())
        {
        vsLocationInfo = COH_TO_STRING(vsLocationInfo << ",member:" << vsMemberName);
        }

    return vsLocationInfo == NULL || vsLocationInfo->isEmpty() ? String::null_string : vsLocationInfo->substring(1);
    }

TypedHandle<const String> LocalMember::toString() const
    {
    String::View vs = "Member(";

    String::View vsLocation = getLocationInfo();
    if (vsLocation != NULL && !vsLocation->isEmpty())
        {
        vs = COH_TO_STRING(vs << "Location=" << vsLocation);
        }

    String::View vsRole = getRoleName();
    if (vsRole != NULL && !vsRole->isEmpty())
        {
        vs = COH_TO_STRING(vs << ", Role=" << vsRole);
        }

    return COH_TO_STRING(vs << ')');
    }

COH_CLOSE_NAMESPACE2
