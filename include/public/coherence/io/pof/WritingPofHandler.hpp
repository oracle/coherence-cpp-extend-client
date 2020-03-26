/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_WRITING_POF_HANDLER_HPP
#define COH_WRITING_POF_HANDLER_HPP

#include "coherence/lang.ns"

#include "coherence/io/WriteBuffer.hpp"
#include "coherence/io/pof/PofHandler.hpp"
#include "coherence/io/pof/PofHelper.hpp"
#include "coherence/util/Binary.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::io::WriteBuffer;
using coherence::util::Binary;


/**
* An implementation of PofHandler that writes a POF stream to a WriteBuffer
* using a BufferOutput object.
*
* @author jh  2008.04.09
*/
class COH_EXPORT WritingPofHandler
    : public class_spec<WritingPofHandler,
        extends<PofHelper>,
        implements<PofHandler> >
    {
    friend class factory<WritingPofHandler>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a Writing POF Handler that will write a POF stream to the
        * passed BufferOutput object.
        *
        * @param hOut  the BufferOutput to write to
        *
        * @return the new WritingPofHandler
        */
        WritingPofHandler(WriteBuffer::BufferOutput::Handle hOut);

    private:
        /**
        * Blocked copy constructor.
        */
        WritingPofHandler(const WritingPofHandler&);


    // ----- WritingPofHandler methods --------------------------------------

    public:
        /**
        * Report that a value of a "user type" has been encountered in the
        * POF stream. A user type is analogous to a "class", and a value of
        * a user type is analogous to an "object".
        *
        * This method call will be followed by a separate call to an "on" or
        * "begin" method for each of the property values in the user type,
        * and the user type will then be terminated by a call to
        * {@link endComplexValue()}.
        *
        * @param iPos         context-sensitive position information: property
        *                     index within a user type, array index within an
        *                     array, element counter within a collection,
        *                     entry counter within a map, -1 otherwise
        * @param nId          identity of the object to encode, or -1 if
        *                     identity shouldn't be encoded in the POF stream
        * @param nUserTypeId  the user type identifier,
        *                     <tt>(nUserTypeId &gt;= 0)</tt>
        * @param nVersionId   the version identifier for the user data type data
        *                     in the POF stream, <tt>(nVersionId &gt;= 0)</tt>
        *
        * @since Coherence 3.7.1
        */
        virtual void beginUserType(int32_t iPos, int nId, int32_t nUserTypeId,
                int32_t nVersionId);


    // ----- PofHandler interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void registerIdentity(int32_t nId);

        /**
        * {@inheritDoc}
        */
        virtual void onNullReference(int32_t iPos);

        /**
        * {@inheritDoc}
        */
        virtual void onIdentityReference(int32_t iPos, int32_t nId);

        /**
        * {@inheritDoc}
        */
        virtual void onInt16(int32_t iPos, int16_t n);

        /**
        * {@inheritDoc}
        */
        virtual void onInt32(int32_t iPos, int32_t n);

        /**
        * {@inheritDoc}
        */
        virtual void onInt64(int32_t iPos, int64_t n);

        /**
        * {@inheritDoc}
        */
        virtual void onFloat32(int32_t iPos, float32_t fl);

        /**
        * {@inheritDoc}
        */
        virtual void onFloat64(int32_t iPos, float64_t dfl);

        /**
        * {@inheritDoc}
        */
        virtual void onBoolean(int32_t iPos, bool f);

        /**
        * {@inheritDoc}
        */
        virtual void onOctet(int32_t iPos, octet_t b);

        /**
        * {@inheritDoc}
        */
        virtual void onOctetString(int32_t iPos, Binary::View vBin);

        /**
        * {@inheritDoc}
        */
        virtual void onChar(int32_t iPos, wchar16_t ch);

        /**
        * {@inheritDoc}
        */
        virtual void onCharString(int32_t iPos, String::View vs);

        /**
        * {@inheritDoc}
        */
        virtual void onDate(int32_t iPos, int32_t nYear, int32_t nMonth,
                int32_t nDay);

        /**
        * {@inheritDoc}
        */
        virtual void onYearMonthInterval(int32_t iPos, int32_t cYears,
                int32_t cMonths);

        /**
        * {@inheritDoc}
        */
        virtual void onTime(int32_t iPos, int32_t nHour, int32_t nMinute,
                int32_t nSecond, int32_t nNano, bool fUTC);

        /**
        * {@inheritDoc}
        */
        virtual void onTime(int32_t iPos, int32_t nHour, int32_t nMinute,
                int32_t nSecond, int32_t nNano, int32_t nHourOffset,
                int32_t nMinuteOffset);

        /**
        * {@inheritDoc}
        */
        virtual void onTimeInterval(int32_t iPos, int32_t cHours,
                int32_t cMinutes, int32_t cSeconds, int32_t cNanos);

        /**
        * {@inheritDoc}
        */
        virtual void onDateTime(int32_t iPos, int32_t nYear, int32_t nMonth,
                int32_t nDay, int32_t nHour, int32_t nMinute, int32_t nSecond,
                int32_t nNano, bool fUTC);

        /**
        * {@inheritDoc}
        */
        virtual void onDateTime(int32_t iPos, int32_t nYear, int32_t nMonth,
                int32_t nDay, int32_t nHour, int32_t nMinute, int32_t nSecond,
                int32_t nNano, int32_t nHourOffset, int32_t nMinuteOffset);

        /**
        * {@inheritDoc}
        */
        virtual void onDayTimeInterval(int32_t iPos, int32_t cDays,
                int32_t cHours, int32_t cMinutes, int32_t cSeconds,
                int32_t cNanos);

        /**
        * {@inheritDoc}
        */
        virtual void beginCollection(int32_t iPos, size32_t cElements);

        /**
        * {@inheritDoc}
        */
        virtual void beginUniformCollection(int32_t iPos, size32_t cElements,
                int32_t nType);

        /**
        * {@inheritDoc}
        */
        virtual void beginArray(int32_t iPos, size32_t cElements);

        /**
        * {@inheritDoc}
        */
        virtual void beginUniformArray(int32_t iPos, size32_t cElements,
                int32_t nType);

        /**
        * {@inheritDoc}
        */
        virtual void beginSparseArray(int32_t iPos, size32_t cElements);

        /**
        * {@inheritDoc}
        */
        virtual void beginUniformSparseArray(int32_t iPos, size32_t cElements,
                int32_t nType);

        /**
        * {@inheritDoc}
        */
        virtual void beginMap(int32_t iPos, size32_t cElements);

        /**
        * {@inheritDoc}
        */
        virtual void beginUniformKeysMap(int32_t iPos, size32_t cElements,
                int32_t nTypeKeys);

        /**
        * {@inheritDoc}
        */
        virtual void beginUniformMap(int32_t iPos, size32_t cElements,
                int32_t nTypeKeys, int32_t nTypeValues);

        /**
        * {@inheritDoc}
        */
        virtual void beginUserType(int32_t iPos, int32_t nUserTypeId,
                int32_t nVersionId);

        /**
        * {@inheritDoc}
        */
        virtual void endComplexValue();


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * Determine if the value encoding can be skipped. A value can be
        * skipped if it is a default value and if it does not have an
        * identity and if it is in a sparse data structure.
        *
        * @return true iff value encoding of default values can be skipped
        *         altogether
        */
        virtual bool isSkippable() const;

        /**
        * Determine if the value encoding can be compressed by combining type
        * and value information in such a way that type information could be
        * lost.
        *
        * @return true iff values can be encoded without type information
        */
        virtual bool isCompressable() const;

        /**
        * Called for each and every value going into the POF stream, in case
        * the value needs its position to be encoded into the stream.
        *
        * @param iPos  the position (property index, array index, etc.)
        */
        virtual void encodePosition(int32_t iPos);

        /**
        * Determine if the type should be encoded for the current value.
        *
        * @param nTypeId  the type of the current value
        *
        * @return true if the type ID should be placed into the POF stream,
        *         and false if only the value itself should be placed into
        *         the stream
        */
        virtual bool isTypeIdEncoded(int32_t nTypeId) const;


    // ----- inner class: Complex -------------------------------------------

    public:
        /**
        * A Complex object represents the current complex data structure in
        * the POF stream.
        */
        class COH_EXPORT Complex
            : public class_spec<Complex>
            {
            friend class factory<Complex>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Construct a Complex object for a data collection or user
                * type.
                *
                * @param hComplexCurrent  the current Complex object or NULL
                * @param fEncodePosition  true to encode the position
                *                         information
                */
                Complex(Complex::Handle hComplexCurrent,
                        bool fEncodePosition);

                /**
                * Construct a Complex object for a uniformly-typed data
                * collection.
                *
                * @param hComplexCurrent  the current Complex object or NULL
                * @param fEncodePosition  true to encode the position
                *                         information
                * @param nUniformTypeId   the type identifier of the uniform
                *                         type
                */
                Complex(Complex::Handle hComplexCurrent,
                        bool fEncodePosition, int32_t nUniformTypeId);

            private:
                /**
                * Blocked copy constructor.
                */
                Complex(const Complex&);

            // ----- Complex interface ----------------------------------

            public:
                /**
                * Notify the Complex object that a value has been encountered.
                *
                * @param iPos  the position that accompanied the value
                */
                virtual void onValue(int32_t iPos);

            // ----- accessors ------------------------------------------

            public:
                /**
                * Determine if the object encoding within the Complex type is
                * uniform.
                *
                * @return true iff values within the Complex type are of a
                *         uniform type and are encoded uniformly
                */
                virtual bool isUniform() const;

                /**
                * If the object encoding is using uniform encoding, obtain
                * the type id of the uniform type.
                *
                * @return the type id used for the uniform encoding
                */
                virtual int32_t getUniformType() const;

                /**
                * Determine if the position information is encoded with the
                * values of the complex type, and if the Complex type is
                * terminated in the POF stream with an illegal position (-1).
                *
                * @return true iff the complex value is a sparse type
                */
                virtual bool isSparse() const;

                /**
                * Pop this Complex object off the stack, returning the outer
                * Complex object or NULL if there is none.
                *
                * @return the outer Complex object or NULL if there is none
                */
                virtual Complex::Handle pop();

                /**
                * Pop this Complex object off the stack, returning the outer
                * Complex object or NULL if there is none.
                *
                * @return the outer Complex object or NULL if there is none
                */
                virtual Complex::View pop() const;


            // ----- data members ---------------------------------------

            protected:
                /**
                * Whether or not the position information is encoded. This is
                * true for user type properties and array elements.
                */
                const bool m_fSparse;

                /**
                * Whether or not values within the complex type are uniformly
                * encoded. This is expected for arrays of primitive types,
                * for example.
                */
                const bool m_fUniform;

                /**
                * The type ID, if uniform encoding is used.
                */
                const int32_t m_nTypeId;

                /**
                * The Complex within which this Complex exists, to support
                * nesting.
                */
                FinalHandle<Complex> f_hComplexOuter;
            };


    // ----- inner class: ComplexMap ----------------------------------------

    public:
        /**
        * A ComplexMap object represents a map data structure (with uniform
        * keys or with uniform keys and values) in the POF stream.
        */
        class COH_EXPORT ComplexMap
            : public class_spec<ComplexMap,
                extends<Complex> >
            {
            friend class factory<ComplexMap>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Construct a ComplexMap object for maps with uniformly-typed
                * keys.
                *
                * @param hcomplexCurrent    the current Complex object or
                *                           NULL
                * @param nUniformKeyTypeId  the type identifier of the
                *                           uniform type
                */
                ComplexMap(Complex::Handle hComplexCurrent,
                        int32_t nUniformKeyTypeId);

                /**
                * Construct a ComplexMap object for maps with uniformly-typed
                * keys and values.
                *
                * @param hComplexCurrent    the current Complex object or
                *                           NULL
                * @param nUniformKeyTypeId  the type identifier of the
                *                           uniform type for keys in the map
                * @param nUniformValTypeId  the type identifier of the
                *                           uniform type for values in the
                *                           map
                */
                ComplexMap(Complex::Handle hComplexCurrent,
                        int32_t nUniformKeyTypeId, int32_t nUniformValTypeId);

            private:
                /**
                * Blocked copy constructor.
                */
                ComplexMap(const ComplexMap&);

            // ----- Complex interface ----------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual void onValue(int32_t iPos);

            // ----- accessors ------------------------------------------

            public:
                /**
                * Determine if the object encoding within the Complex type is
                * uniform.
                *
                * @return true iff values within the Complex type are of a
                *         uniform type and are encoded uniformly
                */
                virtual bool isUniform() const;

                /**
                * If the object encoding is using uniform encoding, obtain
                * the type id of the uniform type.
                *
                * @return the type id used for the uniform encoding
                */
                virtual int32_t getUniformType() const;

            // ----- data members ---------------------------------------

            protected:
                /**
                * Toggles between key and value processing every time the
                * caller invokes {@link #onValue}.
                */
                bool m_fKey;

                /**
                * Whether or not values within the map are uniformly encoded.
                */
                const bool m_fUniformValue;

                /**
                * The value type ID, if uniform encoding is used for values.
                */
                const int32_t m_nValueTypeId;
            };


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Obtain the DataOutput object that this Writing POF Handler is
        * writing to.
        *
        * @return the DataOutput object that this POF handler is writing to
        */
        virtual WriteBuffer::BufferOutput::Handle getBufferOutput();

        /**
        * Obtain the DataOutput object that this Writing POF Handler is
        * writing to.
        *
        * @return the DataOutput object that this POF handler is writing to
        */
        virtual WriteBuffer::BufferOutput::View getBufferOutput() const;

        /**
        * Obtain the current Complex object that represents the complex type
        * that is being written to the POF stream.
        *
        * @return the current Complex object
        */
        virtual Complex::Handle getComplex();

        /**
        * Obtain the current Complex object that represents the complex type
        * that is being written to the POF stream.
        *
        * @return the current Complex object
        */
        virtual Complex::View getComplex() const;

        /**
         * Return the ID of the current Complex object.
         *
         * @return the ID of the current Complex object.
         */
        virtual size64_t getComplexId() const;

    protected:
        /**
         * Set the current Complex object.
         *
         * @param hComplex  the current Complex object.
         */
        virtual void setComplex(Complex::Handle hComplex);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The BufferOutput to write to.
        */
        FinalHandle<WriteBuffer::BufferOutput> f_hOut;

        /**
        * The current containing Complex value in the POF stream.
        */
        MemberHandle<Complex> m_hComplex;

        /**
         * The ID of the current Complex value.
         */
        size64_t m_nComplexId;

        /**
        * Set to true when the next value to write has been tagged with an
        * identity.
        */
        bool m_fHasIdentity;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_WRITING_POF_HANDLER_HPP
