#pragma once
#include "Point.h"
#include "cadpch.h"

namespace CADMageddon
{
    class BaseObject
    {
    public:
        BaseObject(const std::string& name) : m_Name(name) {}

        std::vector<Ref<Point>>& GetControlPoints() { return m_ControlPoints; }
        bool GetIsSelected() const { return m_IsSelected; }
        void SetIsSelected(bool isSelected) { m_IsSelected = isSelected; }

        std::string GetName() const { return m_Name; }
        void SetName(std::string name) { m_Name = name; }

    protected:
        std::vector<Ref<Point>> m_ControlPoints;
        bool m_IsSelected = false;
        std::string m_Name;
    };
}