/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/run/xml/SimpleElement.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/Map.hpp"

#include "private/coherence/run/xml/XmlHelper.hpp"
#include "private/coherence/run/xml/ViewOnlyElement.hpp"
#include "private/coherence/util/StringHelper.hpp"

#include <sstream>

COH_OPEN_NAMESPACE3(coherence,run,xml)

using coherence::util::ArrayList;
using coherence::util::HashMap;
using coherence::util::Iterator;
using coherence::util::Map;
using coherence::util::StringHelper;

COH_REGISTER_PORTABLE_CLASS(501, SimpleElement);

COH_OPEN_NAMESPACE_ANON(SimpleElement)

// ----- local class: ElementIterator ---------------------------------------

class ElementIterator
    : public class_spec<ElementIterator,
        extends<Object>,
        implements<Iterator> >
    {
    friend class factory<ElementIterator>;

    // ----- state enum ---------------------------------------------

    protected:
        /**
        * Iterator state.
        */
        enum State { FOUND, RETURNED };

    // ----- constructors -------------------------------------------

    protected:
        /**
        * Create an ElementIterator.
        *
        * @param hIter   iterator of elements
        * @param vsName  name of elements to return
        * @param fView   flag indicating View
        */
        ElementIterator(Iterator::Handle hIter, String::View vsName, bool fView)
            : f_hIter(self(), hIter, true),
              f_vsName(self(), vsName),
              m_ohElement(self()),
              m_state(RETURNED),
              m_fView(fView)
            {
            }

    // ----- Iterator interface -------------------------------------

    public:
        virtual bool hasNext() const
            {
            if (m_state == FOUND)
                {
                return true;
                }

            while (f_hIter->hasNext())
                {
                XmlElement::Holder ohElement =
                        cast<XmlElement::Holder>(f_hIter->next());
                if (ohElement->getName()->equals(f_vsName))
                    {
                    m_ohElement = ohElement;
                    m_state     = FOUND;
                    return true;
                    }
                }
            return false;
            }

        virtual Object::Holder next()
            {
            switch (m_state)
                {
                case RETURNED:
                if (!hasNext())
                    {
                    COH_THROW (NoSuchElementException::create());
                    }
                    // fall through
                case FOUND:
                    m_state = RETURNED;
                    if (m_fView)
                        {
                        return (Object::View) m_ohElement;
                        }
                    return m_ohElement;
                default:
                    COH_THROW (IllegalStateException::create());
                }
            }

    // ----- data members -------------------------------------------

    protected:
        /**
        * Iterator of Elements
        */
        mutable FinalHandle<Iterator> f_hIter;

        /**
        * Name of element
        */
        FinalView<String> f_vsName;

       /**
       * The next element to return
       */
       mutable MemberHolder<XmlElement> m_ohElement;

       /**
       * Iterator state
       */
       mutable State m_state;

       /**
       * Viewer
       */
       bool m_fView;
    };

COH_CLOSE_NAMESPACE_ANON

// ----- constructors -------------------------------------------------------

SimpleElement::SimpleElement(String::View vsName)
    : m_vsName(self(), vsName),
      m_vsComment(self(), NULL),
      m_hListElements(self(), ArrayList::create()),
      m_hMapAttributes(self(), HashMap::create())
    {
    }

SimpleElement::SimpleElement(String::View vsName, String::View vsValue)
    : super(vsValue, false),
      m_vsName(self(), vsName),
      m_vsComment(self(), NULL),
      m_hListElements(self(), ArrayList::create()),
      m_hMapAttributes(self(), HashMap::create())
    {
    }

SimpleElement::SimpleElement(String::View vsName, int32_t nValue)
    : super(nValue, false),
      m_vsName(self(), vsName),
      m_vsComment(self(), NULL),
      m_hListElements(self(), ArrayList::create()),
      m_hMapAttributes(self(), HashMap::create())
    {
    }

SimpleElement::SimpleElement(String::View vsName, bool fValue)
    : super(fValue, false),
      m_vsName(self(), vsName),
      m_vsComment(self(), NULL),
      m_hListElements(self(), ArrayList::create()),
      m_hMapAttributes(self(), HashMap::create())
    {
    }

