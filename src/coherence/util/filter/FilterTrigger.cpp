/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/FilterTrigger.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

#include "private/coherence/util/InvocableMapHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

COH_REGISTER_PORTABLE_CLASS(89, FilterTrigger);

// ----- constants ----------------------------------------------------------

COH_EXPORT_SPEC_MEMBER(const int32_t FilterTrigger::action_rollback)
COH_EXPORT_SPEC_MEMBER(const int32_t FilterTrigger::action_ignore)
COH_EXPORT_SPEC_MEMBER(const int32_t FilterTrigger::action_remove)


// ----- constructors -------------------------------------------------------

FilterTrigger::FilterTrigger()
    : f_vFilter(self()), m_nAction(action_rollback)
    {
    }

FilterTrigger::FilterTrigger(Filter::View vFilter, int32_t nAction)
    : f_vFilter(self(), vFilter), m_nAction(nAction)
    {
    }


// ----- Filter interface ---------------------------------------------------

void FilterTrigger::process(MapTrigger::Entry::Handle vEntry)
    {
    if (vEntry->isPresent()
        && !InvocableMapHelper::evaluateEntry(f_vFilter, vEntry))
        {
        switch (m_nAction)
            {
             case action_rollback:
             default:
                COH_THROW_STREAM(IllegalArgumentException, "Rejecting " <<
                        vEntry << " by trigger " << *this);

            case action_ignore:
                {
                Object::View vValue = vEntry->getOriginalValue();
                if (vValue != NULL || vEntry->isOriginalPresent())
                    {
                    vEntry->setValue(vValue, true);
                    }
                else
                    {
                    vEntry->remove(true);
                    }
                break;
                }

            case action_remove:
                vEntry->remove(true);
                break;
            }
        }
    }


// ----- PortableObject interface -------------------------------------------

void FilterTrigger::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vFilter, cast<Filter::View>(hIn->readObject(0)));
    m_nAction   = hIn->readInt32(1);
    }

void FilterTrigger:: writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, f_vFilter);
    hOut->writeInt32(1,m_nAction);
    }


// ----- Object interface ---------------------------------------------------

bool FilterTrigger::equals(Object::View v) const
    {
    FilterTrigger::View that = cast<FilterTrigger::View>(v, false);
    if (NULL != that)
        {
        return f_vFilter->equals(that->f_vFilter)
              && m_nAction == that->m_nAction;
        }
    return false;
    }

size32_t FilterTrigger::hashCode() const
    {
    return f_vFilter->hashCode();
    }

TypedHandle<const String> FilterTrigger::toString() const
    {
    String::View vsClass = Class::getClassName(this);
    String::View vsAction;
    switch (m_nAction)
        {
        default:
        case action_rollback:
            vsAction = String::create("action_rollback");
            break;

        case action_ignore:
            vsAction = String::create("action_ignore");
            break;

        case action_remove:
            vsAction = String::create("action_remove");
            break;
        }
    return COH_TO_STRING(vsClass->substring(vsClass->lastIndexOf(':') + 1)
            << '(' << f_vFilter << ", " << vsAction << ')');
    }


// ----- data members accessor-----------------------------------------------

Filter::View FilterTrigger::getFilter() const
    {
    return f_vFilter;
    }

int32_t FilterTrigger::getAction() const
    {
    return m_nAction;
    }

COH_CLOSE_NAMESPACE3
