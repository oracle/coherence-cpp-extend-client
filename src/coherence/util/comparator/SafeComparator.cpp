/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/comparator/SafeComparator.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/extractor/KeyExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,comparator)

using coherence::util::extractor::KeyExtractor;

COH_REGISTER_PORTABLE_CLASS(43, SafeComparator);


// ----- constructors -------------------------------------------------------

SafeComparator::SafeComparator()
    : f_vComparator(self())
    {
    }


SafeComparator::SafeComparator(Comparator::View vComparator)
    : f_vComparator(self(), vComparator)
    {
    }


// ----- Comparator interface -----------------------------------------------

int32_t SafeComparator::compare(Object::View vO1, Object::View vO2) const
    {
    return compareSafe(getComparator(), vO1, vO2);
    }


// ----- QueryMap interface -------------------------------------------------

int32_t SafeComparator::compareEntries(QueryMap::Entry::View vEntry1,
                    QueryMap::Entry::View vEntry2) const
    {

    Comparator::View         vComparator   = getComparator();
    QueryMapComparator::View vQMComparator = cast<QueryMapComparator::View>(
                vComparator, false);
    return (vComparator != NULL)
        ? vQMComparator->compareEntries(vEntry1, vEntry2)
        : compareSafe(vComparator, vEntry1->getValue(), vEntry2->getValue());
    }


// ----- EntryAwareComparator interface -------------------------------------

bool SafeComparator::isKeyComparator() const
    {
    return isKeyComparator(getComparator());
    }


bool SafeComparator::isKeyComparator(Comparator::View vComparator)
    {
    return instanceof<KeyExtractor::View>(vComparator) ||
        (instanceof<EntryAwareComparator::View>(vComparator) &&
        cast<EntryAwareComparator::View>(vComparator)->isKeyComparator());
    }


// ----- PortableObject interface -------------------------------------------

void SafeComparator::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vComparator, cast<Comparator::View>(hIn->readObject(0)));
    }

void SafeComparator:: writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, f_vComparator);
    }


// ----- Object interface ---------------------------------------------------

bool SafeComparator::equals(Object::View v) const
    {
    SafeComparator::View that = cast<SafeComparator::View>(v);
    if (NULL == that)
        {
        return false;
        }
    if (f_vComparator == NULL)
        {
        return that->getComparator() == NULL;
        }
    return f_vComparator->equals(that->getComparator());
    }

size32_t SafeComparator::hashCode() const
    {
    Comparator::View vComparator = getComparator();
    return vComparator == NULL ? 17 : vComparator->hashCode();
    }

TypedHandle<const String> SafeComparator::toString() const
    {
    return COH_TO_STRING(Class::getClassName(this)
        << ": ("
        << getComparator()
        << ")");
    }


// ----- data members accessor-----------------------------------------------

Comparator::View SafeComparator::getComparator() const
    {
    return f_vComparator;
    }


// ----- helpers ------------------------------------------------------------

int32_t SafeComparator::compareSafe(Comparator::View vComparator,
                Object::View vO1, Object::View vO2)
        {
        if (vComparator != NULL)
            {
            try
                {
                return vComparator->compare(vO1, vO2);
                }
            catch (NullPointerException&)
                {
                }
            }


        if (vO1 == NULL)
            {
            return vO2 == NULL ? 0 : -1;
            }

        if (vO2 == NULL)
            {
             return +1;
            }

        Comparable::View vComp1 = cast<Comparable::View>(vO1);
        Comparable::View vComp2 = cast<Comparable::View>(vO2);
        return vComp1->compareTo(vComp2);
        }


// ---- constants -----------------------------------------------------------

SafeComparator::Handle SafeComparator::getInstance()
    {
    static FinalHandle<SafeComparator> hSC(System::common(),
            SafeComparator::create());
    return hSC;
    }
COH_STATIC_INIT(SafeComparator::getInstance());

COH_CLOSE_NAMESPACE3
