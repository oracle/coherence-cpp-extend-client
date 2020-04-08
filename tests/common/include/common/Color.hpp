/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TEST_COLOR_HPP
#define COH_TEST_COLOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"

COH_OPEN_NAMESPACE2(common, test)

using namespace coherence::lang;

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;

/**
 * Designed to map to the Java test.Color enum class.
 */
class Color
    : public class_spec<Color,
        extends<Object>,
        implements<PortableObject> >
    {
    friend class factory<Color>;

    // ----- constructors ---------------------------------------------------

    protected:
        Color()
            : f_vsColor(self())
            {
            }

    private:
        Color(String::View vsColor)
            : f_vsColor(self(), vsColor)
            {
            }

    // ----- PortableObject interface ---------------------------------------

    public:
        virtual void readExternal(PofReader::Handle hIn)
            {
            initialize(f_vsColor, hIn->readString(0));
            }

        virtual void writeExternal(PofWriter::Handle hOut) const
            {
            hOut->writeString(0, f_vsColor);
            }

    // ----- Object overrides -----------------------------------------------

    public:
        virtual bool equals(Object::View v) const
            {
            if (this == v)
                {
                return true;
                }
            if (instanceof<Color::View>(v) && Class::getClassName(this)->equals(Class::getClassName(v)))
                {
                return f_vsColor->equals(cast<Color::View>(v)->f_vsColor);
                }
            return false;
            }

        virtual size32_t hashCode() const
            {
            return f_vsColor->hashCode();
            }

        virtual TypedHandle<const String> toString() const
            {
            return f_vsColor;
            }

    // ----- static constants -------------------------------------------

    public:
        static Color::View BRINDLE()
            {
            static FinalView<Color> vBrindle(System::common(), Color::create("BRINDLE"));
            return vBrindle;
            }
        static Color::View FAWN()
            {
            static FinalView<Color> vFawn(System::common(), Color::create("FAWN"));
            return vFawn;
            }
        static Color::View BLACK()
            {
            static FinalView<Color> vBlack(System::common(), Color::create("BLACK"));
            return vBlack;
            }
        static Color::View WHITE()
            {
            static FinalView<Color> vWhite(System::common(), Color::create("WHITE"));
            return vWhite;
            }
        static Color::View BROWN()
            {
            static FinalView<Color> vBrown(System::common(), Color::create("BROWN"));
            return vBrown;
            }
        static Color::View GRAY()
            {
            static FinalView<Color> vGray(System::common(), Color::create("GRAY"));
            return vGray;
            }
        static Color::View YELLOW()
            {
            static FinalView<Color> vYellow(System::common(), Color::create("YELLOW"));
            return vYellow;
            }

    // ----- data members ---------------------------------------------------

    private:
        FinalView<String> f_vsColor;
    };

COH_STATIC_INIT(Color::BRINDLE());
COH_STATIC_INIT(Color::FAWN());
COH_STATIC_INIT(Color::BLACK());
COH_STATIC_INIT(Color::WHITE());
COH_STATIC_INIT(Color::BROWN());
COH_STATIC_INIT(Color::GRAY());
COH_STATIC_INIT(Color::YELLOW());

COH_REGISTER_PORTABLE_CLASS(51720, Color);

COH_CLOSE_NAMESPACE2

#endif // COH_TEST_COLOR_HPP

