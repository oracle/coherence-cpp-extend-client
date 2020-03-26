/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/LikeFilter.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/LinkedList.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/LinkedList.hpp"
#include "coherence/util/MapIndex.hpp"
#include "coherence/util/SortedMap.hpp"


#include "private/coherence/util/StringHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

COH_REGISTER_PORTABLE_CLASS(77, LikeFilter);

namespace
    {
    // ---- constants -------------------------------------------------------

    /**
    * Non-optimized plan with support for trailing data.
    */
    const int32_t ITERATIVE_EVAL       = 0;

    /**
    * Optimized plan: The pattern is anything that starts with a specific
    * character ("x%").
    */
    const int32_t STARTS_WITH_CHAR     = 1;

    /**
    * Optimized plan: The pattern is anything that starts with a specific
    * string ("xyz%").
    */
    const int32_t STARTS_WITH_STRING   = 2;

    /**
    * Optimized plan: The pattern is anything that starts with a specific
    * (but case-insensitive) string ("xyz%").
    */
    const int32_t STARTS_WITH_INSENS   = 3;

    /**
    * Optimized plan: The pattern is anything that ends with a specific
    * character ("%x").
    */
    const int32_t ENDS_WITH_CHAR       = 4;

    /**
    * Optimized plan: The pattern is anything that ends with a specific
    * string ("%xyz").
    */
    const int32_t ENDS_WITH_STRING     = 5;

    /**
    * Optimized plan: The pattern is anything that ends with a specific
    * (but case-insensitive) string ("%xyz").
    */
    const int32_t ENDS_WITH_INSENS     = 6;

    /**
    * Optimized plan: The pattern is anything that contains a specific
    * character ("%x%").
    */
    const int32_t CONTAINS_CHAR        = 7;

    /**
    * Optimized plan: The pattern is anything that contains a specific
    * string ("%xyz%").
    */
    static const int32_t CONTAINS_STRING      = 8;

    /**
    * Optimized plan: Everyting matches ("%").
    */
    const int32_t ALWAYS_TRUE          = 9;

    /**
    * Optimized plan: Nothing matches (NULL).
    */
    const int32_t ALWAYS_FALSE         = 10;

    /**
    * Optimized plan: Exact match ("xyz").
    */
    const int32_t EXACT_MATCH          = 11;

    /**
    * Optimized plan: Exact case-insensitive match ("xyz").
    */
    const int32_t INSENS_MATCH         = 12;

    ObjectArray::View _createPlanNames()
        {
        ObjectArray::Handle haNames = ObjectArray::create(13);
        haNames[0]  = String::create("iterative evaluation");
        haNames[1]  = String::create("starts-with-character");
        haNames[2]  = String::create("starts-with-string");
        haNames[3]  = String::create("starts-with-string (case-insensitive)");
        haNames[4]  = String::create("ends-with-character");
        haNames[5]  = String::create("ends-with-string");
        haNames[6]  = String::create("ends-with-string (case-insensitive)");
        haNames[7]  = String::create("contains-character");
        haNames[8]  = String::create("contains-string");
        haNames[9]  = String::create("always-true");
        haNames[10] = String::create("always-false");
        haNames[11] = String::create("exact-match");
        haNames[12] = String::create("exact-match (case-insensitive)");

        return haNames;
        }

    ObjectArray::View getPlanNames()
        {
        static FinalView<ObjectArray> vPlanNames(System::common(),
                _createPlanNames());
        return vPlanNames;
        }
    COH_STATIC_INIT(getPlanNames());

    Object::View getANY()
        {
        static FinalView<Object> vANY(System::common(),
                Object::create());
        return vANY;
        }
    COH_STATIC_INIT(getANY());


    // ----- helper functions -----------------------------------------------

    /**
    * Obtain an Array<char> that represents the characters of vsSource.
    *
    * @param vsSource  the String with the desired characters
    *
    * @return the Array<char> from vsSource, not includeing the NUL terminator
    */
    Array<char>::Handle toChar8Array(String::View vsSource)
        {
        Array<octet_t>::View vab  = vsSource->getOctets();
        Array<char>::Handle  hach = Array<char>::create(vsSource->length());

        for (size32_t i = 0, c = hach->length; i < c; ++i)
            {
            hach[i] = (char) vab[i];
            }
        return hach;
        }
    }


