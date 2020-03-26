/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/util/StringHelper.hpp"
#include "coherence/util/ArrayList.hpp"

#include <sstream>
#include <iostream>

COH_OPEN_NAMESPACE2(coherence,util)


// ----- file local helpers -------------------------------------------------

namespace
    {
    ObjectArray::View _createMemSuffixArray()
        {
        ObjectArray::Handle ha = ObjectArray::create(5);
        ha[0] = StringHelper::getEmptyString();
        ha[1] = String::create("KB");
        ha[2] = String::create("MB");
        ha[3] = String::create("GB");
        ha[4] = String::create("TB");

        return ha;
        }

    ObjectArray::View getMemSuffixArray()
        {
        static FinalView<ObjectArray> sva(System::common(),
                _createMemSuffixArray());
        return sva;
        }
    COH_STATIC_INIT(getMemSuffixArray());

    void assertASCII(String::View vsStr)
        {
        if (!vsStr->isASCII())
            {
            COH_THROW (UnsupportedOperationException::create(
                "StringHelper utilities only support ASCII strings."));
            }
        }
    }


// ----- StringHelper interface ---------------------------------------------

String::Handle StringHelper::getEmptyString()
    {
    static FinalHandle<String> hs(System::common(), String::create(""));
    return hs;
    }
COH_STATIC_INIT(StringHelper::getEmptyString());

String::View StringHelper::replace(String::View vsSource,
                                   String::View vsSearch,
                                   String::View vsReplace)
    {
    size32_t          cSearch = vsSearch->length();
    size32_t          cSource = vsSource->length();
    std::wstringstream sstream;

    for (size32_t iNext = 0, iLast = 0;
          (iNext != StringHelper::npos && iLast < cSource);
          iLast = iNext + cSearch)
        {
        iNext = vsSource->indexOf(vsSearch, iLast);

        sstream << vsSource->substring(iLast, iNext);
        if (iNext != String::npos)
            {
            sstream << vsReplace;
            }
        }

    return sstream.str();
    }

String::View StringHelper::replaceFirst(String::View vsSource,
                                        String::View vsSearch,
                                        String::View vsReplace)
    {
    size32_t iNext = vsSource->indexOf(vsSearch, 0);

    if (iNext == StringHelper::npos)
        {
        return vsSource;
        }
    else
        {
        std::wstringstream sstream;

        sstream << vsSource->substring(0, iNext);
        sstream << vsReplace;
        sstream << vsSource->substring(iNext + vsSearch->length(), vsSource->length());
        return sstream.str();
        }
    }

ObjectArray::Handle StringHelper::split(String::View vsSource,
        String::View vsDelims)
    {
    COH_ENSURE_PARAM(vsDelims);

    if (NULL == vsSource)
        {
        return ObjectArray::create(0);
        }

    assertASCII(vsSource);
    assertASCII(vsDelims);

    Array<octet_t>::View vabSrc   = vsSource->getOctets();
    const char*          acSrc    = (const char*) (const octet_t*) vabSrc->raw;
    const char*          acDelims = vsDelims->getCString();
    size32_t             ccSource = vsSource->length();
    size32_t             iNext    = 0;

    ArrayList::Handle hListTokens = ArrayList::create();

    for (size32_t iSource = 0; acSrc[iSource]; ++iSource)
        {
        for (size32_t iDelim = 0; acDelims[iDelim]; ++iDelim)
            {
            if (acSrc[iSource] == acDelims[iDelim])
                {
                String::View vStr =
                    String::create(vabSrc, iNext, iSource - iNext);

                hListTokens->add(vStr);
                iNext = iSource + 1;
                break;
                }
            }
        }
    // add remaining chars. Only empty if last char is a delimiter.
    if (iNext < ccSource)
        {
        hListTokens->add(String::create(vabSrc, iNext, ccSource - iNext));
        }

    return hListTokens->toArray();
    }

int64_t StringHelper::parseTime(String::View vS)
    {
    return parseTime(vS, unit_ms);
    }

int64_t StringHelper::parseTime(String::View vS, int32_t nDefaultUnit)
    {
    return parseTimeNanos(vS, nDefaultUnit) / 1000000L;
    }

