#pragma once
#include <iostream>

namespace CADMageddon
{
    typedef enum class KeyCode : uint16_t
    {
        // From glfw3.h
        Space = 32,
        Apostrophe = 39, /* ' */
        Comma = 44, /* , */
        Minus = 45, /* - */
        Period = 46, /* . */
        Slash = 47, /* / */

        D0 = 48, /* 0 */
        D1 = 49, /* 1 */
        D2 = 50, /* 2 */
        D3 = 51, /* 3 */
        D4 = 52, /* 4 */
        D5 = 53, /* 5 */
        D6 = 54, /* 6 */
        D7 = 55, /* 7 */
        D8 = 56, /* 8 */
        D9 = 57, /* 9 */

        Semicolon = 59, /* ; */
        Equal = 61, /* = */

        A = 65,
        B = 66,
        C = 67,
        D = 68,
        E = 69,
        F = 70,
        G = 71,
        H = 72,
        I = 73,
        J = 74,
        K = 75,
        L = 76,
        M = 77,
        N = 78,
        O = 79,
        P = 80,
        Q = 81,
        R = 82,
        S = 83,
        T = 84,
        U = 85,
        V = 86,
        W = 87,
        X = 88,
        Y = 89,
        Z = 90,

        LeftBracket = 91,  /* [ */
        Backslash = 92,  /* \ */
        RightBracket = 93,  /* ] */
        GraveAccent = 96,  /* ` */

        World1 = 161, /* non-US #1 */
        World2 = 162, /* non-US #2 */

        /* Function keys */
        Escape = 256,
        Enter = 257,
        Tab = 258,
        Backspace = 259,
        Insert = 260,
        Delete = 261,
        Right = 262,
        Left = 263,
        Down = 264,
        Up = 265,
        PageUp = 266,
        PageDown = 267,
        Home = 268,
        End = 269,
        CapsLock = 280,
        ScrollLock = 281,
        NumLock = 282,
        PrintScreen = 283,
        Pause = 284,
        F1 = 290,
        F2 = 291,
        F3 = 292,
        F4 = 293,
        F5 = 294,
        F6 = 295,
        F7 = 296,
        F8 = 297,
        F9 = 298,
        F10 = 299,
        F11 = 300,
        F12 = 301,
        F13 = 302,
        F14 = 303,
        F15 = 304,
        F16 = 305,
        F17 = 306,
        F18 = 307,
        F19 = 308,
        F20 = 309,
        F21 = 310,
        F22 = 311,
        F23 = 312,
        F24 = 313,
        F25 = 314,

        /* Keypad */
        KP0 = 320,
        KP1 = 321,
        KP2 = 322,
        KP3 = 323,
        KP4 = 324,
        KP5 = 325,
        KP6 = 326,
        KP7 = 327,
        KP8 = 328,
        KP9 = 329,
        KPDecimal = 330,
        KPDivide = 331,
        KPMultiply = 332,
        KPSubtract = 333,
        KPAdd = 334,
        KPEnter = 335,
        KPEqual = 336,

        LeftShift = 340,
        LeftControl = 341,
        LeftAlt = 342,
        LeftSuper = 343,
        RightShift = 344,
        RightControl = 345,
        RightAlt = 346,
        RightSuper = 347,
        Menu = 348
    } Key;

    inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
    {
        os << static_cast<int32_t>(keyCode);
        return os;
    }
}