SimpleElement::SimpleElement(const SimpleElement& that)
    : super(that),
      m_vsName(self(), that.m_vsName),
      m_vsComment(self(), that.m_vsComment),
      m_hListElements(self(), cast<ArrayList::Handle>(
            that.m_hListElements->clone())),
      m_hMapAttributes(self(), cast<HashMap::Handle>(
            that.m_hMapAttributes->clone()))
    {
    }


// ----- SimpleElement interface --------------------------------------------

XmlValue::Handle SimpleElement::addAttribute(String::View vsName)
    {
    XmlValue::Handle hVal = getAttribute(vsName);
    if (NULL == hVal)
        {
        hVal = SimpleValue::create(StringHelper::getEmptyString(), true);
        setAttribute(vsName, hVal);
        }
    return hVal;
    }

void SimpleElement::setAttribute(String::View vsName, XmlValue::Handle hValue)
    {
    HashMap::Handle hMap = m_hMapAttributes;
    if (NULL == hValue)
        {
        hMap->remove(vsName);
        }
    else
        {
        hMap->put(vsName, hValue);
        }
    }

XmlValue::Handle SimpleElement::getAttribute(String::View vsName) const
    {
    COH_ENSURE_PARAM(vsName);
    return cast<XmlValue::Handle>(m_hMapAttributes->get(vsName));
    }

HashMap::Handle SimpleElement::getAttributeMap()
    {
    return m_hMapAttributes;
    }

XmlValue::Handle SimpleElement::getSafeAttribute(String::View vsName) const
    {
    XmlValue::Handle hValue = getAttribute(vsName);
    if (NULL == hValue)
        {
        SimpleValue::Handle hSimpleValue = SimpleValue::create(StringHelper::getEmptyString(), true);
        // TODO: unable to setParent() as "this" is a View.
        // hSimpleValue->setParent(this);
        // Should this method return a View?
        // hSimpleValue->setMutable(false);
        return hSimpleValue;
        }

    return hValue;
    }

HashMap::View SimpleElement::getAttributeMap() const
    {
    HashMap::View vMap      = m_hMapAttributes;
    HashMap::Handle hNewMap = HashMap::create();

    for (Iterator::Handle hIter = vMap->entrySet()->iterator();
          hIter->hasNext();)
        {
        Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
        hNewMap->put(vEntry->getKey(),
                cast<XmlValue::View>(vEntry->getValue()));
        }
    return hNewMap;
    }

XmlElement::Handle SimpleElement::addElement(String::View vsName)
    {
    if (NULL == vsName)
        {
        COH_THROW (IllegalArgumentException::create(
                "Element name can't be NULL"));
        }

    XmlElement::Handle hElement = SimpleElement::create(vsName);
    hElement->setParent(this);
    m_hListElements->add(hElement);

    return hElement;
    }

template<class T, class PT> T find(PT that, String::View vsPath)
    {
    T hElement = that;
    if (NULL == vsPath || vsPath->length() == 0)
        {
        COH_THROW (IllegalArgumentException::create(
            "Path must not be NULL or an empty string"));
        }

    if (vsPath->getCString()[0] == '/')
        {
        hElement = that->getRoot();
        }

    ObjectArray::Handle oaTokens = StringHelper::split(vsPath, "/");

    for (size32_t i = 0; i < oaTokens->length && hElement != NULL; ++i)
        {
        String::View vsToken = cast<String::View>(oaTokens[i]);
        // if path starts with '/', first token will be ""
        if (vsToken->length() == 0)
            {
            continue;
            }

        if (vsToken->equals(".."))
            {
            hElement = hElement->getParent();
            if (NULL == hElement)
                {
                std::stringstream ss;
                ss << "Invalid Path: " << vsPath;
                COH_THROW (IllegalArgumentException::create(ss.str()));
                }
            }
        else
            {
            hElement = hElement->getElement(vsToken);
            }
        }
    return hElement;
    }

XmlElement::Handle SimpleElement::findElement(String::View vsPath)
    {
    return find<XmlElement::Handle, SimpleElement*>(this, vsPath);
    }

XmlElement::View SimpleElement::findElement(String::View vsPath) const
    {
    return find<XmlElement::View, const SimpleElement*>(this, vsPath);
    }

