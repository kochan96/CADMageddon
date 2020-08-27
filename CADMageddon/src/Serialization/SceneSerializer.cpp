#include "SceneSerializer.h"
#include "tinyxml2.h"
#include "Core\Base.h"

namespace CADMageddon
{
    std::unordered_map<std::string, Ref<Point>> SceneSerializer::m_Points;
    std::unordered_map<std::string, Ref<Point>> SceneSerializer::m_NotAssignedPoints;

    bool SceneSerializer::SerializeScene(const Scene& scene, const std::string& filePath)
    {
        tinyxml2::XMLDocument doc;
        doc.NewDeclaration();
        auto sceneElement = doc.NewElement("Scene");
        sceneElement->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
        sceneElement->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
        sceneElement->SetAttribute("xmlns", "http://mini.pw.edu.pl/mg1");


        for (auto point : scene.GetPoints())
        {
            auto pointElement = sceneElement->InsertNewChildElement("Point");
            pointElement->SetAttribute("Name", point->GetName().c_str());
            auto positionElement = pointElement->InsertNewChildElement("Position");
            auto position = point->GetPosition();
            positionElement->SetAttribute("X", position.x);
            positionElement->SetAttribute("Y", position.y);
            positionElement->SetAttribute("Z", position.z);
        }

        for (auto bezierCurve : scene.GetBezierC0())
        {
            auto bezierElement = sceneElement->InsertNewChildElement("BezierC0");
            bezierElement->SetAttribute("Name", bezierCurve->GetName().c_str());
            bezierElement->SetAttribute("ShowControlPolygon", bezierCurve->GetShowPolygon());
            auto pointRefsElement = bezierElement->InsertNewChildElement("Points");

            for (auto point : bezierCurve->GetControlPoints())
            {
                auto pointRefElement = pointRefsElement->InsertNewChildElement("PointRef");
                pointRefElement->SetAttribute("Name", point->GetName().c_str());
            }
        }

        for (auto bSplineCurve : scene.GetBSpline())
        {
            auto bezierElement = sceneElement->InsertNewChildElement("BezierC2");
            bezierElement->SetAttribute("Name", bSplineCurve->GetName().c_str());
            bezierElement->SetAttribute("ShowControlPolygon", bSplineCurve->GetShowBSplinePolygon());
            auto pointRefsElement = bezierElement->InsertNewChildElement("Points");

            for (auto point : bSplineCurve->GetControlPoints())
            {
                auto pointRefElement = pointRefsElement->InsertNewChildElement("PointRef");
                pointRefElement->SetAttribute("Name", point->GetName().c_str());
            }
        }

        for (auto interpolated : scene.GetInterpolated())
        {
            auto bezierElement = sceneElement->InsertNewChildElement("BezierInter");
            bezierElement->SetAttribute("Name", interpolated->GetName().c_str());
            bezierElement->SetAttribute("ShowControlPolygon", interpolated->GetShowPolygon());
            auto pointRefsElement = bezierElement->InsertNewChildElement("Points");

            for (auto point : interpolated->GetControlPoints())
            {
                auto pointRefElement = pointRefsElement->InsertNewChildElement("PointRef");
                pointRefElement->SetAttribute("Name", point->GetName().c_str());
            }
        }

        for (auto torus : scene.GetTorus())
        {
            auto torusElement = sceneElement->InsertNewChildElement("Torus");
            torusElement->SetAttribute("Name", torus->GetName().c_str());

            auto torusParameters = torus->GetTorusParameters();

            torusElement->SetAttribute("MajorRadius", torusParameters.MajorRadius);
            torusElement->SetAttribute("MinorRadius", torusParameters.MinorRadius);

            torusElement->SetAttribute("VerticalSlices", torusParameters.MajorRadiusCount);
            torusElement->SetAttribute("HorizontalSlices", torusParameters.MinorRadiusCount);

            auto torusPosition = torus->GetTransform()->GetMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            auto torusRotation = torus->GetTransform()->Rotation;
            auto torusScale = torus->GetTransform()->Scale;

            auto positionElement = torusElement->InsertNewChildElement("Position");
            positionElement->SetAttribute("X", torusPosition.x);
            positionElement->SetAttribute("Y", torusPosition.y);
            positionElement->SetAttribute("Z", torusPosition.z);

            auto rotationElement = torusElement->InsertNewChildElement("Rotation");
            rotationElement->SetAttribute("X", torusRotation.x);
            rotationElement->SetAttribute("Y", torusRotation.y);
            rotationElement->SetAttribute("Z", torusRotation.z);

            auto scaleElement = torusElement->InsertNewChildElement("Scale");
            scaleElement->SetAttribute("X", torusScale.x);
            scaleElement->SetAttribute("Y", torusScale.y);
            scaleElement->SetAttribute("Z", torusScale.z);
        }

        for (auto bezierPatch : scene.GetBezierPatch())
        {
            auto bezierPatchElement = sceneElement->InsertNewChildElement("PatchC0");
            bezierPatchElement->SetAttribute("Name", bezierPatch->GetName().c_str());
            bezierPatchElement->SetAttribute("ShowControlPolygon", bezierPatch->GetShowPolygon());
            bezierPatchElement->SetAttribute("RowSlices", bezierPatch->GetUDivisionCount());
            bezierPatchElement->SetAttribute("ColumnSlices", bezierPatch->GetVDivisionCount());
            bezierPatchElement->SetAttribute("WrapDirection", bezierPatch->GetIsCylinder() ? "Column" : "None");
            auto pointRefsElement = bezierPatchElement->InsertNewChildElement("Points");

            int verticesCountX = 0;
            if (bezierPatch->GetIsCylinder())
                verticesCountX = bezierPatch->GetPatchCountX() * 3;
            else
                verticesCountX = bezierPatch->GetPatchCountX() * 3 + 1;

            auto points = bezierPatch->GetControlPoints();
            for (int i = 0; i < points.size(); i++)
            {
                auto pointRefElement = pointRefsElement->InsertNewChildElement("PointRef");
                pointRefElement->SetAttribute("Name", points[i]->GetName().c_str());
                pointRefElement->SetAttribute("Row", i / verticesCountX);
                pointRefElement->SetAttribute("Column", i % verticesCountX);
            }
        }

        for (auto bSplinePatch : scene.GetBSplinePatch())
        {
            auto bezierPatchElement = sceneElement->InsertNewChildElement("PatchC2");
            bezierPatchElement->SetAttribute("Name", bSplinePatch->GetName().c_str());
            bezierPatchElement->SetAttribute("ShowControlPolygon", bSplinePatch->GetShowPolygon());
            bezierPatchElement->SetAttribute("RowSlices", bSplinePatch->GetUDivisionCount());
            bezierPatchElement->SetAttribute("ColumnSlices", bSplinePatch->GetVDivisionCount());
            bezierPatchElement->SetAttribute("WrapDirection", bSplinePatch->GetIsCylinder() ? "Column" : "None");
            auto pointRefsElement = bezierPatchElement->InsertNewChildElement("Points");

            int verticesCountX = 0;
            if (bSplinePatch->GetIsCylinder())
                verticesCountX = bSplinePatch->GetPatchCountX();
            else
                verticesCountX = bSplinePatch->GetPatchCountX() + 3;

            auto points = bSplinePatch->GetControlPoints();
            for (int i = 0; i < points.size(); i++)
            {
                auto pointRefElement = pointRefsElement->InsertNewChildElement("PointRef");
                pointRefElement->SetAttribute("Name", points[i]->GetName().c_str());
                pointRefElement->SetAttribute("Row", i / verticesCountX);
                pointRefElement->SetAttribute("Column", i % verticesCountX);
            }
        }


        doc.LinkEndChild(sceneElement);
        return doc.SaveFile(filePath.c_str()) == tinyxml2::XML_SUCCESS;
    }

