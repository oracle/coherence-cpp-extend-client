/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_STRING_HELPER_HPP
#define COH_STRING_HELPER_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* StringHelper provides all string utility functions for Strings.  Unless
* otherwise stated these functions only support ASCII bases Strings, and will
* throw an UnsupportedOperationExceptoin if supplied with a non-ASCII string.
* It is for this reason that these utlitie are not part of the String class.
*/
class COH_EXPORT StringHelper
    {
    // ----- constants ------------------------------------------------------

    public:
        /**
        * The largest possible value of type size32_t.
        */
        static const size32_t npos = String::npos;

        /**
        * Integer constant representing a unit of nanoseconds.
        *
        * @see parseTimeNanos(String, int)
        */
        static const int32_t unit_ns = -1000000;

        /**
        * Integer constant representing a unit of microseconds.
        *
        * @see #parseTimeNanos(String, int)
        */
        static const int32_t unit_us = -1000;

        /**
        * Integer constant representing a unit of milliseconds.
        *
        * @see #parseTime(String, int)
        */
        static const int32_t unit_ms = 1;

        /**
        * Integer constant representing a unit of seconds.
        *
        * @see #parseTime(String, int)
        */
        static const int32_t unit_s  = 1000*unit_ms;

        /**
        * Integer constant representing a unit of minutes.
        *
        * @see #parseTime(String, int)
        */
        static const int32_t unit_m  = 60*unit_s;

        /**
        * Integer constant representing a unit of hours.
        *
        * @see #parseTime(String, int)
        */
        static const int32_t unit_h  = 60*unit_m;

        /**
        * Integer constant representing a unit of days.
        *
        * @see #parseTime(String, int)
        */
        static const int32_t unit_d  = 24*unit_h;

        /**
        * Integer constant representing an exponent of zero.
        *
        * @see #parseBandwidth(String, int)
        * @see #parseMemorySize(String, int)
        */
        static const int32_t power_0 = 0;

        /**
        * Integer constant representing an exponent of 10.
        *
        * @see #parseBandwidth(String, int)
        * @see #parseMemorySize(String, int)
        */
        static const int32_t power_k = 10;

        /**
        * Integer constant representing an exponent of 20.
        *
        * @see #parseBandwidth(String, int)
        * @see #parseMemorySize(String, int)
        */
        static const int32_t power_m = 20;

        /**
        * Integer constant representing an exponent of 30.
        *
        * @see #parseBandwidth(String, int)
        * @see #parseMemorySize(String, int)
        */
        static const int32_t power_g = 30;

        /**
        * Integer constant representing an exponent of 40.
        *
        * @see #parseBandwidth(String, int)
        * @see #parseMemorySize(String, int)
        */
        static const int32_t power_t = 40;

    private:
        /**
        * Memory size constants.
        */
        static const int32_t      kb         = 1 << 10;
        static const int32_t      kb_mask    = kb - 1;


    // ----- static helper methods ------------------------------------------

    public:
        /**
        * Return an empty String.
        *
        * @return a zero-length empty String
        */
        static String::Handle getEmptyString();

        /**
        * Return a copy of vsSource which has had any occurrence of vsSearch
        * replaced with vsReplace.
        *
        * @param vsSource   the string to replace elements of
        * @param vsSearch   the string to search for in vsSource and
        *                   replace with vsReplace.
        * @param vsReplace  the new string to replace vsSearch with
        *
        * @return A copy of vsSource which has had any occurance of
        * vsSearch replaced with vsReplace.
        */
        static String::View replace(String::View vsSource,
                                      String::View vsSearch,
                                      String::View vsReplace);

        /**
        * Return a copy of vsSource which has first occurrence of vsSearch
        * replaced with vsReplace.
        *
        * @param vsSource   the string to replace element of
        * @param vsSearch   the string to search for in vsSource and
        *                   replace with vsReplace.
        * @param vsReplace  the new string to replace vsSearch with
        *
        * @return A copy of vsSource with the first occurrence of
        * vsSearch replaced with vsReplace if vsSearch exists in vsSource
        */
        static String::View replaceFirst(String::View vsSource,
                                         String::View vsSearch,
                                         String::View vsReplace);

        /**
        * Splits a string around matches of the supplied delimiters.  The
        * tokens returned by this function contain each substring of the
        * source string that is terminated by another delimiter or by the end
        * of the string. Trailing empty tokens are discarded. The substrings
        * in the array are in the order in which they occur in the source
        * string. If the source string does not contain any delimeters, then
        * the resulting array has just one element, the source string.
        * <p/>
        * Given the source string "boo:and:foo", the following tokens are
        * returned:
        * <p/>
        * <ul>
        *  <li>Delimeters = ":"  { "boo", "and", "foo" }</li>
        *  <li>Delimeters = "o"  { "b", "", ":and:f" }</li>
        *  <li>Delimeters = "bf" { "", "oo:and:", "oo" }</li>
        * </ul>
        * <p/>
        *
        * @param vsSource   the string to be parsed
        * @param vsDelims   the string containing the delimeter chars
        *
        * @return array containing String tokens; never returns NULL
        *
        * @throws coherence::lang::IllegalArgumentException
        *         if vsDelims is NULL
        */
        static ObjectArray::Handle split(String::View vsSource,
                String::View vsDelims);

        /**
        * Parse the given string representation of a time duration and return its
        * value as a number of milliseconds. The supplied string must be in the
        * format:
        *
        * <tt>[\d]+[[.][\d]+]?[NS|ns|US|us|MS|ms|S|s|M|m|H|h|D|d]?</tt>
        *
        * where the first non-digits (from left to right) indicate the unit of
        * time duration:
        * <ul>
        * <li><tt>NS</tt> or <tt>ns</tt> (nanoseconds)</li>
        * <li><tt>US</tt> or <tt>us</tt> (microseconds)</li>
        * <li><tt>MS</tt> or <tt>ms</tt> (milliseconds)</li>
        * <li><tt>S</tt>  or <tt>s</tt>  (seconds)</li>
        * <li><tt>M</tt>  or <tt>m</tt>  (minutes)</li>
        * <li><tt>H</tt>  or <tt>h</tt>  (hours)</li>
        * <li><tt>D</tt>  or <tt>d</tt>  (days)</li>
        * </ul>
        *
        * If the string value does not contain a unit, a unit of milliseconds is
        * assumed.
        *
        * @param s  a string with the format
        *           <tt>[\d]+[[.][\d]+]?[NS|ns|US|us|MS|ms|S|s|M|m|H|h|D|d]?</tt>
        *
        * @return the number of milliseconds represented by the given string
        *         rounded down to the nearest millisecond
        *
        * @see parseTimeNanos(String)
        */
        static int64_t parseTime(String::View vS);

        /**
        * Parse the given string representation of a time duration and return its
        * value as a number of milliseconds. The supplied string must be in the
        * format:
        *
        * <tt>[\d]+[[.][\d]+]?[NS|ns|US|us|MS|ms|S|s|M|m|H|h|D|d]?</tt>
        *
        * where the first non-digits (from left to right) indicate the unit of
        * time duration:
        * <ul>
        * <li><tt>NS</tt> or <tt>ns</tt> (nanoseconds)</li>
        * <li><tt>US</tt> or <tt>us</tt> (microseconds)</li>
        * <li><tt>MS</tt> or <tt>ms</tt> (milliseconds)</li>
        * <li><tt>S</tt>  or <tt>s</tt>  (seconds)</li>
        * <li><tt>M</tt>  or <tt>m</tt>  (minutes)</li>
        * <li><tt>H</tt>  or <tt>h</tt>  (hours)</li>
        * <li><tt>D</tt>  or <tt>d</tt>  (days)</li>
        * </ul>
        *
        * If the string value does not contain a unit, the specified default unit
        * is assumed. The default unit can be one of:
        * <ul>
        * <li>{@link #unit_ns}</li>
        * <li>{@link #unit_us}</li>
        * <li>{@link #unit_ms}</li>
        * <li>{@link #unit_s}</li>
        * <li>{@link #unit_m}</li>
        * <li>{@link #unit_h}</li>
        * <li>{@link #unit_d}</li>
        * </ul>
        *
        * @param s             a string with the format
        *                      <tt>[\d]+[[.][\d]+]?[NS|ns|US|us|MS|ms|S|s|M|m|H|h|D|d]?</tt>
        * @param nDefaultUnit  the unit to use in the conversion to milliseconds
        *                      if one is not specified in the supplied string
        *
        * @return the number of milliseconds represented by the given string
        *         rounded down to the nearest millisecond
        *
        * @see parseTimeNanos(String, int)
        */
        static int64_t parseTime(String::View vS, int32_t nDefaultUnit);

        /**
        * Parse the given string representation of a time duration and return its
        * value as a number of nanoseconds. The supplied string must be in the
        * format:
        *
        * <tt>[\d]+[[.][\d]+]?[NS|ns|US|us|MS|ms|S|s|M|m|H|h|D|d]?</tt>
        *
        * where the first non-digits (from left to right) indicate the unit of
        * time duration:
        * <ul>
        * <li><tt>NS</tt> or <tt>ns</tt> (nanoseconds)</li>
        * <li><tt>US</tt> or <tt>us</tt> (microseconds)</li>
        * <li><tt>MS</tt> or <tt>ms</tt> (milliseconds)</li>
        * <li><tt>S</tt>  or <tt>s</tt>  (seconds)</li>
        * <li><tt>M</tt>  or <tt>m</tt>  (minutes)</li>
        * <li><tt>H</tt>  or <tt>h</tt>  (hours)</li>
        * <li><tt>D</tt>  or <tt>d</tt>  (days)</li>
        * </ul>
        *
        * If the string value does not contain a unit, the specified default unit
        * is assumed. The default unit can be one of:
        * <ul>
        * <li>{@link #unit_ns}</li>
        * <li>{@link #unit_us}</li>
        * <li>{@link #unit_ms}</li>
        * <li>{@link #unit_s}</li>
        * <li>{@link #unit_m}</li>
        * <li>{@link #unit_h}</li>
        * <li>{@link #unit_d}</li>
        * </ul>
        *
        * @param s             a string with the format
        *                      <tt>[\d]+[[.][\d]+]?[NS|ns|US|us|MS|ms|S|s|M|m|H|h|D|d]?</tt>
        * @param nDefaultUnit  the unit to use in the conversion to nanoseconds
        *                      if one is not specified in the supplied string
        *
        * @return the number of nanoseconds represented by the given string
        *         rounded down to the nearest nanosecond
        */
        static int64_t parseTimeNanos(String::View vS, int32_t nDefaultUnit);

        /**
        * Format the passed memory size (in bytes) as a String. This method will
        * possibly round the memory size for purposes of producing a more-easily
        * read String value unless the <tt>fExact</tt> parameters is passed as
        * true; if <tt>fExact</tt> is true, then
        * <tt>cb==parseMemorySize(toMemorySizeString(cb, true))</tt> holds true
        * for all legal values of <tt>cb</tt>.
        *
        * @param cb        the number of bytes of memory
        * @param fExact    true if the String representation must be exact, or
        *                  false if it can be an approximation
        * @param fVerbose  true if the output string should verbose
        *
        * @return a String representation of the given memory size
        */
        static String::View toMemorySizeString(int64_t cb,
                bool fExact = true, bool fVerbose = false);

        /**
        * Tranform a delimited string into a camel case string.
        *
        * @param vsSource  the string to be parsed
        * @param vsDelims  the string containing the delimeter chars
        */
        static String::View camel(String::View vsSource,
                String::View vsDelims);

        /**
        * Compare two string for ordering.
        *
        * @param vsA             the first string
        * @param vsB             the second string
        * @param fCaseSensitive  true if the comparison should be case sensitive
        *
        * @return  a value greater than, equal to, or less than 0, if the vsA
        *          is greater then, equal to, or less then vsB
        */
        static int32_t compare(String::View vsA, String::View vsB, bool fCaseSensitive = false);

        /**
        * Tests if two string regions are equal.
        *
        * @param vsSource     the String whose substring is compared against
        *                     the parameter vsOther
        * @param ofSource     the offset in vsSource where comparison begins
        * @param vsOther      the String whose substring is compared against
        *                     the parameter vsSource
        * @param ofOther      the offset in vsOther where comparison begins
        * @param cLen         the count of characters to compare
        * @param fIgnoreCase  ignore case if true
        *
        * return the result of the compariaon
        */
        static bool regionMatches(String::View vsSource, size32_t oSource,
                String::View vsOther, size32_t oOther, size32_t cLen,
                bool fIgnoreCase = false);


        /**
        * Return true if vsSource string starts with vsPrefix string.
        *
        * @param vsSource     the string to search
        * @param vsPrefix     the string to search for
        * @param fIgnoreCase  ignore case if true
        *
        * return true if vsSource starts with vsPrefix
        */
        static bool startsWith(String::View vsSource, String::View vsPrefix,
                       bool fIgnoreCase = false);

        /**
        * Strip directories from a file name.
        *
        * @return a file name without any source prefix
        */
        static String::View stripSource(String::View vsFile);

        /**
        * Parse the passed String of hexidecimal characters into a binary
        * value.  This implementation allows the passed String to be prefixed
        * with "0x".
        *
        * @param vs  the hex String to evaluate
        *
        * @return the byte array value of the passed hex String, or NULL if
        * the string is empty
        */
        static Array<octet_t>::View parseHex(String::View vs);

        /**
        * Return the integer value of a hexidecimal digit.
        *
        * @param ch  the hex character to evaluate
        *
        * @return the integer value of the passed hex character
        */
        static int32_t parseHex(char ch);

        /**
        * Convert a byte array to a hex sequence of '0' + 'x' + 2 hex digits
        * per byte.
        *
        * @param vab  the byte array
        *
        * @return the hex sequence
        */
        static String::View toHexEscape(Array<octet_t>::View vab);

        /** Parse the given string representation of a number of bytes. The
        * supplied string must be in the format:
        * <p>
        * <tt>[\d]+[[.][\d]+]?[K|k|M|m|G|g|T|t]?[B|b]?</tt>
        * <p>
        * where the first non-digit (from left to right) indicates the factor
        * with which the preceeding decimal value should be multiplied:
        * <ul>
        * <li><tt>K</tt> or <tt>k</tt> (kilo, 2<sup>10</sup>)</li>
        * <li><tt>M</tt> or <tt>m</tt> (mega, 2<sup>20</sup>)</li>
        * <li><tt>G</tt> or <tt>g</tt> (giga, 2<sup>30</sup>)</li>
        * <li><tt>T</tt> or <tt>t</tt> (tera, 2<sup>40</sup>)</li>
        * </ul>
        * <p>
        * If the string value does not contain a factor, a factor of one is
        * assumed.
        * <p>
        * The optional last character <tt>B</tt> or <tt>b</tt> indicates a unit
        * of bytes.
        *
        * @param s  a string with the format
        *           <tt>[\d]+[[.][\d]+]?[K|k|M|m|G|g|T|t]?[B|b]?</tt>
        *
        * @return the number of bytes represented by the given string
        */
        static int64_t parseMemorySize(String::View vS);

        /**
        * Parse the given string representation of a number of bytes. The
        * supplied string must be in the format:
        * <p>
        * <tt>[\d]+[[.][\d]+]?[K|k|M|m|G|g|T|t]?[B|b]?</tt>
        * <p>
        * where the first non-digit (from left to right) indicates the factor
        * with which the preceeding decimal value should be multiplied:
        * <ul>
        * <li><tt>K</tt> or <tt>k</tt> (kilo, 2<sup>10</sup>)</li>
        * <li><tt>M</tt> or <tt>m</tt> (mega, 2<sup>20</sup>)</li>
        * <li><tt>G</tt> or <tt>g</tt> (giga, 2<sup>30</sup>)</li>
        * <li><tt>T</tt> or <tt>t</tt> (tera, 2<sup>40</sup>)</li>
        * </ul>
        * <p>
        * If the string value does not contain an explict or implicit factor, a
        * factor calculated by raising 2 to the given default power is used. The
        * default power can be one of:
        * <ul>
        * <li>{@link #power_0}</li>
        * <li>{@link #power_k}</li>
        * <li>{@link #power_m}</li>
        * <li>{@link #power_g}</li>
        * <li>{@link #power_t}</li>
        * </ul>
        * <p>
        * The optional last character <tt>B</tt> or <tt>b</tt> indicates a unit
        * of bytes.
        *
        * @param s              a string with the format
        *                       <tt>[\d]+[[.][\d]+]?[K|k|M|m|G|g|T|t]?[B|b]?</tt>
        * @param nDefaultPower  the exponent used to calculate the factor used in
        *                       the conversion if one is not implied by the given
        *                       string
        *
        * @return the number of bytes represented by the given string
        */
        static int64_t parseMemorySize(String::View vS, int32_t nDefaultPower);

        /**
        * Calculate the number of decimal digits needed to display the passed
        * value.
        *
        * @param n  the value
        *
        * @return the number of decimal digits needed to display the value
        */
        static int32_t getMaxDecDigits(int32_t n);

        /**
        * Format the passed non-negative integer as a fixed-length decimal
        * string.
        *
        * @param n        the value
        * @param cDigits  the length of the resulting decimal string
        *
        * @return the decimal value formated to the specified length string
        */
        static String::View toDecString(int32_t n, size32_t cDigits);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_STRING_HELPER_HPP
