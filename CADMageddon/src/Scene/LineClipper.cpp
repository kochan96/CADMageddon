#include "LineClipper.h"

namespace CADMageddon
{

    static const int INSIDE = 0; // 0000
    static const int LEFT = 1;   // 0001
    static const int RIGHT = 2;  // 0010
    static const int BOTTOM = 4; // 0100
    static const int TOP = 8;    // 1000

    void LineClipper::ClipLine(glm::vec2 p0, glm::vec2 p1, glm::vec2 xBoundary, glm::vec2 yBoundary, glm::vec2& intersection, glm::vec2& intersection2)
    {
        OutCode outcode0 = GetOutCode(p0, xBoundary, yBoundary);
        OutCode outcode1 = GetOutCode(p1, xBoundary, yBoundary);

        while (true)
        {
            if (outcode0 == INSIDE && outcode1 == INSIDE)
            {
                break;
            }

            OutCode outCode = outcode0 == INSIDE ? outcode1 : outcode0;
            if (outCode & TOP) {           // point is above the clip window
                intersection.x = p0.x + (p1.x - p0.x) * (yBoundary.y - p0.y) / (p1.y - p0.y);
                intersection.y = yBoundary.y;

                intersection2.x = intersection.x;
                intersection2.y = yBoundary.x;
            }
            else if (outCode & BOTTOM) { // point is below the clip window
                intersection.x = p0.x + (p1.x - p0.x) * (yBoundary.x - p0.y) / (p1.y - p0.y);
                intersection.y = yBoundary.x;
                intersection2.x = intersection.x;
                intersection2.y = yBoundary.y;
            }
            else if (outCode & RIGHT) {  // point is to the right of clip window
                intersection.y = p0.y + (p1.y - p0.y) * (xBoundary.y - p0.x) / (p1.x - p0.x);
                intersection.x = xBoundary.y;
                intersection2.y = intersection.y;
                intersection2.x = xBoundary.x;
            }
            else if (outCode & LEFT) {   // point is to the left of clip window
                intersection.y = p0.y + (p1.y - p0.y) * (xBoundary.x - p0.x) / (p1.x - p0.x);
                intersection.x = xBoundary.x;
                intersection2.y = intersection.y;
                intersection2.x = xBoundary.y;
            }

            // Now we move outside point to intersection point to clip
            // and get ready for next pass.
            if (outCode == outcode0) {
                p1.x = intersection.x;
                p1.y = intersection.y;
                outcode0 = GetOutCode(p0, xBoundary, yBoundary);
            }
            else {
                p1.x = intersection.x;
                p1.y = intersection.y;
                outcode1 = GetOutCode(p1, xBoundary, yBoundary);
            }
        }
    }

    LineClipper::OutCode LineClipper::GetOutCode(glm::vec2 p, glm::vec2 xBoundary, glm::vec2 yBoundary)
    {
        OutCode code = INSIDE;

        if (p.x < xBoundary.x)
            code |= LEFT;
        else if (p.x > xBoundary.y)
            code |= RIGHT;

        if (p.y < yBoundary.x)
            code |= BOTTOM;
        else if (p.y > yBoundary.y)
            code |= TOP;

        return code;
    }
}