String::View SimpleElement::getAbsolutePath() const
    {
    std::stringstream ss;
    SimpleElement::View vParent = cast<SimpleElement::View>(getParent());

    if (NULL != vParent)
        {
        ss << vParent->getAbsolutePath();
        }

        ss << "/" << getName();
        return ss.str();
    }

String::View SimpleElement::getComment() const
    {
    return m_vsComment;
    }

XmlElement::Handle SimpleElement::getElement(String::View vsName)
    {
    return cast<XmlElement::Handle>(getElementInternal(vsName));
    }

XmlElement::View SimpleElement::getElement(String::View vsName) const
    {
    return cast<XmlElement::View>(getElementInternal(vsName));
    }

Iterator::Handle SimpleElement::getElements(String::View vsName)
    {
    return ElementIterator::create(m_hListElements->iterator(), vsName, false);
    }

Iterator::Handle SimpleElement::getElements(String::View vsName) const
    {
    return ElementIterator::create(m_hListElements->iterator(), vsName, true);
    }

XmlElement::View SimpleElement::getSafeElement(String::View vsPath) const
    {
    COH_ENSURE(NULL != vsPath);

    if (vsPath->length() == 0)
        {
        return this;
        }

    if (vsPath->getCString()[0] == '/')
        {
        return getRoot()->getSafeElement(vsPath->substring(1));
        }

    // get the first name from the path
    size32_t of = vsPath->indexOf('/');
    String::View vsName;
    String::View vsRemain;
    if (String::npos == of)
        {
        vsName   = vsPath;
        vsRemain = NULL;
        }
    else
        {
        vsName   = vsPath->substring(0, of);
        vsRemain = vsPath->substring(of+1);
        }

    // check if going "up" (..) or "down" (child name)
    XmlElement::View vElement;
    if (vsName->equals(".."))
        {
        vElement = cast<SimpleElement::View>(getParent());
        if (NULL == vElement)
            {
            COH_THROW_STREAM(IllegalArgumentException, "Invalid path " << vsPath);
            }
        }
    else
        {
        vElement = getElement(vsName);
        if (NULL == vElement)
            {
            // create a temporary "safe" element (read-only)
            // parent (this) does not know its new safe child (element)
            // because this is a "read-only" operation; however, the child
            // does know its parent so it can answer pathed questions etc.
            vElement = ViewOnlyElement::create(this, vsName);
            }
        }

    return NULL == vsRemain ? vElement : vElement->getSafeElement(vsRemain);
    }

Object::Holder SimpleElement::getElementInternal(String::View vsName) const
    {
    if (m_hListElements->isEmpty())
        {
        return (SimpleElement::Handle) NULL;
        }
    else
        {
        for (Iterator::Handle i = m_hListElements->iterator(); i->hasNext(); )
            {
            XmlElement::Handle hElement = cast<XmlElement::Handle>(i->next());
            if (NULL != hElement)
                {
                String::View vsElementName = hElement->getName();
                if (Object::equals(vsElementName, vsName))
                    {
                    return hElement;
                    }
                }
            }
        return (SimpleElement::Handle) NULL;
        }
    }

ObjectArray::Handle SimpleElement::getAllElements() const
    {
    return m_hListElements->toArray();
    }

List::Handle SimpleElement::getElementList()
    {
    return m_hListElements;
    }

List::View SimpleElement::getElementList() const
    {
    List::View   vList    = m_hListElements;
    List::Handle hNewList = ArrayList::create();

    for (Iterator::Handle hIter = vList->listIterator(); hIter->hasNext();)
        {
        hNewList->add(cast<XmlElement::View>(hIter->next()));
        }
    return hNewList;
    }

XmlElement::Handle SimpleElement::ensureElement(String::View vsPath)
    {
    XmlElement::Handle hElement = this;
    if (NULL == vsPath)
        {
        COH_THROW (IllegalArgumentException::create(
            "Path must not be NULL or an empty string"));
        }

    if (vsPath->getCString()[0] == '/')
        {
        hElement = getRoot();
        }

    ObjectArray::Handle oaTokens = StringHelper::split(vsPath, "/");

    for (size32_t i = 0; i < oaTokens->length; ++i)
        {
        String::View vsToken = cast<String::View>(oaTokens[i]);
        // if path starts with '/', first token will be ""
        if (vsToken->length() == 0)
            {
            continue;
            }
        if (vsToken->equals(".."))
            {
            hElement = cast<XmlElement::Handle>(hElement->getParent());
            if (NULL == hElement)
                {
                std::stringstream ss;
                ss << "Invalid Path: " << vsPath;
                COH_THROW (IllegalArgumentException::create(ss.str()));
                }
            }
        else
            {
            XmlElement::Handle hChild = hElement->getElement(vsToken);
            hElement = (NULL == hChild) ?
                    hElement->addElement(vsToken) : hChild;
            }
        }
    return hElement;
    }

