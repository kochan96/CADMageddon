#pragma once
#include "cadpch.h"
#include "Core/Base.h"
#include <glm/glm.hpp>

namespace CADMageddon
{
    enum class GizmoMode
    {
        None,
        Translation,
        Rotation,
        Scale
    };

    struct CameraRay
    {
        glm::vec3 Origin;
        glm::vec3 Direction;
        float t;
    };

    struct Circle
    {
        float Radius;
        glm::vec3 Center;
        glm::vec3 orientation;
    };

    struct Plane
    {
        glm::vec3 Point;
        glm::vec3 Normal;
    };

    struct GizmoContext
    {
        bool IsActive;
        GizmoMode Mode = GizmoMode::None;
        CameraRay CameraRay;
        glm::vec3 PreviousIntersection;
        glm::vec3 CurrentIntersection;

        glm::vec3 Center;

        glm::vec3 Axes[3] = {
            glm::vec3(1.0f,0.0f,0.0f),
            glm::vec3(0.0f,1.0f,0.0f),
            glm::vec3(0.0f,0.0f,-1.0f) };

        int AxesCount = 3;
        int SelectedAxis = -1;
    };

    struct Transform;

    class Gizmo
    {
    public:
        static bool IsActive() { return context->IsActive; }
        static void Manipulate(GizmoMode mode, Transform& transform, const glm::mat4& cameraMatrix, glm::vec2 ndcMousePosition);

    private:
        static constexpr float SELECTION_THRESHOLD = 0.04f;

        static void ManipulateTranslation(Transform& transform, const glm::mat4& cameraMatrix, glm::vec2 ndcMousePosition);
        static void ManipulateRotation(Transform& transform, const glm::mat4& cameraMatrix, glm::vec2 ndcMousePosition);
        static void ManipulateScale(Transform& transform, const glm::mat4& cameraMatrix, glm::vec2 ndcMousePosition);

        static void RenderTranslation(Transform& transform);
        static void RenderRotation(Transform& transform);
        static void RenderScale(Transform& transform);


        static void RenderCircle(glm::vec3 position, float radius, glm::vec3 normal, glm::vec4 color);

        static bool CheckIfActiveTranslation(Transform& transform, const glm::mat4& cameraMatrix, glm::vec2 ndcMousePosition);
        static bool CheckIfActiveRotation(Transform& transform, const glm::mat4& cameraMatrix, glm::vec2 ndcMousePosition);
        static bool CheckIfActiveScale(Transform& transform, const glm::mat4& cameraMatrix, glm::vec2 ndcMousePosition);


        static float ClosestDistanceBetweenLines(CameraRay& ray1, CameraRay& ray2);
        static float ClosestDistanceLineCircle(const CameraRay& ray, const Circle& circle, glm::vec3& point);
        static bool IntersectLinePlane(CameraRay& ray, Plane& plane);
        static float ClosestDistanceLinePoint(const CameraRay& ray, glm::vec3 point);

        static glm::vec3 findV(glm::vec3 normal);

        static Scope<GizmoContext> context;
        static CameraRay GetCameraRay(const glm::mat4& cameraMatrix, glm::vec2 ndcMousePosition);
    };
}