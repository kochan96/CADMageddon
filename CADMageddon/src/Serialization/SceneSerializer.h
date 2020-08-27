#pragma once
#include <string>
#include "Scene\Scene.h"
#include "tinyxml2.h"
#include <glm\glm.hpp>
#include "cadpch.h"

namespace CADMageddon
{
    class SceneSerializer
    {

    public:
        static bool SerializeScene(const Scene& scene, const std::string& filePath);
        static Ref<Scene> LoadScene(const std::string& filePath);


    private:
        static void LoadPoints(Scene& scene, tinyxml2::XMLElement* sceneElement);
        static void LoadTorus(Scene& scene, tinyxml2::XMLElement* torusElement);
        static void LoadBezierC0(Scene& scene, tinyxml2::XMLElement* bezierC0Element);
        static void LoadBSpline(Scene& scene, tinyxml2::XMLElement* bSplineElement);
        static void LoadInterpolatedCurve(Scene& scene, tinyxml2::XMLElement* interpolatedCurveElement);
        static void LoadBezierPatch(Scene& scene, tinyxml2::XMLElement* bezierPatchElement);
        static void LoadBSplinePatch(Scene& scene, tinyxml2::XMLElement* bSplinePatchElement);


        static glm::vec3 ReadVector(tinyxml2::XMLElement* Pos);

        static std::unordered_map<std::string, Ref<Point>> m_NotAssignedPoints;
        static std::unordered_map<std::string, Ref<Point>> m_Points;

        static Ref<Point> FindPointByName(std::string name);
    };


}