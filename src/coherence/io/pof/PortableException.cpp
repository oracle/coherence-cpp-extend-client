/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/PortableException.hpp"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/io/pof/ThrowablePofSerializer.hpp"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/Iterator.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::util::ArrayList;
using coherence::util::Collection;
using coherence::util::Iterator;

COH_OPEN_NAMESPACE_ANON(PortableException)

// ----- file local helpers -------------------------------------------------

class ForeignStackTraceElement
    : public class_spec<ForeignStackTraceElement,
        extends<Object>,
        implements<StackTraceElement> >
    {
    friend class factory<ForeignStackTraceElement>;

    protected:
        ForeignStackTraceElement(String::View vsFrame)
            : f_vsFrame(self(), vsFrame)
        {
        }

    public:
        virtual TypedHandle<const String> toString() const
            {
            return COH_TO_STRING(f_vsFrame);
            }

    private:
        FinalView<String> f_vsFrame;
    };

COH_CLOSE_NAMESPACE_ANON

// ----- constructors -------------------------------------------------------

PortableException::PortableException(String::View vsMessage,
        Exception::View ve)
    : super(vsMessage, ve), f_vsName(self(), NULL, /*fMutable*/ true), f_vaStackRemote(self())
    {
    }

PortableException::PortableException(const PortableException& that)
    : super(that), f_vsName(self(), that.f_vsName, /*fMutable*/ true),
      f_vaStackRemote(self(), that.f_vaStackRemote)
    {
    }


// ----- PortableObject methods ---------------------------------------------

void PortableException::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vsName, hIn->readString(0));
    initialize(f_vsMessage, hIn->readString(1));

    Collection::View    vCollStringStack = hIn->readCollection(2,
            ArrayList::create());
    ObjectArray::Handle haStack          = ObjectArray::create(
            vCollStringStack->size());
    Iterator::Handle    hIter            = vCollStringStack->iterator();

    for (size32_t i = 0, c = haStack->length; i < c; ++i)
        {
        haStack[i] = ForeignStackTraceElement::create(
                cast<String::View>(hIter->next()));
        }

    initialize(f_vaStackRemote, haStack);
    initialize(f_veCause, cast<Exception::View>(hIn->readObject(3)));
    }

void PortableException::writeExternal(PofWriter::Handle hOut) const
    {
    ThrowablePofSerializer::writeThrowable(hOut, this);
    }


// ----- Exception methods --------------------------------------------------

String::View PortableException::getName() const
    {
    String::View vsName = f_vsName;

    if (vsName == NULL)
        {
        initialize(f_vsName, Class::getClassName(this));
        }
    else if (typeid(*this) == typeid(PortableException))
        {
        String::View vsPrefix = "Portable(";
        if (vsName->startsWith(vsPrefix))
            {
            return vsName;
            }
        return COH_TO_STRING("Portable(" << vsName << ")");
        }

    return super::getName();
    }

ObjectArray::View PortableException::getStackTrace() const
    {
    ObjectArray::View vaStackRemote = f_vaStackRemote;

    if (vaStackRemote == NULL)
        {
        return super::getStackTrace();
        }

    ObjectArray::View   vaStackLocal = super::getStackTrace();
    size32_t            cLocal       = NULL == vaStackLocal ? 0 : vaStackLocal->length;
    size32_t            cRemote      = vaStackRemote->length;
    ObjectArray::Handle haStackFull  = ObjectArray::create(cRemote + cLocal + 1);

    ObjectArray::copy(vaStackRemote, 0, haStackFull, 0, cRemote);

    haStackFull[cRemote] = ForeignStackTraceElement::create("at <process boundary>");

    if (cLocal > 0)
        {
        ObjectArray::copy(vaStackLocal, 0, haStackFull, cRemote + 1, cLocal);
        }

    return haStackFull;
    }

COH_CLOSE_NAMESPACE3