// From glfw3.h
#define CDM_KEY_SPACE           ::CADMageddon::Key::Space
#define CDM_KEY_APOSTROPHE      ::CADMageddon::Key::Apostrophe    /* ' */
#define CDM_KEY_COMMA           ::CADMageddon::Key::Comma         /* , */
#define CDM_KEY_MINUS           ::CADMageddon::Key::Minus         /* - */
#define CDM_KEY_PERIOD          ::CADMageddon::Key::Period        /* . */
#define CDM_KEY_SLASH           ::CADMageddon::Key::Slash         /* / */
#define CDM_KEY_0               ::CADMageddon::Key::D0
#define CDM_KEY_1               ::CADMageddon::Key::D1
#define CDM_KEY_2               ::CADMageddon::Key::D2
#define CDM_KEY_3               ::CADMageddon::Key::D3
#define CDM_KEY_4               ::CADMageddon::Key::D4
#define CDM_KEY_5               ::CADMageddon::Key::D5
#define CDM_KEY_6               ::CADMageddon::Key::D6
#define CDM_KEY_7               ::CADMageddon::Key::D7
#define CDM_KEY_8               ::CADMageddon::Key::D8
#define CDM_KEY_9               ::CADMageddon::Key::D9
#define CDM_KEY_SEMICOLON       ::CADMageddon::Key::Semicolon     /* ; */
#define CDM_KEY_EQUAL           ::CADMageddon::Key::Equal         /* = */
#define CDM_KEY_A               ::CADMageddon::Key::A
#define CDM_KEY_B               ::CADMageddon::Key::B
#define CDM_KEY_C               ::CADMageddon::Key::C
#define CDM_KEY_D               ::CADMageddon::Key::D
#define CDM_KEY_E               ::CADMageddon::Key::E
#define CDM_KEY_F               ::CADMageddon::Key::F
#define CDM_KEY_G               ::CADMageddon::Key::G
#define CDM_KEY_H               ::CADMageddon::Key::H
#define CDM_KEY_I               ::CADMageddon::Key::I
#define CDM_KEY_J               ::CADMageddon::Key::J
#define CDM_KEY_K               ::CADMageddon::Key::K
#define CDM_KEY_L               ::CADMageddon::Key::L
#define CDM_KEY_M               ::CADMageddon::Key::M
#define CDM_KEY_N               ::CADMageddon::Key::N
#define CDM_KEY_O               ::CADMageddon::Key::O
#define CDM_KEY_P               ::CADMageddon::Key::P
#define CDM_KEY_Q               ::CADMageddon::Key::Q
#define CDM_KEY_R               ::CADMageddon::Key::R
#define CDM_KEY_S               ::CADMageddon::Key::S
#define CDM_KEY_T               ::CADMageddon::Key::T
#define CDM_KEY_U               ::CADMageddon::Key::U
#define CDM_KEY_V               ::CADMageddon::Key::V
#define CDM_KEY_W               ::CADMageddon::Key::W
#define CDM_KEY_X               ::CADMageddon::Key::X
#define CDM_KEY_Y               ::CADMageddon::Key::Y
#define CDM_KEY_Z               ::CADMageddon::Key::Z
#define CDM_KEY_LEFT_BRACKET    ::CADMageddon::Key::LeftBracket   /* [ */
#define CDM_KEY_BACKSLASH       ::CADMageddon::Key::Backslash     /* \ */
#define CDM_KEY_RIGHT_BRACKET   ::CADMageddon::Key::RightBracket  /* ] */
#define CDM_KEY_GRAVE_ACCENT    ::CADMageddon::Key::GraveAccent   /* ` */
#define CDM_KEY_WORLD_1         ::CADMageddon::Key::World1        /* non-US #1 */
#define CDM_KEY_WORLD_2         ::CADMageddon::Key::World2        /* non-US #2 */

