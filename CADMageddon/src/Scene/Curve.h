#pragma once
#include "BaseObject.h"

namespace CADMageddon
{
    class Curve : public BaseObject
    {
    public:
        Curve(std::string name) :BaseObject(name) {}
        void AddControlPoint(Ref<Point> point);
        void RemoveControlPoint(Ref<Point> point);
    };
}