// ----- constructors -------------------------------------------------------

LikeFilter::LikeFilter()
        : super(), m_vsPart(self()), m_vStepFront(self()),
          m_vStepBack(self()), m_vaStepMiddle(self()),
          m_fPatternIsASCII(true)
    {
    }

LikeFilter::LikeFilter(ValueExtractor::View vExtractor,
        String::View vsPattern, wchar16_t chEscape, bool fIgnoreCase)
        : super(vExtractor, vsPattern), m_chEscape(chEscape),
          m_fIgnoreCase(fIgnoreCase), m_vsPart(self()), m_vStepFront(self()),
          m_vStepBack(self()), m_vaStepMiddle(self()),
          m_fPatternIsASCII(true)
    {
    init(chEscape, fIgnoreCase);
    }

void LikeFilter::init(wchar16_t chEscape, bool fIgnoreCase)
    {
    m_chEscape    = chEscape;
    m_fIgnoreCase = fIgnoreCase;
    buildPlan();
    }


// ----- ExtractorFilter interface ------------------------------------------

bool LikeFilter::evaluateExtracted(Object::View vExtracted) const
    {
    try
        {
        if (NULL == vExtracted)
            {
            return false;
            }
        else
            {
            return isMatch(COH_TO_STRING(vExtracted));
            }
        }
    catch (ClassCastException::View)
        {
        return false;
        }
    }


// ----- IndexAwareFilter interface -----------------------------------------

size32_t LikeFilter::calculateEffectiveness(Map::View vMapIndexes,
        Set::View vSetKeys) const
    {
    int32_t nPlan = m_nPlan;
    if (nPlan == ALWAYS_FALSE || nPlan == ALWAYS_TRUE)
        {
        return 1;
        }

    MapIndex::View vIndex = cast<MapIndex::View>(vMapIndexes->get(getValueExtractor()));
    if (NULL == vIndex)
        {
        return calculateIteratorEffectiveness(vSetKeys->size());
        }

    if (nPlan == EXACT_MATCH)
        {
        return 1;
        }

    String::View vsPattern = getPattern();
    if (vIndex->isOrdered() && vsPattern->indexOf('%') != 0 && vsPattern->indexOf('_') != 0)
        {
        int32_t nEffectivness = vIndex->getIndexContents()->size() / 4;
        return nEffectivness > 1 ? nEffectivness : 1;
        }
    else
        {
        return vIndex->getIndexContents()->size();
        }
    }

