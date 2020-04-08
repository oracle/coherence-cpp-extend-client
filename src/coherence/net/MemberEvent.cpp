/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/MemberEvent.hpp"

#include "coherence/net/MemberListener.hpp"
#include "coherence/net/Service.hpp"

COH_OPEN_NAMESPACE2(coherence,net)


// ----- constructors -------------------------------------------------------

MemberEvent::MemberEvent(MemberEvent::ServiceView vService,
        Id identity, Member::View vMember)
    : super(vService), m_id(identity), f_vMember(self(), vMember)
    {
    }


// ----- vService interface -------------------------------------------------

void MemberEvent::dispatch(Listeners::View vListeners) const
    {
    if (NULL != vListeners)
        {
        ObjectArray::View vaListener = vListeners->listeners();

        for (size32_t i = vaListener->length; i-- > 0; )
            {
            MemberListener::Handle hListener =
                    cast<MemberListener::Handle>(vaListener[i]);

            switch (getId())
                {
                case MemberEvent::member_joined:
                    hListener->memberJoined(this);
                    break;

                case MemberEvent::member_leaving:
                    hListener->memberLeaving(this);
                    break;

                case MemberEvent::member_left:
                    hListener->memberLeft(this);
                    break;
                }
            }
        }
    }

bool MemberEvent::isLocal() const
    {
    return true;
    }


// ----- Object interface ---------------------------------------------------

static const size32_t COH_ID_TEXT_LENGTH = 4;
static const char*  COH_ID_TEXT[COH_ID_TEXT_LENGTH] =
    {
    "<unknown>",
    "JOINED",
    "LEAVING",
    "LEFT"
    };

TypedHandle<const String> MemberEvent::toString() const
    {
    return COH_TO_STRING("MemberEvent{"
        << getMember()
        << ' '
        << COH_ID_TEXT[getId()]
        << ' '
        << getService()
        << '}');
    }


// ----- accessors ----------------------------------------------------------

MemberEvent::Id MemberEvent::getId() const
    {
    return m_id;
    }

MemberEvent::ServiceView MemberEvent::getService() const
    {
    return cast<Service::View>(getSource());
    }

Member::View MemberEvent::getMember() const
    {
    return f_vMember;
    }

COH_CLOSE_NAMESPACE2
