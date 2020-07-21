#pragma once
#include <iostream>
namespace CADMageddon
{
	typedef enum class MouseCode : uint16_t
	{
		// From glfw3.h
		Button0 = 0,
		Button1 = 1,
		Button2 = 2,
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Button6 = 6,
		Button7 = 7,

		ButtonLast = Button7,
		ButtonLeft = Button0,
		ButtonRight = Button1,
		ButtonMiddle = Button2
	} Mouse;

	inline std::ostream& operator<<(std::ostream& os, MouseCode mouseCode)
	{
		os << static_cast<int32_t>(mouseCode);
		return os;
	}
}

#define CDM_MOUSE_BUTTON_0      ::CADMageddon::Mouse::Button0
#define CDM_MOUSE_BUTTON_1      ::CADMageddon::Mouse::Button1
#define CDM_MOUSE_BUTTON_2      ::CADMageddon::Mouse::Button2
#define CDM_MOUSE_BUTTON_3      ::CADMageddon::Mouse::Button3
#define CDM_MOUSE_BUTTON_4      ::CADMageddon::Mouse::Button4
#define CDM_MOUSE_BUTTON_5      ::CADMageddon::Mouse::Button5
#define CDM_MOUSE_BUTTON_6      ::CADMageddon::Mouse::Button6
#define CDM_MOUSE_BUTTON_7      ::CADMageddon::Mouse::Button7
#define CDM_MOUSE_BUTTON_LAST   ::CADMageddon::Mouse::ButtonLast
#define CDM_MOUSE_BUTTON_LEFT   ::CADMageddon::Mouse::ButtonLeft
#define CDM_MOUSE_BUTTON_RIGHT  ::CADMageddon::Mouse::ButtonRight
#define CDM_MOUSE_BUTTON_MIDDLE ::CADMageddon::Mouse::ButtonMiddle