Filter::View LikeFilter::applyIndex(Map::View vMapIndexes,
        Set::Handle hSetKeys) const
    {
    int32_t nPlan = m_nPlan;
    switch (nPlan)
        {
        case ALWAYS_FALSE:
            hSetKeys->clear();
            return NULL;

        case ALWAYS_TRUE:
            return NULL;
        }

    MapIndex::View vIndex = cast<MapIndex::View>(vMapIndexes->get(getValueExtractor()));
    if (NULL == vIndex)
        {
        // there is no relevant index
        return this;
        }

    if (nPlan == EXACT_MATCH)
        {
        Set::View vSetEquals = cast<Set::View>(vIndex->getIndexContents()->get(m_vsPart));
        if (NULL == vSetEquals || vSetEquals->isEmpty())
            {
            hSetKeys->clear();
            }
        else
            {
            hSetKeys->retainAll(vSetEquals);
            }
        return NULL;
        }

    Map::View vMapValues = vIndex->getIndexContents();

    if ((nPlan == STARTS_WITH_STRING || nPlan == STARTS_WITH_CHAR)
            && vIndex->isOrdered())
        {
        try
            {
            String::View vsPrefix = nPlan == STARTS_WITH_STRING ?
                    (String::View) m_vsPart :
                    (String::View) COH_TO_STRING(m_chPart);

            SortedMap::View vMapTail  = cast<SortedMap::View>(vMapValues)->tailMap(vsPrefix);
            Set::Handle     hSetMatch = HashSet::create();
            for (Iterator::Handle hIter = vMapTail->entrySet()->iterator(); hIter->hasNext();)
                {
                Map::Entry::View vEntry  = cast<Map::Entry::View>(hIter->next());
                String::View     vsValue = cast<String::View>(vEntry->getKey());

                if (vsValue->startsWith(vsPrefix))
                    {
                    hSetMatch->addAll(cast<Set::View>(vEntry->getValue()));
                    }
                else
                    {
                    break;
                    }
                }
            hSetKeys->retainAll(hSetMatch);
            return NULL;
            }
        catch (ClassNotFoundException::View)
            {
            // incompatible types; go the long way
            }
        }

    Set::Handle hSetMatch = HashSet::create();
    for (Iterator::Handle hIter = vMapValues->entrySet()->iterator(); hIter->hasNext();)
        {
        Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
        Object::View     vValue = vEntry->getKey();

        String::View sValue = vValue == NULL ?
                (String::View) String::null_string :
                (String::View) COH_TO_STRING(vValue);
        if (isMatch(sValue))
            {
            hSetMatch->addAll(cast<Set::View>(vEntry->getValue()));
            }
        }
    hSetKeys->retainAll(hSetMatch);

    return NULL;
    }

// ----- PortableObject interface -------------------------------------------

void LikeFilter::readExternal(PofReader::Handle hIn)
    {
    super::readExternal(hIn);
    m_fIgnoreCase = hIn->readBoolean(2);
    m_chEscape    = hIn->readChar16(3);
    buildPlan();
    }

void LikeFilter::writeExternal(PofWriter::Handle hOut) const
    {
    super::writeExternal(hOut);
    hOut->writeBoolean(2,m_fIgnoreCase);
    hOut->writeChar16(3, m_chEscape);
    }


// ----- accessors ----------------------------------------------------------

String::View LikeFilter::getPattern() const
    {
    return cast<String::View>(getValue());
    }

bool LikeFilter::isIgnoreCase() const
    {
    return m_fIgnoreCase;
    }

wchar16_t LikeFilter::getEscapeChar() const
    {
    return m_chEscape;
    }

String::View LikeFilter::formatPlan() const
        {
        ObjectArray::View vaPlanNames = getPlanNames();
        String::View vs = COH_TO_STRING("Plan for case-" << (isIgnoreCase() ? "in" : "")
                  << "sensitive LIKE \"" << getPattern() << "\" (escape=\""
                  << getEscapeChar() << "\") is \""
                  << vaPlanNames[m_nPlan] << "\"" << std::endl
                  << "initial step: " << m_vStepFront << std::endl);
        ObjectArray::View vaStep = m_vaStepMiddle;
         if (NULL != vaStep && vaStep->length > 0)
            {
            for (size32_t i = 0, c = vaStep->length; i < c; ++i)
                {
                vs = COH_TO_STRING(vs << "step " << (i+1) << ": "
                          << vaStep[i] << std::endl);
                 }
            }
         return COH_TO_STRING(vs <<"final step: " << m_vStepBack << std::endl << std::endl);
        }


// ----- internal methods ---------------------------------------------------

