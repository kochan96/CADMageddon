#include "InspectorControl.h"
#include "TorusComponentEditor.h"
#include "ColorComponentEditor.h"
#include "TransformComponentEditor.h"

namespace CADMageddon
{
    void Inspector::Begin()
    {
        ImGui::Begin("Inspector");
    }

    void Inspector::End()
    {
        ImGui::End();
    }

    void Inspector::Render(Entity& entity)
    {
        if (entity.HasComponent<TorusComponent>())
        {
            TorusComponentEditor(entity.GetComponent<TorusComponent>());
        }

        if (entity.HasComponent<TransformComponent>())
        {
            TransformComponentEditor(entity.GetComponent<TransformComponent>());
        }
    }
}