/* Function keys */
#define CDM_KEY_ESCAPE          ::CADMageddon::Key::Escape
#define CDM_KEY_ENTER           ::CADMageddon::Key::Enter
#define CDM_KEY_TAB             ::CADMageddon::Key::Tab
#define CDM_KEY_BACKSPACE       ::CADMageddon::Key::Backspace
#define CDM_KEY_INSERT          ::CADMageddon::Key::Insert
#define CDM_KEY_DELETE          ::CADMageddon::Key::Delete
#define CDM_KEY_RIGHT           ::CADMageddon::Key::Right
#define CDM_KEY_LEFT            ::CADMageddon::Key::Left
#define CDM_KEY_DOWN            ::CADMageddon::Key::Down
#define CDM_KEY_UP              ::CADMageddon::Key::Up
#define CDM_KEY_PAGE_UP         ::CADMageddon::Key::PageUp
#define CDM_KEY_PAGE_DOWN       ::CADMageddon::Key::PageDown
#define CDM_KEY_HOME            ::CADMageddon::Key::Home
#define CDM_KEY_END             ::CADMageddon::Key::End
#define CDM_KEY_CAPS_LOCK       ::CADMageddon::Key::CapsLock
#define CDM_KEY_SCROLL_LOCK     ::CADMageddon::Key::ScrollLock
#define CDM_KEY_NUM_LOCK        ::CADMageddon::Key::NumLock
#define CDM_KEY_PRINT_SCREEN    ::CADMageddon::Key::PrintScreen
#define CDM_KEY_PAUSE           ::CADMageddon::Key::Pause
#define CDM_KEY_F1              ::CADMageddon::Key::F1
#define CDM_KEY_F2              ::CADMageddon::Key::F2
#define CDM_KEY_F3              ::CADMageddon::Key::F3
#define CDM_KEY_F4              ::CADMageddon::Key::F4
#define CDM_KEY_F5              ::CADMageddon::Key::F5
#define CDM_KEY_F6              ::CADMageddon::Key::F6
#define CDM_KEY_F7              ::CADMageddon::Key::F7
#define CDM_KEY_F8              ::CADMageddon::Key::F8
#define CDM_KEY_F9              ::CADMageddon::Key::F9
#define CDM_KEY_F10             ::CADMageddon::Key::F10
#define CDM_KEY_F11             ::CADMageddon::Key::F11
#define CDM_KEY_F12             ::CADMageddon::Key::F12
#define CDM_KEY_F13             ::CADMageddon::Key::F13
#define CDM_KEY_F14             ::CADMageddon::Key::F14
#define CDM_KEY_F15             ::CADMageddon::Key::F15
#define CDM_KEY_F16             ::CADMageddon::Key::F16
#define CDM_KEY_F17             ::CADMageddon::Key::F17
#define CDM_KEY_F18             ::CADMageddon::Key::F18
#define CDM_KEY_F19             ::CADMageddon::Key::F19
#define CDM_KEY_F20             ::CADMageddon::Key::F20
#define CDM_KEY_F21             ::CADMageddon::Key::F21
#define CDM_KEY_F22             ::CADMageddon::Key::F22
#define CDM_KEY_F23             ::CADMageddon::Key::F23
#define CDM_KEY_F24             ::CADMageddon::Key::F24
#define CDM_KEY_F25             ::CADMageddon::Key::F25

/* Keypad */
#define CDM_KEY_KP_0            ::CADMageddon::Key::KP0
#define CDM_KEY_KP_1            ::CADMageddon::Key::KP1
#define CDM_KEY_KP_2            ::CADMageddon::Key::KP2
#define CDM_KEY_KP_3            ::CADMageddon::Key::KP3
#define CDM_KEY_KP_4            ::CADMageddon::Key::KP4
#define CDM_KEY_KP_5            ::CADMageddon::Key::KP5
#define CDM_KEY_KP_6            ::CADMageddon::Key::KP6
#define CDM_KEY_KP_7            ::CADMageddon::Key::KP7
#define CDM_KEY_KP_8            ::CADMageddon::Key::KP8
#define CDM_KEY_KP_9            ::CADMageddon::Key::KP9
#define CDM_KEY_KP_DECIMAL      ::CADMageddon::Key::KPDecimal
#define CDM_KEY_KP_DIVIDE       ::CADMageddon::Key::KPDivide
#define CDM_KEY_KP_MULTIPLY     ::CADMageddon::Key::KPMultiply
#define CDM_KEY_KP_SUBTRACT     ::CADMageddon::Key::KPSubtract
#define CDM_KEY_KP_ADD          ::CADMageddon::Key::KPAdd
#define CDM_KEY_KP_ENTER        ::CADMageddon::Key::KPEnter
#define CDM_KEY_KP_EQUAL        ::CADMageddon::Key::KPEqual

#define CDM_KEY_LEFT_SHIFT      ::CADMageddon::Key::LeftShift
#define CDM_KEY_LEFT_CONTROL    ::CADMageddon::Key::LeftControl
#define CDM_KEY_LEFT_ALT        ::CADMageddon::Key::LeftAlt
#define CDM_KEY_LEFT_SUPER      ::CADMageddon::Key::LeftSuper
#define CDM_KEY_RIGHT_SHIFT     ::CADMageddon::Key::RightShift
#define CDM_KEY_RIGHT_CONTROL   ::CADMageddon::Key::RightControl
#define CDM_KEY_RIGHT_ALT       ::CADMageddon::Key::RightAlt
#define CDM_KEY_RIGHT_SUPER     ::CADMageddon::Key::RightSuper
#define CDM_KEY_MENU            ::CADMageddon::Key::Menu