void LikeFilter::buildPlan()
    {
    Object::View ANY       = getANY();
    String::View vsPattern = getPattern();
    if (NULL == vsPattern)
        {
        // the result of "v LIKE NULL" is false for all values of "v"
        m_nPlan = ALWAYS_FALSE;
        return;
        }

    if (vsPattern->isASCII())
        {
        m_fPatternIsASCII = true;
        }
    else
        {
        m_fPatternIsASCII = false;
        return;
        }

    Array<char>::View   vachPattern    = toChar8Array(vsPattern);
    size32_t            cchPattern     = vachPattern->length;
    char                chEscape       = (char) getEscapeChar();
    bool                fEscape        = false;
    bool                fIgnoreCase    = isIgnoreCase();
    size32_t            ofCollectStart = 0;
    size32_t            cchCollected   = 0;
    HashSet::Handle     hBitset        = NULL;
    List::Handle        hList          = ArrayList::create();

    // parse the pattern into a hList of steps
    for (size32_t of = 0; of < cchPattern; ++of)
        {
        char ch = vachPattern[of];
        if (fEscape)
            {
            fEscape = false;
            }
        else if (ch == chEscape)
            {
            fEscape = true;
            continue;
            }
        else if (ch == '%')
            {
            if (cchCollected > 0)
                {
                hList->add(MatchStep::create(
                        String::create(vachPattern, ofCollectStart,
                                cchCollected),
                            hBitset, isIgnoreCase()));
                cchCollected = 0;
                hBitset = NULL;
                }

            if (hList->isEmpty() || hList->get(hList->size() -1) != ANY)
                {
                hList->add(ANY);
                }
            continue;
            }
        else if (ch == '_')
            {
            if (NULL == hBitset)
                {
                hBitset = HashSet::create();
                }
            hBitset->add(Integer32::valueOf(cchCollected));
            }
        if (cchCollected == 0)
          {
          ofCollectStart = of;
          }
        cchCollected++;
        }

    // check for unclosed escape
    if (fEscape)
        {
        COH_THROW_STREAM(IllegalArgumentException,
                            "pattern ends with an unclosed escape: \""
                << vsPattern << "\"");
        }

    // store off the last match step (if there is one)
    if (cchCollected > 0)
        {
        hList->add(MatchStep::create(
                String::create(vachPattern,ofCollectStart, cchCollected),
                hBitset, isIgnoreCase()));
        }

    // check for simple optimizations
    switch (hList->size())
        {
        case 0:
            // case sensistive     case insensitive    pattern
            // ------------------  ------------------  -------
            // EXACT_MATCH         EXACT_MATCH         ""
            m_nPlan  = EXACT_MATCH;
            m_vsPart = StringHelper::getEmptyString();
            return;

        case 1:
            // case sensistive     case insensitive    pattern
            // ------------------  ------------------  -------
            // EXACT_MATCH         INSENS_MATCH        "xyz"(no wildcards)
            // ALWAYS_TRUE         ALWAYS_TRUE         "%"  (only '%' wildcards)
            {
            Object::View v = hList->get(0);
            if (v == ANY)
                {
                m_nPlan = ALWAYS_TRUE;
                return;
                }

            MatchStep::View vMatchStep = cast<MatchStep::View>(v);
            if (vMatchStep->isLiteral())
                {
                m_nPlan = fIgnoreCase ? INSENS_MATCH : EXACT_MATCH;

                // vMatchStep may contain escaped chars (such as '_')
                m_vsPart = vMatchStep->f_vsMatch;
                return;
                }
            }
            break;

        case 2:
            // case sensistive     case insensitive    pattern
            // ------------------  ------------------  -------
            // STARTS_WITH_CHAR    STARTS_WITH_INSENS  "x%"
            // STARTS_WITH_STRING  STARTS_WITH_INSENS  "xyz%"
            // ENDS_WITH_CHAR      ENDS_WITH_INSENS    "%x"
            // ENDS_WITH_STRING    ENDS_WITH_INSENS    "%xyz"
            {
            MatchStep::View vMatchStep;
            bool            fStartsWith;
            Object::View    v = hList->get(0);
            if (v == ANY)
                {
                fStartsWith = false;
                vMatchStep  = cast<MatchStep::View>(hList->get(1));
                }
            else
                {
                fStartsWith = true;
                vMatchStep  = cast<MatchStep::View>(v);
                }
            if (vMatchStep->isLiteral())
                {
                if (fIgnoreCase)
                    {
                    m_nPlan  = fStartsWith ? STARTS_WITH_INSENS : ENDS_WITH_INSENS;
                    m_vsPart = vMatchStep->getString();
                    }
                else if (vMatchStep->getLength() == 1)
                    {
                    m_nPlan  = fStartsWith ? STARTS_WITH_CHAR : ENDS_WITH_CHAR;
                    m_chPart = vMatchStep->getString()->getOctets()[0];
                    }
                else
                    {
                    m_nPlan  = fStartsWith ? STARTS_WITH_STRING : ENDS_WITH_STRING;
                    m_vsPart = vMatchStep->getString();
                    }
                return;
                }
            }
            break;

        case 3:
            // case sensistive     case insensitive    pattern
            // ------------------  ------------------  -------
            // CONTAINS_CHAR       n/a                 "%x%"
            // CONTAINS_STRING     n/a                 "%xyz%"
            {
            if (!fIgnoreCase)
                {
                Object::View v = hList->get(1);
                if (v != ANY)
                    {
                    MatchStep::View vMatchStep = cast<MatchStep::View>(v);
                    if (vMatchStep->isLiteral())
                        {
                        if (vMatchStep->getLength() == 1)
                            {
                            m_nPlan  = CONTAINS_CHAR;
                            m_chPart = (char) vMatchStep->getString()->getOctets()[0];
                            }
                        else
                            {
                            m_nPlan  = CONTAINS_STRING;
                            m_vsPart = vMatchStep->getString();
                            }
                        return;
                        }
                    }
                }
            }
            break;
        }

    // build iterative plan
    // # steps  description
    // -------  --------------------------------------------------------
    //    1     match with '_'
    //    2     starts with or ends with match with '_'
    //    3     starts and ends with matches, or contains match with '_'
    //    4+    alternating % and matches, potentially starting with
    //          and/or ending with matches, each could have '_'
    m_nPlan = ITERATIVE_EVAL;
    switch (hList->size())
        {
        case 0:
            COH_ENSURE(false)
            break;

        case 1:
            m_vStepFront           = cast<MatchStep::View>(hList->get(0));
            m_fTrailingTextAllowed = false;
            break;

        case 2:
            {
            Object::View vStep1 = hList->get(0);
            Object::View vStep2 = hList->get(1);

            // should not have two "ANYs" in a row, but one must be ANY
            COH_ENSURE((vStep1 == ANY) ^ (vStep2 == ANY));

            if (vStep1 == ANY)
                {
                m_vStepBack            = cast<MatchStep::View>(vStep2);
                m_fTrailingTextAllowed = false;
                }
            else
                {
                m_vStepFront           = cast<MatchStep::View>(vStep1);
                m_fTrailingTextAllowed = true;
                }
            }
            break;

        default:
            {
            int32_t cMatchSteps   = hList->size();

            // figure out where the "middle" is; the "middle" is defined
            // as those steps that occur after one or more '%' matches
            // and before one or more '%' matches
            int32_t ofStartMiddle = 1;                // offset in hList of
                                                      // first middle step
            int32_t ofEndMiddle   = cMatchSteps - 2;  // offset in hList of
                                                      // last middle step

            Object::View vFirst = hList->get(0);
            if (vFirst != ANY)
                {
                m_vStepFront = cast<MatchStep::View>(vFirst);
                ++ofStartMiddle;
                }

            Object::View vLast          = hList->get(cMatchSteps - 1);
            bool         fLastStepIsAny = (vLast == ANY);
            if (!fLastStepIsAny)
                {
                m_vStepBack = cast<MatchStep::View>(vLast);
                --ofEndMiddle;
                }
            m_fTrailingTextAllowed = fLastStepIsAny;

            int32_t             cMatches   = (ofEndMiddle - ofStartMiddle) / 2 + 1;

            ObjectArray::Handle  vaMatches = ObjectArray::create(cMatches);
            int32_t             nMatch     = 0;
            for (int32_t of = ofStartMiddle; of <= ofEndMiddle; of += 2)
                {
                vaMatches[nMatch++] = hList->get(of);
                }
            m_vaStepMiddle = vaMatches;
            }
        }
    }