    Ref<Scene> SceneSerializer::LoadScene(const std::string& filePath)
    {
        tinyxml2::XMLDocument doc;
        auto scene = CreateRef<Scene>();

        doc.LoadFile(filePath.c_str());
        auto sceneElement = doc.FirstChildElement("Scene");

        LoadPoints(*scene, sceneElement);

        for (auto elem = sceneElement->FirstChildElement(); elem != nullptr; elem = elem->NextSiblingElement()) {

            std::string type = elem->Name();
            if (type == "Point") {
            }
            else if (type == "BezierC0") {
                LoadBezierC0(*scene, elem);
            }
            else if (type == "BezierC2") {
                LoadBSpline(*scene, elem);
            }
            else if (type == "BezierInter")
            {
                LoadInterpolatedCurve(*scene, elem);
            }
            else if (type == "Torus")
            {
                LoadTorus(*scene, elem);
            }
            else if (type == "PatchC0")
            {
                LoadBezierPatch(*scene, elem);
            }
            else if (type == "PatchC2")
            {
                LoadBSplinePatch(*scene, elem);
            }
        }

        for (auto point : m_NotAssignedPoints)
        {
            scene->m_FreePoints.push_back(point.second);
        }

        m_NotAssignedPoints.clear();
        m_Points.clear();

        return scene;
    }

