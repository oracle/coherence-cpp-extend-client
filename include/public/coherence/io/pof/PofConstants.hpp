/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POF_CONSTANTS_HPP
#define COH_POF_CONSTANTS_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE3(coherence,io,pof)


/**
* Constants related to POF streams.
*
* @author jh  2008.04.09
*/
class COH_EXPORT PofConstants
    : public abstract_spec<PofConstants>
    {
    // ----- POF intrinsic type constants -----------------------------------

    public:
        // NOTE: The hex value to the right is the packed integer value of
        // the constant.
        typedef enum
            {
            t_int16                 = -1,       // 0x40
            t_int32                 = -2,       // 0x41
            t_int64                 = -3,       // 0x42
            t_int128                = -4,       // 0x43
            t_float32               = -5,       // 0x44
            t_float64               = -6,       // 0x45
            t_float128              = -7,       // 0x46
            t_decimal32             = -8,       // 0x47
            t_decimal64             = -9,       // 0x48
            t_decimal128            = -10,      // 0x49
            t_boolean               = -11,      // 0x4A
            t_octet                 = -12,      // 0x4B
            t_octet_string          = -13,      // 0x4C
            t_char                  = -14,      // 0x4D
            t_char_string           = -15,      // 0x4E
            t_date                  = -16,      // 0x4F
            t_year_month_interval   = -17,      // 0x50
            t_time                  = -18,      // 0x51
            t_time_interval         = -19,      // 0x52
            t_datetime              = -20,      // 0x53
            t_day_time_interval     = -21,      // 0x54
            t_collection            = -22,      // 0x55
            t_uniform_collection    = -23,      // 0x56
            t_array                 = -24,      // 0x57
            t_uniform_array         = -25,      // 0x58
            t_sparse_array          = -26,      // 0x59
            t_uniform_sparse_array  = -27,      // 0x5A
            t_map                   = -28,      // 0x5B
            t_uniform_keys_map      = -29,      // 0x5C
            t_uniform_map           = -30,      // 0x5D
            t_identity              = -31,      // 0x5E
            t_reference             = -32       // 0x5F
            } PofType;


    // ----- POF compact "small" values -------------------------------------

    public:
        // NOTE: The hex value to the right is the packed integer value of
        // the constant.
        typedef enum
            {
            v_boolean_false         = -33,      // 0x60
            v_boolean_true          = -34,      // 0x61
            v_string_zero_length    = -35,      // 0x62
            v_collection_empty      = -36,      // 0x63
            v_reference_null        = -37,      // 0x64
            v_fp_pos_infinity       = -38,      // 0x65
            v_fp_neg_infinity       = -39,      // 0x66
            v_fp_nan                = -40,      // 0x67
            v_int_neg_1             = -41,      // 0x68
            v_int_0                 = -42,      // 0x69
            v_int_1                 = -43,      // 0x6A
            v_int_2                 = -44,      // 0x6B
            v_int_3                 = -45,      // 0x6C
            v_int_4                 = -46,      // 0x6D
            v_int_5                 = -47,      // 0x6E
            v_int_6                 = -48,      // 0x6F
            v_int_7                 = -49,      // 0x70
            v_int_8                 = -50,      // 0x71
            v_int_9                 = -51,      // 0x72
            v_int_10                = -52,      // 0x73
            v_int_11                = -53,      // 0x74
            v_int_12                = -54,      // 0x75
            v_int_13                = -55,      // 0x76
            v_int_14                = -56,      // 0x77
            v_int_15                = -57,      // 0x78
            v_int_16                = -58,      // 0x79
            v_int_17                = -59,      // 0x7A
            v_int_18                = -60,      // 0x7B
            v_int_19                = -61,      // 0x7C
            v_int_20                = -62,      // 0x7D
            v_int_21                = -63,      // 0x7E
            v_int_22                = -64,      // 0x7F
            // Indicates unknown type. Not seen in POF stream
            t_unknown               = -65       // 0x1C0
            } PofValue;


    // ----- C++ type constants ---------------------------------------------

    public:
        typedef enum
            {
            c_null                      = 0,
            c_boolean                   = 1,
            c_octet                     = 2,
            c_char16                    = 3,
            c_int16                     = 4,
            c_int32                     = 5,
            c_int64                     = 6,
            c_float32                   = 7,
            c_float64                   = 8,
            c_binary                    = 9,
            c_string                    = 10,
            c_raw_date                  = 11,
            c_raw_date_time             = 12,
            c_raw_day_time_interval     = 13,
            c_raw_time                  = 14,
            c_raw_time_interval         = 15,
            c_raw_year_month_interval   = 16,
            c_boolean_array             = 17,
            c_octet_array               = 18,
            c_char16_array              = 19,
            c_int16_array               = 20,
            c_int32_array               = 21,
            c_int64_array               = 22,
            c_float32_array             = 23,
            c_float64_array             = 24,
            c_object_array              = 25,
            c_sparse_array              = 26,
            c_collection                = 27,
            c_map                       = 28,
            c_user_type                 = 29
            } CppType;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_POF_CONSTANTS_HPP