int64_t StringHelper::parseTimeNanos(String::View vS, int32_t nDefaultUnit)
    {
    if (NULL == vS)
        {
        COH_THROW (IllegalArgumentException::create("passed String must not be null"));
        }

    switch (nDefaultUnit)
        {
        case unit_ns:
        case unit_us:
        case unit_ms:
        case unit_s:
        case unit_m:
        case unit_h:
        case unit_d:
            break;
        default:
            COH_THROW_STREAM(IllegalArgumentException, "illegal default unit: "
                    << nDefaultUnit);
        }

    // remove trailing "[NS|ns|US|us|MS|ms|S|s|M|m|H|h|D|d]?" and store it as a factor
    int64_t nMultiplier = nDefaultUnit;
    size32_t  cch         = vS->length();
    const char *cStr = vS->getCString();
    if (cch != 0)
        {
        switch (cStr[--cch])
            {
            case 'S': case 's':
                nMultiplier = unit_s;
                if (cch > 1)
                    {
                    char c = cStr[cch - 1];
                    switch (c)
                        {
                        case 'N': case 'n':
                            --cch;
                            nMultiplier = unit_ns;
                            break;
                        case 'U': case 'u':
                            --cch;
                            nMultiplier = unit_us;
                            break;
                        case 'M': case 'm':
                            --cch;
                            nMultiplier = unit_ms;
                            break;
                        }
                    }
                break;

            case 'M': case 'm':
                nMultiplier = unit_m;
                break;

            case 'H': case 'h':
                nMultiplier = unit_h;
                break;

            case 'D': case 'd':
                nMultiplier = unit_d;
                break;

            default:
                ++cch; // oops: shouldn't have chopped off the last char
                break;
            }
        }

    // convert multiplier into nanos
    nMultiplier = nMultiplier < 0 ? 1000000L / -nMultiplier
                                  : 1000000L * nMultiplier;

    // make sure that the string contains some digits
    if (cch == 0)
        {
        COH_THROW_STREAM(IllegalArgumentException, "passed String (\"" << vS
                << "\") must contain a number");
        }

    // extract the digits (decimal form) to assemble the base number
    int64_t cNanos   = 0;
    bool    fDecimal = false;
    int32_t nDivisor = 1;
    for (size32_t of = 0; of < cch; ++of)
        {
        char ch = cStr[of];
        switch (ch)
            {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                cNanos = (cNanos * 10) + (ch - '0');
                if (fDecimal)
                    {
                    nDivisor *= 10;
                    }
                break;

            case '.':
                if (fDecimal)
                    {
                    COH_THROW_STREAM(IllegalArgumentException,
                            "invalid time: \"" <<  vS
                            << "\" (illegal second decimal point)");
                    }
                fDecimal = true;
                break;

            default:
                COH_THROW_STREAM(IllegalArgumentException, "invalid time: \""
                        << vS << "\" (illegal digit: \"" << ch << "\")");
            }
        }

    cNanos *= nMultiplier;
    if (fDecimal)
        {
        if (nDivisor == 1)
            {
            COH_THROW_STREAM(IllegalArgumentException, "invalid time: \""
                << vS << "\" (illegal trailing decimal point)");
            }
        else
            {
            cNanos /= nDivisor;
            }
        }
    return cNanos;
    }

String::View StringHelper::toMemorySizeString(int64_t cb, bool fExact, bool fVerbose)
    {
    if (cb < 0)
        {
        COH_THROW_STREAM(IllegalArgumentException, "negative quantity: "
                << cb);
        }

    if (cb < 1024)
        {
        return fVerbose ? COH_TO_STRING(cb << " B") : COH_TO_STRING(cb);
        }

    ObjectArray::View vaSuffix = getMemSuffixArray();
    size32_t          cDivs    = 0;
    size32_t          cMaxDivs = vaSuffix->length - 1;

    if (fExact)
        {
        // kilobytes? megabytes? gigabytes? terabytes?
        while (((((int32_t) cb) & kb_mask) == 0) && cDivs < cMaxDivs)
            {
            cb = (uint64_t)cb >> 10;
            ++cDivs;
            }

        return fVerbose
                ? cDivs == 0
                    ? COH_TO_STRING(cb << " B")
                    : COH_TO_STRING(cb << vaSuffix[cDivs])
                : COH_TO_STRING(cb << vaSuffix[cDivs]);
        }

    // need roughly the 3 most significant decimal digits
    int32_t cbRem = 0;
    while (cb >= kb && cDivs < cMaxDivs)
        {
        cbRem = ((int32_t) cb) & kb_mask;
        cb = (uint64_t)cb >> 10;
        ++cDivs;
        }

    String::View vs = COH_TO_STRING(cb);
    size32_t cch = vs->length();
    if (cch < 3 && cbRem != 0)
        {
        // need the first digit or two of string value of cbRem / 1024;
        // format the most significant two digits ".xx" as a string "1xx"
        String::Handle hsDec = COH_TO_STRING((int32_t)(cbRem / 10.24 + 100));
        vs = COH_TO_STRING(vs << '.' << hsDec->substring(1, 1 + 3 - cch));
        }
    if (fVerbose)
        {
        if (cDivs == 0)
            {
            vs = COH_TO_STRING(vs << " B");
            }
        else
            {
            vs = COH_TO_STRING(vs << ' ' << vaSuffix[cDivs]);
            }
        }
    else
        {
        vs = COH_TO_STRING(vs << vaSuffix[cDivs]);
        }

    return vs;
    }