    void SceneSerializer::LoadPoints(Scene& scene, tinyxml2::XMLElement* sceneElement)
    {
        //TODO check for unique names;

        for (auto elem = sceneElement->FirstChildElement("Point"); elem != nullptr; elem = elem->NextSiblingElement("Point"))
        {
            std::string name = elem->Attribute("Name");
            glm::vec3 pos = ReadVector(elem->FirstChildElement("Position"));
            auto point = CreateRef<Point>(pos, name);
            scene.m_Points.push_back(point);
            m_NotAssignedPoints.insert({ name,point });
            m_Points.insert({ name,point });
        }
    }

    void SceneSerializer::LoadTorus(Scene& scene, tinyxml2::XMLElement* torusElement)
    {
        std::string name = torusElement->Attribute("Name");
        glm::vec3 position = ReadVector(torusElement->FirstChildElement("Position"));
        glm::vec3 rotation = ReadVector(torusElement->FirstChildElement("Rotation"));
        glm::vec3 scale = ReadVector(torusElement->FirstChildElement("Scale"));
        TorusParameters torusParameters;
        torusParameters.MajorRadius = torusElement->FloatAttribute("MajorRadius");
        torusParameters.MinorRadius = torusElement->FloatAttribute("MinorRadius");
        torusParameters.MajorRadiusCount = torusElement->IntAttribute("HorizontalSlices");
        torusParameters.MinorRadiusCount = torusElement->IntAttribute("VerticalSlices");

        auto torus = CreateRef<Torus>(position, name);
        torus->GetTorusParameters() = torusParameters;
        torus->GetTransform()->Rotation = rotation;
        torus->GetTransform()->Scale = scale;

        torus->RecalculateMesh();
        scene.m_Torus.push_back(torus);
    }

    void SceneSerializer::LoadBezierC0(Scene& scene, tinyxml2::XMLElement* bezierC0Element)
    {
        std::string name = bezierC0Element->Attribute("Name");
        auto bezierC0 = CreateRef<BezierC0>(name);
        auto pointsRefs = bezierC0Element->FirstChildElement();
        for (auto elem = pointsRefs->FirstChildElement(); elem != nullptr; elem = elem->NextSiblingElement())
        {
            std::string pointName = elem->Attribute("Name");
            auto point = FindPointByName(pointName);
            bezierC0->AddControlPoint(point);
        }

        scene.m_BezierC0.push_back(bezierC0);
    }

    void SceneSerializer::LoadBSpline(Scene& scene, tinyxml2::XMLElement* bSplineElement)
    {
        std::string name = bSplineElement->Attribute("Name");
        auto bSpline = CreateRef<BSpline>(name);
        auto pointsRefs = bSplineElement->FirstChildElement();
        for (auto elem = pointsRefs->FirstChildElement(); elem != nullptr; elem = elem->NextSiblingElement())
        {
            std::string pointName = elem->Attribute("Name");
            auto point = FindPointByName(pointName);
            bSpline->AddControlPoint(point);
        }

        scene.m_BSpline.push_back(bSpline);
    }

    void SceneSerializer::LoadInterpolatedCurve(Scene& scene, tinyxml2::XMLElement* interpolatedCurveElement)
    {
        std::string name = interpolatedCurveElement->Attribute("Name");
        auto interpolated = CreateRef<InterpolatedCurve>(name);
        auto pointsRefs = interpolatedCurveElement->FirstChildElement();
        for (auto elem = pointsRefs->FirstChildElement(); elem != nullptr; elem = elem->NextSiblingElement())
        {
            std::string pointName = elem->Attribute("Name");
            auto point = FindPointByName(pointName);
            interpolated->AddControlPoint(point);
        }

        scene.m_InterpolatedCurve.push_back(interpolated);
    }

