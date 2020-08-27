#pragma once
#include "BaseObject.h"

namespace CADMageddon
{
    class Curve : public BaseObject
    {
    public:
        void AddControlPoint(Ref<Point> point);
        void RemoveControlPoint(Ref<Point> point);
    };
}