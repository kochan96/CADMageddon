#pragma once
#include "BezierPatch.h"

namespace CADMageddon
{
    enum class Border
    {
        B30, B012, B1215, B153, B03, B120, B1512, B315, None
    };

    class GregoryPatch : public BaseObject
    {
    public:
        static Ref<GregoryPatch> Create(Ref<BezierPatch> b1, Ref<BezierPatch> b2, Ref<BezierPatch> b3, Ref<Point> commonPoints[3]);

        Ref<BezierPatch> GetB1() const { return b1; }
        Ref<BezierPatch> GetB2() const { return b2; }
        Ref<BezierPatch> GetB3() const { return b3; }

    private:
        GregoryPatch(std::string name);
        Border GetBorderEnum(Ref<BezierPatch> b1, Ref<Point> p0, Ref<Point> p1);
        std::vector<glm::vec3> GetBorderPoints();



    private:
        Ref<BezierPatch> b1;
        Ref<BezierPatch> b2;
        Ref<BezierPatch> b3;
    };
}