    void SceneSerializer::LoadBezierPatch(Scene& scene, tinyxml2::XMLElement* bezierPatchElement)
    {
        std::string name = bezierPatchElement->Attribute("Name");
        bool isCylinder = bezierPatchElement->Attribute("WrapDirection") != std::string("None");
        bool isRowWrapDirection = bezierPatchElement->Attribute("WrapDirection") == std::string("Row");
        bool showPolygon = bezierPatchElement->BoolAttribute("ShowControlPolygon");
        int uDivisionCount = bezierPatchElement->IntAttribute("RowSlices") + 1;
        int vDivisionCount = bezierPatchElement->IntAttribute("ColumnSlices") + 1;

        std::vector<Ref<Point>> controlPoints;

        auto pointRefsElement = bezierPatchElement->FirstChildElement();
        int rowCount = 0;
        int columnCount = 0;
        for (auto elem = pointRefsElement->FirstChildElement(); elem != nullptr; elem = elem->NextSiblingElement())
        {
            rowCount = std::max(rowCount, elem->IntAttribute("Row"));
            columnCount = std::max(columnCount, elem->IntAttribute("Column"));
        }

        rowCount++;
        columnCount++;

        controlPoints.resize(rowCount * columnCount);

        for (auto elem = pointRefsElement->FirstChildElement(); elem != nullptr; elem = elem->NextSiblingElement())
        {
            int Row = elem->IntAttribute("Row");
            int Column = elem->IntAttribute("Column");
            auto point = FindPointByName(elem->Attribute("Name"));
            if (isRowWrapDirection)
                controlPoints[Column * rowCount + Row] = point;
            else
                controlPoints[Row * columnCount + Column] = point;
        }

        auto bezierPatch = BezierPatch::CreateBezierPatch(
            name,
            controlPoints,
            rowCount,
            columnCount,
            uDivisionCount,
            vDivisionCount,
            isCylinder,
            showPolygon);

        scene.m_BezierPatch.push_back(bezierPatch);
    }

    void SceneSerializer::LoadBSplinePatch(Scene& scene, tinyxml2::XMLElement* bSplinePatchElement)
    {
        std::string name = bSplinePatchElement->Attribute("Name");
        bool isCylinder = bSplinePatchElement->Attribute("WrapDirection") != std::string("None");
        bool isRowWrapDirection = bSplinePatchElement->Attribute("WrapDirection") == std::string("Row");
        bool showPolygon = bSplinePatchElement->BoolAttribute("ShowControlPolygon");
        int uDivisionCount = bSplinePatchElement->IntAttribute("RowSlices") + 1;
        int vDivisionCount = bSplinePatchElement->IntAttribute("ColumnSlices") + 1;

        std::vector<Ref<Point>> controlPoints;
        auto pointRefsElement = bSplinePatchElement->FirstChildElement();
        int rowCount = 0;
        int columnCount = 0;
        for (auto elem = pointRefsElement->FirstChildElement(); elem != nullptr; elem = elem->NextSiblingElement())
        {
            rowCount = std::max(rowCount, elem->IntAttribute("Row"));
            columnCount = std::max(columnCount, elem->IntAttribute("Column"));
        }

        rowCount++;
        columnCount++;
        controlPoints.resize(rowCount * columnCount);

        for (auto elem = pointRefsElement->FirstChildElement(); elem != nullptr; elem = elem->NextSiblingElement())
        {
            int Row = elem->IntAttribute("Row");
            int Column = elem->IntAttribute("Column");
            auto point = FindPointByName(elem->Attribute("Name"));
            if (isRowWrapDirection)
                controlPoints[Column * rowCount + Row] = point;
            else
                controlPoints[Row * columnCount + Column] = point;
        }

        auto bSplinePatch = BSplinePatch::CreateBSplinePatch(
            name,
            controlPoints,
            rowCount,
            columnCount,
            uDivisionCount,
            vDivisionCount,
            isCylinder,
            showPolygon);

        scene.m_BSplinePatch.push_back(bSplinePatch);
    }

    glm::vec3 SceneSerializer::ReadVector(tinyxml2::XMLElement* Pos)
    {
        glm::vec3 pos;
        pos.x = Pos->FloatAttribute("X");
        pos.y = Pos->FloatAttribute("Y");
        pos.z = Pos->FloatAttribute("Z");
        return pos;
    }

    Ref<Point> SceneSerializer::FindPointByName(std::string name)
    {
        auto it = m_Points.find(name);
        m_NotAssignedPoints.erase(name);
        return it->second;
    }
}