bool LikeFilter::isMatch(String::View vsValue) const
    {
    if (NULL == vsValue)
        {
        // NULL is not like anything
        return false;
        }

    if (!m_fPatternIsASCII)
         {
         COH_THROW (UnsupportedOperationException::create(
            "Pattern must be ASCII"));
         }

    if (!vsValue->isASCII())
        {
        COH_THROW (UnsupportedOperationException::create(
           "Strings must be ASCII"));
        }

    size32_t cchValue = vsValue->length();
    switch (m_nPlan)
        {
        case STARTS_WITH_CHAR:
            return cchValue >= 1 && vsValue->getOctets()[0] == m_chPart;

        case STARTS_WITH_STRING:
            return vsValue->startsWith(m_vsPart);

        case STARTS_WITH_INSENS:
            {
            String::View vsPrefix  = m_vsPart;
            size32_t     cchPrefix = vsPrefix->length();
            if (cchPrefix > cchValue)
                {
                return false;
                }
            return StringHelper::regionMatches(vsValue, 0,
                    vsPrefix, 0, cchPrefix, true);
            }

        case ENDS_WITH_CHAR:
            return cchValue >= 1
                && vsValue->getOctets()[cchValue - 1] == m_chPart;

        case ENDS_WITH_STRING:
            return vsValue->endsWith(m_vsPart);

        case ENDS_WITH_INSENS:
            {
            String::View vsSuffix   = m_vsPart;
            size32_t     cchSuffix  = vsSuffix->length();
            if (cchSuffix > cchValue)
                {
                return false;
                }
            return StringHelper::regionMatches(vsValue, cchValue - cchSuffix,
                    vsSuffix, 0, cchSuffix, true);
            }

        case CONTAINS_CHAR:
            return vsValue->indexOf(m_chPart) != String::npos;

        case CONTAINS_STRING:
            return vsValue->indexOf(m_vsPart) != String::npos;

        case ALWAYS_TRUE:
            return true;

        case ALWAYS_FALSE:
            return false;

        case EXACT_MATCH:
            return m_vsPart->equals(vsValue);

        case INSENS_MATCH:
            return StringHelper::compare(m_vsPart, vsValue, false) == 0;
        }

    // get the character data and iteratively process the LIKE
    Array<char>::View vach    = toChar8Array(vsValue);
    size32_t          cch     = vach->length;
    size32_t          ofBegin = 0;
    size32_t          ofEnd   = cch;

    // start by checking the front
    LikeFilter::MatchStep::View vMatchStep = m_vStepFront;
    if (NULL != vMatchStep)
        {
        size32_t cchStep = vMatchStep->getLength();
        if (cchStep > cch ||
                vMatchStep->indexOf(vach, ofBegin, cchStep)
                    == Array<char>::npos)
            {
            return false;
            }
        else
            {
            ofBegin = cchStep;
            }
        }

    // next check the back
    vMatchStep = m_vStepBack;
    if (NULL != vMatchStep)
        {
        size32_t cchStep = vMatchStep->getLength();
        size32_t ofStep  = cch - cchStep;
        if (ofStep < ofBegin ||
                vMatchStep->indexOf(vach, ofStep, ofEnd)
                    == Array<char>::npos)
            {
            return false;
            }
        else
            {
            ofEnd = ofStep;
            }
        }

    // check the middle
    ObjectArray::View vaMatchStep = m_vaStepMiddle;
    if (NULL != vaMatchStep)
        {
        for (size32_t i = 0, c = vaMatchStep->length; i < c; ++i)
            {
            vMatchStep = cast<MatchStep::View>(vaMatchStep[i]);
            size32_t of = vMatchStep->indexOf(vach, ofBegin, ofEnd);
            if (of == Array<char>::npos)
                {
                return false;
                }
            else
                {
                ofBegin = of + vMatchStep->getLength();
                }
            }
        }

    // this is the "is there anything left" check, which solves an
    // ambiguity in the "iterative step" design that did not correctly
    // differentiate between "%a_%" and "%a_", for example
    if (NULL == m_vStepBack && !m_fTrailingTextAllowed)
        {
        if (ofBegin != cch)
            {
            return false;
            }
        }

    return true;
    }


