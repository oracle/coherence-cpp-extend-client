/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include "coherence/util/Binary.hpp"
#include "coherence/util/BinaryWriteBuffer.hpp"

#include "cxxtest/TestSuite.h"

using namespace coherence::lang;

using coherence::util::Binary;
using coherence::util::BinaryWriteBuffer;


/**
* Test suite for the Binary class.
*/
class BinaryTest : public CxxTest::TestSuite
    {
    public:
        Array<octet_t>::Handle createByteArray(size32_t c, octet_t b)
            {
            Array<octet_t>::Handle hab  = Array<octet_t>::create(c);
            for (size32_t i = 0; i < c; ++i)
                {
                hab[i] = (octet_t) b;
                }
            return hab;
            }

        /**
        * Test Binary creation.
        */
        void testCreate()
            {
            Array<octet_t>::Handle hab  = createByteArray(10, (octet_t) 7);
            Binary::View           vBin;

            vBin = Binary::getEmptyBinary();
            TS_ASSERT(vBin->length() == 0);

            vBin = Binary::create();
            TS_ASSERT(vBin->length() == 0);
            TS_ASSERT(vBin->equals(Binary::getEmptyBinary()));

            vBin = Binary::create(cast<Array<octet_t>::View>(Binary::getEmptyOctetArray()));
            TS_ASSERT(vBin->length() == 0);
            TS_ASSERT(vBin->equals(Binary::getEmptyBinary()));

            TS_ASSERT_THROWS(vBin = Binary::create((Array<octet_t>::View) NULL), NullPointerException::View);

            // create Binary from Array Handle - mutable Array
            vBin = Binary::create(cast<Array<octet_t>::View>(hab));
            TS_ASSERT(vBin->length() == hab->length);
            TS_ASSERT(vBin->toOctetArray()->isImmutable());
            TS_ASSERT(vBin->toOctetArray() != hab);

            vBin = Binary::create(hab, 0);
            TS_ASSERT(vBin->length() == hab->length);
            TS_ASSERT(vBin->toOctetArray()->isImmutable());
            TS_ASSERT(vBin->toOctetArray() != hab);

            vBin = Binary::create(hab, 1);
            TS_ASSERT(vBin->length() == hab->length - 1);
            TS_ASSERT(vBin->toOctetArray()->isImmutable());
            TS_ASSERT(vBin->toOctetArray() != hab);

            vBin = Binary::create(hab, 0, 10);
            TS_ASSERT(vBin->length() == hab->length);
            TS_ASSERT(vBin->toOctetArray()->isImmutable());
            TS_ASSERT(vBin->toOctetArray() != hab);

            vBin = Binary::create(hab, 10);
            TS_ASSERT(vBin->length() == 0);
            TS_ASSERT(vBin->toOctetArray()->length == 0);

            vBin = Binary::create(hab, 9, 0);
            TS_ASSERT(vBin->length() == 0);

            vBin = Binary::create(hab, 1, 5);
            TS_ASSERT(vBin->length() == 5);
            TS_ASSERT(vBin->toOctetArray()->isImmutable());
            TS_ASSERT(vBin->toOctetArray() != hab);

            TS_ASSERT_THROWS(vBin = Binary::create(hab, 11), IndexOutOfBoundsException::View);
            TS_ASSERT_THROWS(vBin = Binary::create(hab, 0, 11), IndexOutOfBoundsException::View);

            // equals test between Binary created from Array Handle and Array View
            vBin = Binary::create(hab, 0, 10);
            Array<octet_t>::View vab = createByteArray(10, (octet_t) 7);
            Binary::View vBin2 = Binary::create(vab);
            TS_ASSERT(vBin->equals(vBin2));

            // create Binary from Array View - immutable Array
            vBin = Binary::create(vab);
            TS_ASSERT(vBin->length() == vab->length);
            TS_ASSERT(vBin->toOctetArray()->isImmutable());
            TS_ASSERT(vBin->toOctetArray() == vab);

            vBin = Binary::create(vab, 0);
            TS_ASSERT(vBin->length() == vab->length);
            TS_ASSERT(vBin->toOctetArray()->isImmutable());
            TS_ASSERT(vBin->toOctetArray() == vab);

            vBin = Binary::create(vab, 1);
            TS_ASSERT(vBin->length() == vab->length - 1);
            TS_ASSERT(vBin->toOctetArray()->isImmutable());
            TS_ASSERT(vBin->toOctetArray() != vab);

            vBin = Binary::create(vab, 0, 10);
            TS_ASSERT(vBin->length() == vab->length);
            TS_ASSERT(vBin->toOctetArray()->isImmutable());
            TS_ASSERT(vBin->toOctetArray() == vab);

            vBin = Binary::create(vab, 10);
            TS_ASSERT(vBin->length() == 0);
            TS_ASSERT(vBin->toOctetArray()->length == 0);

            vBin = Binary::create(vab, 9, 0);
            TS_ASSERT(vBin->length() == 0);

            vBin = Binary::create(vab, 1, 5);
            TS_ASSERT(vBin->length() == 5);
            TS_ASSERT(vBin->toOctetArray()->isImmutable());
            TS_ASSERT(vBin->toOctetArray() != vab);

            TS_ASSERT_THROWS(vBin = Binary::create(vab, 11), IndexOutOfBoundsException::View);
            TS_ASSERT_THROWS(vBin = Binary::create(vab, 0, 11), IndexOutOfBoundsException::View);

            BinaryWriteBuffer::Handle hwb = BinaryWriteBuffer::create(10);
            hwb->write(0, vab);
            vBin  = Binary::create(cast<BinaryWriteBuffer::View>(hwb));
            vBin2 = Binary::create(vab);
            TS_ASSERT(vBin->length() == 10);
            TS_ASSERT(vBin->equals(vBin2));
            }

        /**
        * Test calculateNaturalPartition().
        */
        void testCalculateNaturalPartition()
            {
            Array<octet_t>::Handle hab = Array<octet_t>::create(1);
            hab[0] = 124;

            Binary::View vBin  = Binary::create(hab, 0);
            int32_t      nHash = vBin->hashCode();
            TS_ASSERT_EQUALS(nHash,     vBin->calculateNaturalPartition(0));
            TS_ASSERT_EQUALS(nHash % 7, vBin->calculateNaturalPartition(7));

            // Check portability. Verify that partition values are consistent
            // with those generated by Java and .NET
            TS_ASSERT_EQUALS(-1, Binary::getEmptyBinary()->calculateNaturalPartition(0));

            hab = Array<octet_t>::create(256);
            for (int i = 0, b = -128; i < 256; ++i, ++b)
                {
                hab[i] = (octet_t) b;
                }

            int32_t aiExpected[] =
                {
                -1069182126, -1220369468,  776729214,   1498202856, -953657525,  -1339070499,  690370151,   1579222769,
                -828499104,  -1181144074,  546339404,   1469532890, -906764423,  -1091244049,  670940757,   1358597827,
                -571309258,  -1426738272,  872210970,   1157354124, -627095761,  -1382516807,  881927683,   1133909653,
                -752284924,  -1540473966,  1025993256,  1243634366, -733688035,  -1555824757,  977972785,   1296932519,
                -81022054,   -1943239924,  354800310,   1646453280, -62490749,   -1958656235,  306714287,   1699685945,
                -168805464,  -2097738946,  469654148,   1828284946, -224526415,  -2053451993,  479436445,   1804905995,
                -425942018,  -1852075160,  143835858,   2140533316, -504272921,  -1762240655,  268371659,   2029532765,
                -397988916,  -1623188646,  105466592,   1900968566, -282398763,  -1741824189,  19173113,    1982053999,
                -1231433022, -1046551980,  1486337006,  797999992,  -1309403429, -957143475,   1610250231,  687687521,
                -1203610896, -817534362,   1447836636,  558566218,  -1087398167, -936857985,   1361182661,  640077651,
                -1422998874, -601230800,   1159766922,  841454364,  -1404893505, -616286679,   1112369043,  894064389,
                -1510651244, -755860990,   1274751928,  1023154990, -1567060339, -710951397,   1284960161,  999415607,
                -1913130486, -84884836,    1677300518,  352232368,  -1969605101, -40040827,    1687443263,  328427433,
                -2094237128, -198489426,   1830951700,  438643586,  -2076066271, -213479753,   1783619341,  491319195,
                -1874795922, -414723336,   2119074626,  155825108,  -1758648713, -534112543,   2032355163,  237270989,
                -1633981860, -375629110,   1888815984,  127024102,  -1711886779, -286155053,   2012794729,  16777215,
                 771559538,   1526341860, -1007455906, -1259060792,  714134635,   1570235645, -996231865,  -1281784367,
                 589731904,   1411492054, -852952724,  -1171273222,  608918617,   1397517519, -901431947,  -1119744541,
                 810156054,   1196241024, -565944006,  -1455205972,  925352975,   1075901593, -651582173,  -1372678731,
                 1049724964,  1234614450, -794826488,  -1483155042,  972835901,   1325104299, -671994607,  -1594548857,
                 378745018,   1637089324, -123907690,  -1885708032,  301921443,   1727644725, -1010289,    -1997036263,
                 407419016,   1867483166, -163128924,  -2126386894,  522550417,   1747078151, -248832579,  -2043925205,
                 186917086,   2082672712, -450215438,  -1842515612,  206169287,   2068763729, -498629141,  -1790921347,
                 100641004,   1928894586, -336475712,  -1661535914,  43150581,    1972722787, -325317159,  -1684325041,
                 1528910306,  740712820,  -1255198514, -1037565864,  1548523003,  726377837,  -1304234793, -985283519,
                 1442503120,  587065670,  -1141589764, -856455062,   1385635273,  630205791,  -1130791707, -878818189,
                 1184252294,  831615248,  -1466425174, -543223748,   1107002783,  922531081,  -1342839629, -655174619,
                 1213057460,  1061878050, -1505515368, -784033778,   1327500717,  942095675,  -1590793087, -701932521,
                 1615819050,  390611388,  -1908338682, -112844656,   1730327859,  270894501,  -1993550817, -30677879,
                 1855256856,  429115790,  -2137352140, -140662622,   1777941761,  519966103,  -2013832147, -252678981,
                 2113429838,  184504792,  -1812594590, -453955340,   2056627543,  227710401,  -1801730949, -476252947,
                 1931733372,  69523946,   -1657960368, -366298938,   1951280485,  55123443,   -1707062199, -314082081
                };

            TS_ASSERT_EQUALS(-2018391514, Binary::create(hab, 0)->calculateNaturalPartition(0));

            for (size32_t i = 0; i < hab->length; ++i)
                {
                vBin  = Binary::create(hab, i, 1);
                nHash = vBin->hashCode();

                int32_t nPart = vBin->calculateNaturalPartition(0);
                TS_ASSERT_EQUALS(aiExpected[i], nPart);
                TS_ASSERT_EQUALS(nHash,         nPart);
                }
            }

        /**
        * Test hashCode().
        */
        void testHashCode()
            {
            Array<octet_t>::Handle hab  = createByteArray(10, (octet_t) 7);
            Binary::View           vBin = Binary::create(hab, 0, 10);

            TS_ASSERT(vBin->hashCode() == hab->hashCode());

            vBin = Binary::create(hab, 1, 5);

            TS_ASSERT(vBin->hashCode() == hab->subArray(1, 6)->hashCode());

            Array<octet_t>::View vab  = createByteArray(10, (octet_t) 7);
            vBin = Binary::create(vab, 0, 10);

            TS_ASSERT(vBin->hashCode() == vab->hashCode());

            vBin = Binary::create(vab, 1, 5);

            TS_ASSERT(vBin->hashCode() == vab->subArray(1, 6)->hashCode());
            }

        /**
        * Test equals().
        */
        void testEquals()
            {
            Array<octet_t>::Handle hab  = createByteArray(10, (octet_t) 7);
            Binary::View           vBin = Binary::create(hab, 0, 10);

            TS_ASSERT(vBin->equals(vBin));
            TS_ASSERT(vBin->equals(Binary::create(hab, 0, 10)));

            vBin = Binary::create(hab, 1, 5);

            TS_ASSERT(vBin->equals(Binary::create(hab->subArray(1, 6), 0, 5)));

            Array<octet_t>::View vab  = createByteArray(10, (octet_t) 7);
            vBin = Binary::create(vab, 0, 10);

            TS_ASSERT(vBin->equals(Binary::create(vab, 0, 10)));

            vBin = Binary::create(vab, 1, 5);

            TS_ASSERT(vBin->equals(Binary::create(hab->subArray(1, 6), 0, 5)));
            }

        /**
        * Test clone().
        */
        void testClone()
            {
            Array<octet_t>::View vab  = createByteArray(10, (octet_t) 7);
            Binary::View         vBin = Binary::create(vab, 0, 10);
            Binary::Handle       hBin = cast<Binary::Handle>(vBin->clone());

            TS_ASSERT(vBin != hBin);
            TS_ASSERT(vBin->equals(hBin));
            TS_ASSERT(vBin->hashCode() == hBin->hashCode());
            }

        /**
        * Test isImmutable().
        */
        void testIsImmutable()
            {
            Array<octet_t>::View vab  = createByteArray(10, (octet_t) 7);
            Binary::View         vBin = Binary::create(vab, 0, 10);

            TS_ASSERT(vBin->isImmutable());
            }

        /**
        * Test getReadBuffer().
        */
        void testGetReadBuffer()
            {
            Array<octet_t>::View vab  = createByteArray(10, (octet_t) 7);
            Binary::View         vBin = Binary::create(vab, 0, 10);

            TS_ASSERT(vBin == vBin->getReadBuffer(0, 10));
            TS_ASSERT(vBin->getReadBuffer(1, 5)->toOctetArray()->equals(
                    vab->subArray(1, 6)));
            }
    };