String::View SimpleElement::getName() const
    {
    return m_vsName;
    }

XmlElement::Handle SimpleElement::getRoot()
    {
    XmlElement::Handle hParent = getParent();

    return NULL == hParent ? this : hParent->getRoot();
    }

XmlElement::View SimpleElement::getRoot() const
    {
    SimpleElement::View vParent = cast<SimpleElement::View>(getParent());

    return NULL == vParent ? this : vParent->getRoot();
    }

void SimpleElement::setComment(String::View vsComment)
    {
    if (NULL != vsComment && vsComment->length() == 0)
        {
        vsComment = NULL;
        }
    else if (vsComment->indexOf("--") != String::npos)
        {
        COH_THROW (IllegalArgumentException::create(
            "comment contains \"--\"; see XML 1.0 2ed section 2.5 [15]"));
        }
    m_vsComment = vsComment;
    }

void SimpleElement::setName(String::View vsName)
    {
    if (NULL != vsName && vsName->length() == 0)
        {
        vsName = NULL;
        }
    m_vsName = vsName;
    }

String::View SimpleElement::formatXml(bool fPretty, size32_t nIndent) const
    {
    std::stringstream out;
    String::View vsComment = getComment();
    bool         fComment  = NULL != vsComment && vsComment->length() > 0;
    bool         fValue    = !isEmpty();
    bool         fChildren = getAllElements()->length > 0;

    if (!fComment && !fValue && !fChildren)
        {
        writeEmptyTag(out);
        }
    else if (!fChildren)
        {
        writeStartTag(out);
        writeComment(out, fPretty);
        writeValue(out);
        writeChildren(out, fPretty);
        writeEndTag(out);
        }
    else
        {
        writeStartTag(out);
        if (fPretty)
            {
            XmlHelper::writeNewLineAndIndent(out, nIndent);
            }

        if (fComment)
            {
            writeComment(out, fPretty, nIndent);
            if (fPretty)
                {
                XmlHelper::writeNewLineAndIndent(out, nIndent);
                }
            }

        if (fValue)
            {
            writeValue(out);
            if (fPretty)
                {
                XmlHelper::writeNewLineAndIndent(out, nIndent);
                }
            }

        writeChildren(out, fPretty, nIndent);
        if (fPretty)
            {
            XmlHelper::writeNewLineAndIndent(out, nIndent);
            }

        out.flush();

        writeEndTag(out);
        }

    if (NULL == getParent())
        {
        out.flush();
        }

    return String::create(out.str());
    }

void SimpleElement::writeEmptyTag(std::ostream& out) const
    {
    out << '<' << getName();
    writeAttributes(out);
    out << "/>";
    }

void SimpleElement::writeStartTag(std::ostream& out) const
    {
    out << '<' << getName();
    writeAttributes(out);
    out << '>';
    }

void SimpleElement::writeEndTag(std::ostream& out) const
    {
    out << "</" << getName() << ">";
    }

void SimpleElement::writeAttributes(std::ostream& out) const
    {
    for (Iterator::Handle hIter = m_hMapAttributes->entrySet()->iterator();
          hIter->hasNext(); )
        {
        Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
        out << ' ' << vEntry->getKey() << '=';
        cast<XmlValue::View>(vEntry->getValue())->writeValue(out);
        }
    }

void SimpleElement::writeChildren(std::ostream& out, bool fPretty,
        size32_t nIndent) const
    {
    for (Iterator::Handle hIter = m_hListElements->iterator();
            hIter->hasNext(); )
        {
        cast<XmlElement::View>(hIter->next())-> writeXml(out, fPretty,
                nIndent + 2);
        }
    }