String::View StringHelper::camel(String::View vsSource,
                String::View vsDelims)
    {
    ObjectArray::View vaToken = split(vsSource, vsDelims);
    std::wstringstream out;

    for (size32_t i = 0, c = vaToken->length; i < c; ++i)
        {
        const char* achToken = cast<String::View>(vaToken[i])->getCString();
        char        ch       = achToken[0];
        if (islower(ch))
            {
            out << (char) toupper(ch) << ++achToken;
            }
        else
            {
            out << achToken;
            }
        }
    return out.str();
    }

int32_t StringHelper::compare(String::View vsA, String::View vsB, bool fCaseSensitive)
    {
    COH_ENSURE_PARAM(vsA);
    COH_ENSURE_PARAM(vsB);

    assertASCII(vsA);
    assertASCII(vsB);

    if (vsA == vsB)
        {
        return 0;
        }

    const char* achA = vsA->getCString();
    const char* achB = vsB->getCString();
    if (fCaseSensitive)
        {
        for (; *achA && (*achA == *achB); ++achA, ++achB);
        return *achA - *achB;
        }
    else
        {
        for (; *achA && (toupper(*achA) == toupper(*achB)); ++achA, ++achB);
        return toupper(*achA) - toupper(*achB);
        }
    }

bool StringHelper::regionMatches(String::View vsSource, size32_t oSource,
        String::View vsOther, size32_t oOther, size32_t cLen,
        bool fIgnoreCase)
    {
    return compare(
            vsSource->substring(oSource, oSource + cLen),
            vsOther->substring(oOther, oOther + cLen),
            !fIgnoreCase) == 0;
    }

bool StringHelper::startsWith(String::View vsSource, String::View vsPrefix,
                              bool fIgnoreCase)
    {
    return vsPrefix->length() > vsSource->length() ? false : compare(vsSource->substring(0, vsPrefix->length()), vsPrefix, !fIgnoreCase) == 0;
    }

String::View StringHelper::stripSource(String::View vsFile)
    {
    size32_t of = vsFile->lastIndexOf('/');
    if (of == String::npos)
        {
        of = vsFile->lastIndexOf('\\');
        }
    return of == String::npos ? vsFile : vsFile->substring(of + 1);
    }

Array<octet_t>::View StringHelper::parseHex(String::View vs)
    {
    std::stringstream ss;
    const char*       ach = vs->getCString();
    size32_t          cch = vs->length();

    if (0 == cch)
        {
        return NULL;
        }

    size32_t ofch = 0;
    if ('x' == ach[1] || 'X' == ach[1])
        {
        ofch = 2;
        }

    size32_t               cb = (cch - ofch) / 2;
    Array<octet_t>::Handle ab = Array<octet_t>::create(cb);
    for (size32_t ofb = 0; ofb < cb; ++ofb)
        {
        ab[ofb] = (octet_t) (parseHex(ach[ofch]) << 4 |
                parseHex(ach[ofch + 1]));
        ofch += 2;
        }
    return ab;
    }

int32_t StringHelper::parseHex(char ch)
    {
    switch (ch)
        {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            return ch - '0';

        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
            return ch - 'A' + 0x0A;

        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
            return ch - 'a' + 0x0A;

        default:
            COH_THROW_STREAM(IllegalArgumentException,
                    "Illegal hex char: " << ch);
        }
    }