// ----- inner class: MatchStep ---------------------------------------------

// ----- constructors -------------------------------------------------------

LikeFilter::MatchStep::MatchStep(String::View vsMatch, HashSet::View hBitset,
        bool fIsIgnoreCase)
        : f_vsMatch(self(), vsMatch), m_vachMatch(self()),m_vachLower(self()),
          m_vafAny(self()), m_cchSkipFront(0), m_cchSkipBack(0),
          m_fMiddleWilds(0), m_fIsIgnoreCase(fIsIgnoreCase)
    {
    Array<char>::Handle hachMatch = toChar8Array(vsMatch);
    Array<char>::Handle hachLower = NULL;
    Array<bool>::Handle hafAny    = NULL;

    size32_t     cchSkipFront = 0;       // count of leading wildcards
    size32_t     cchSkipBack  = 0;       // count of trailing wildcards
    bool         fMiddleWilds = false;   // true iff any wildcards occur
                                         // in the middle of non-wildcards
    if (NULL != hBitset)
        {
        size32_t cch = hachMatch->length;
        hafAny = Array<bool>::create(cch);  // true for each char that is
                                            // a wildcard
        bool         fFront = true;         // false iff a non-wildcard is encountered
        size32_t     cWilds = 0;            // total number of wildcards
        size32_t     cCont  = 0;            // current count of continuous
                                            // wildcards
        for (size32_t i = 0; i < cch; ++i)
            {
            if (hBitset->contains(Integer32::valueOf(i))) // indicates a wildcard
                {
                hafAny[i] = true;
                if (fFront)
                    {
                    ++cchSkipFront;
                    }
                ++cWilds;
                ++cCont;
                }
            else
                {
                fFront = false;
                cCont  = 0;
                }
            }
        if (cCont > 0 && cCont < cWilds)
            {
            cchSkipBack = cCont;    // trailing continuous wildcards
            }
        fMiddleWilds = (cWilds > (cchSkipFront + cchSkipBack));
        }

    if (fIsIgnoreCase)
        {
        // create both "upper" and "lower" case characters for the
        // literal characters that need to be matched
        size32_t cch = hachMatch->length;
        hachLower = Array<char>::create(cch);
        for (size32_t of = 0; of < cch; ++of)
            {
            char ch = hachMatch[of];
            if (NULL == hafAny || !hafAny[of])
                {
                ch = toupper(ch);
                hachMatch[of] = ch;
                ch = tolower(ch);
                }
            hachLower[of] = ch;
            }
         }

    m_vachMatch     = hachMatch;
    m_vachLower     = hachLower;
    m_vafAny        = hafAny;
    m_cchSkipFront  = cchSkipFront;
    m_cchSkipBack   = cchSkipBack;
    m_fMiddleWilds  = fMiddleWilds;
    }