void SimpleElement::writeComment(std::ostream& out, bool fPretty,
        size32_t nIndent) const
    {
    String::View vsComment = getComment();
    if (NULL != vsComment && vsComment->length())
        {
        out << "<!--" << (fPretty ? XmlHelper::breakLines(vsComment, 78,
                nIndent) : vsComment) << "-->";
        }
    }


// ----- PortableObject interface -------------------------------------------

void SimpleElement::readExternal(PofReader::Handle hIn)
    {
    if (m_vsName != NULL)
        {
        COH_THROW (IllegalArgumentException::create(""));
        }

    super::readExternal(hIn);

    // element name
    if (hIn->readBoolean(4))
        {
        m_vsName = hIn->readString(5);
        }

    // child elements
    if (hIn->readBoolean(6))
        {
        hIn->readCollection(7, m_hListElements = ArrayList::create());
        }

    // attributes
    if (hIn->readBoolean(8))
        {
        hIn->readMap(9, m_hMapAttributes = HashMap::create());
        }

    // element comment
    if (hIn->readBoolean(10))
        {
        m_vsComment = hIn->readString(11);
        }
    }

void SimpleElement::writeExternal(PofWriter::Handle hOut) const
    {
    super::writeExternal(hOut);

    // element name
    String::View vsName = m_vsName;
    bool fName = vsName != NULL;
    hOut->writeBoolean(4, fName);
    if (fName)
        {
        hOut->writeString(5, vsName);
        }

    // child elements
    ArrayList::View hListKids = cast<ArrayList::View>(m_hListElements);
    bool fKids = hListKids != NULL && !hListKids->isEmpty();
    hOut->writeBoolean(6, fKids);
    if (fKids)
        {
        hOut->writeCollection(7, hListKids);
        }

    // attributes
    HashMap::View hMapAttr = cast<HashMap::View>(m_hMapAttributes);
    bool fAttr = hMapAttr != NULL && !hMapAttr->isEmpty();
    hOut->writeBoolean(8, fAttr);
    if (fAttr)
        {
        hOut->writeMap(9, hMapAttr);
        }

    // element comment
    String::View vsComment = m_vsComment;
    bool fComment = vsComment != NULL;
    hOut->writeBoolean(10, fComment);
    if (fComment)
        {
        hOut->writeString(11, vsComment);
        }
    }

// ----- Object interface ---------------------------------------------------

TypedHandle<const String> SimpleElement::toString() const
    {
    return formatXml(true);
    }

size32_t SimpleElement::hashCode() const
    {
    size32_t nHash = SimpleValue::hashCode();

    String::View vsName    = getName();
    String::View vsComment = getComment();
    nHash = 37 * nHash + Object::hashCode(vsName);
    nHash = 37 * nHash + Object::hashCode(vsComment);
    nHash = 37 * nHash + m_hListElements->hashCode();

    for (Iterator::Handle hIter = m_hMapAttributes->entrySet()->iterator();
          hIter->hasNext(); )
        {
        Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
        Object::Holder oh = vEntry->getKey();
        nHash = 37 * nHash + Object::hashCode(oh);

        oh = vEntry->getValue();
        nHash = 37 * nHash + Object::hashCode(oh);
        }

    return nHash;
    }

bool SimpleElement::equals(Object::View v) const
    {
    SimpleElement::View vThat = cast<SimpleElement::View>(v);
    if (NULL == vThat)
        {
        return false;
        }
    else if (((SimpleElement::View) this) == vThat)
        {
        return true;
        }
    else if (! SimpleValue::equals(v))
        {
        return false;
        }

    if (!Object::equals(getComment(), vThat->getComment()) ||
        !Object::equals(getName(),    vThat->getName()))
        {
        return false;
        }

    HashMap::View vMap = m_hMapAttributes;
    if (vMap->size() != vThat->m_hMapAttributes->size())
        {
        return false;
        }

    if (!vMap->entrySet()->equals(vThat->m_hMapAttributes->entrySet()))
        {
        return false;
        }

    List::View vList = m_hListElements;
    if (vList->size() != vThat->m_hListElements->size())
        {
        return false;
        }

    for (Iterator::Handle hIter = vList->iterator(),
            hThatIter = vThat->m_hListElements->iterator(); hIter->hasNext();)
        {
        if (!hIter->next()->equals(hThatIter->next()))
            {
            return false;
            }
        }
    return true;
    }

COH_CLOSE_NAMESPACE3