String::View StringHelper::toHexEscape(Array<octet_t>::View vab)
    {
    const char*         achHex = "0123456789ABCDEF";
    size32_t            cLen   = vab->length;
    std::stringstream   ss;

    ss << '0';
    ss << 'x';

    for (size32_t ofb = 0; ofb < cLen; ++ofb)
        {
        int32_t n = vab[ofb] & 0xFF;
        ss << achHex[n >> 4];
        ss << achHex[n & 0x0F];
        }
    return ss.str();
    }

int64_t StringHelper::parseMemorySize(String::View vS)
    {
    return parseMemorySize(vS, power_0);
    }

int64_t StringHelper::parseMemorySize(String::View vS, int32_t nDefaultPower)
    {
    if (NULL == vS)
        {
        COH_THROW (IllegalArgumentException::create("passed String must not be null"));
        }

    switch (nDefaultPower)
        {
        case power_0:
        case power_k:
        case power_m:
        case power_g:
        case power_t:
            break;
        default:
            COH_THROW_STREAM(IllegalArgumentException,
                    "illegal default power: " << nDefaultPower);
        }

    // remove trailing "[K|k|M|m|G|g|T|t]?[B|b]?" and store it as a factor
    int32_t     cBitShift = power_0;
    size32_t    cch       = vS->length();
    const char* cStr      = vS->getCString();
    if (cch > 0)
        {
        bool fDefault;
        char      ch = cStr[cch - 1];
        if (ch == 'B' || ch == 'b')
            {
            // bytes are implicit
            --cch;
            fDefault = false;
            }
        else
            {
            fDefault = true;
            }

        if (cch > 0)
            {
            switch (cStr[--cch])
                {
                case 'K': case 'k':
                    cBitShift = power_k;
                    break;

                case 'M': case 'm':
                    cBitShift = power_m;
                    break;

                case 'G': case 'g':
                    cBitShift = power_g;
                    break;

                case 'T': case 't':
                    cBitShift = power_t;
                    break;

                default:
                    if (fDefault)
                        {
                        cBitShift = nDefaultPower;
                        }
                    ++cch; // oops: shouldn't have chopped off the last char
                    break;
                }
            }
        }

    // make sure that the string contains some digits
    if (cch == 0)
        {
        COH_THROW_STREAM(IllegalArgumentException, "passed String (\"" << vS
                << "\") must contain a number");
        }

    // extract the digits (decimal form) to assemble the base number
    int64_t cb       = 0;
    bool    fDecimal = false;
    int32_t nDivisor = 1;
    for (size32_t of = 0; of < cch; ++of)
        {
        char ch = cStr[of];
        switch (ch)
            {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                cb = (cb * 10) + (ch - '0');
                if (fDecimal)
                    {
                    nDivisor *= 10;
                    }
                break;

            case '.':
                if (fDecimal)
                    {
                    COH_THROW_STREAM(IllegalArgumentException,
                            "invalid memory size: \"" << vS
                            << "\" (illegal second decimal point)");
                    }
                fDecimal = true;
                break;

            default:
                COH_THROW_STREAM(IllegalArgumentException,
                        "invalid memory size: \"" << vS
                        << "\" (illegal digit: \"" << ch << "\")");
            }
        }

    cb <<= cBitShift;
    if (fDecimal)
        {
        if (nDivisor == 1)
            {
            COH_THROW_STREAM(IllegalArgumentException,
                    "invalid memory size: \"" << vS
                    << "\" (illegal trailing decimal point)");
            }
        else
            {
            cb /= nDivisor;
            }
        }
    return cb;
    }

int32_t StringHelper::getMaxDecDigits(int32_t n)
    {
    int32_t cDigits = 0;
    do
        {
        cDigits += 1;
        n /= 10;
        }
    while (n != 0);

    return cDigits;
    }

String::View StringHelper::toDecString(int32_t n, size32_t cDigits)
    {
    Array<char>::Handle hach = Array<char>::create(cDigits);
    while (cDigits > 0)
        {
        hach[--cDigits] = (char) ('0' + n % 10);
        n /= 10;
        }

    return String::create((Array<char>::View) hach);
    }

COH_CLOSE_NAMESPACE2
