#include "GregoryPatch.h"

namespace CADMageddon
{
    GregoryPatch::GregoryPatch(std::string name) : BaseObject(name)
    {
    }

    Ref<GregoryPatch> CADMageddon::GregoryPatch::Create(Ref<BezierPatch> b1, Ref<BezierPatch> b2, Ref<BezierPatch> b3, Ref<Point> commonPoints[3])
    {
        static int gregoryCount = 0;
        auto gregory = Ref<GregoryPatch>(new GregoryPatch("Gregory_" + std::to_string(gregoryCount)));

        gregory->b1 = b1;
        gregory->b2 = b2;
        gregory->b3 = b3;

        return gregory;
    }
}
