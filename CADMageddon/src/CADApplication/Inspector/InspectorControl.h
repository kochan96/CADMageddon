#pragma once
#include "Scene\Entity.h"

namespace CADMageddon
{
    class Inspector
    {
    public:
        static void Begin();
        static void Render(Entity& entity);
        static void End();
    };
}