// ----- matching methods ---------------------------------------------------

size32_t LikeFilter::MatchStep::indexOf(Array<char>::View vach,
        size32_t ofBegin, size32_t ofEnd) const
    {
    Array<char>::View vachMatch = m_vachMatch;
    size32_t          cchMatch  = vachMatch->length;
    size32_t          cch       = ofEnd - ofBegin;

    if (cchMatch > cch)
        {
        // doesn't fit: can't match
        return Array<char>::npos;
        }

    size32_t cchSkipFront = m_cchSkipFront;
    if (cchSkipFront > 0)
        {
        if (cchSkipFront == cchMatch)
            {
            // just wildcards; found it if it fits
            return ofBegin;
            }

        // do not bother to match leading wildcards
        ofBegin += cchSkipFront;
        ofEnd   += cchSkipFront;
        }

    ofEnd    -= cchMatch;      // determine last offset that allows it to fit
    cchMatch -= m_cchSkipBack; // don't bother matching trailing wilds

    bool              fMiddleWilds = m_fMiddleWilds;
    Array<bool>::View vafAny       = m_vafAny;

    if (m_fIsIgnoreCase)
        {
        // processed in an equivalent way to String.equalsIngoreCase()
        Array<char>::View vachLower     = m_vachLower;
        char              chFirstUpper  = vachMatch[cchSkipFront];
        char              chFirstLower  = vachLower[cchSkipFront];
        NextChar: for ( ; ofBegin <= ofEnd; ++ofBegin)
            {
            char ch = vach[ofBegin];
            if (ch == chFirstUpper || ch == chFirstLower)
                {
                if (fMiddleWilds)
                    {
                    for (size32_t ofMatch = cchSkipFront + 1,
                            ofCur = ofBegin + 1;
                            ofMatch < cchMatch; ++ofMatch, ++ofCur)
                        {
                        if (!vafAny[ofMatch])
                            {
                            ch = vach[ofCur];
                            if (ch != vachMatch[ofMatch]
                                               && ch != vachLower[ofMatch])
                                {
                                // c++ does not have continue NextChar so
                                // we must increment ofBegin
                                ++ofBegin;
                                goto NextChar;
                                }
                            }
                        }
                    }
                else
                    {
                    for (size32_t ofMatch = cchSkipFront + 1,
                            ofCur = ofBegin + 1;
                            ofMatch < cchMatch; ++ofMatch, ++ofCur)
                        {
                        ch = vach[ofCur];
                        if (ch != vachMatch[ofMatch]
                                           && ch != vachLower[ofMatch])
                            {
                            // c++ does not have continue NextChar so
                            // we must increment ofBegin
                            ++ofBegin;
                            goto NextChar;
                            }
                        }
                    }

                // found it; adjust for the leading wilds that
                // we skipped matching
                return ofBegin - cchSkipFront;
                }
            }
        }
    else
        {
        // scan for a match
        char chFirst = vachMatch[cchSkipFront];
        NextChar2: for ( ; ofBegin <= ofEnd; ++ofBegin)
            {
             if (vach[ofBegin] == chFirst)
                {
                if (fMiddleWilds)
                    {
                    for (size32_t ofMatch = cchSkipFront + 1,
                            ofCur = ofBegin + 1;
                            ofMatch < cchMatch; ++ofMatch, ++ofCur)
                        {
                        if (!vafAny[ofMatch]
                                   && vachMatch[ofMatch] != vach[ofCur])
                            {
                            // c++ does not have continue NextChar so
                            // we must increment ofBegin
                            ++ofBegin;
                            goto NextChar2;
                            }
                        }
                    }
                else
                    {
                    for (size32_t ofMatch = cchSkipFront + 1,
                            ofCur = ofBegin + 1;
                            ofMatch < cchMatch; ++ofMatch, ++ofCur)
                        {
                         if (vachMatch[ofMatch] != vach[ofCur])
                            {
                            // c++ does not have continue NextChar so
                            // we must increment ofBegin
                            ++ofBegin;
                            goto NextChar2;
                            }
                        }
                    }

                // found it; adjust for the leading wilds that
                // we skipped matching
                return ofBegin - cchSkipFront;
                }
            }
        }

    return Array<char>::npos;
    }


// ----- Object methods -----------------------------------------------------

TypedHandle<const String> LikeFilter::MatchStep::toString() const
    {
     return COH_TO_STRING("MatchStep(" << f_vsMatch << ", "
         << (NULL == m_vafAny ? "exact" : "wild") << ')');
    }


// ----- accessors ----------------------------------------------------------

String::View LikeFilter::MatchStep::getString() const
    {
    return f_vsMatch;
    }

size32_t LikeFilter::MatchStep::getLength() const
    {
    return m_vachMatch->length;
    }

bool LikeFilter::MatchStep::isLiteral() const
    {
    return NULL == m_vafAny;
    }

COH_CLOSE_NAMESPACE3
