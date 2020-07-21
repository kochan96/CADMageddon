#pragma once

#include "Base.h"
#include "Timestep.h"
#include "Events/Event.h"


namespace CADMageddon
{
    class Layer
    {
    public:
        Layer(const std::string& name = "Layer");
        virtual ~Layer() = default;

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate(Timestep ts) {}
        virtual void OnEvent(Event& event) {}

        const std::string& GetName() const { return m_DebugName; };

    private:
        std::string m_DebugName;

    };
}