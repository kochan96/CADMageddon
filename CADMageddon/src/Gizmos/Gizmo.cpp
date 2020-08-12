#include "Gizmo.h"
#include "Core/Input.h"
#include "Rendering\Renderer.h"

namespace CADMageddon
{
    Scope<GizmoContext> Gizmo::context = CreateScope<GizmoContext>();

    void Gizmo::Manipulate(GizmoMode mode, TransformComponent& TransformComponent, const glm::mat4& cameraMatrix, glm::vec2 ndcMousePosition)
    {
        if (context->Mode != mode)
        {
            context->SelectedAxis = -1;
            context->IsActive = false;
            context->Mode = mode;
        }

        switch (mode)
        {
        case GizmoMode::Translation:
            RenderTranslation(TransformComponent);
            break;
        case GizmoMode::Rotation:
            RenderRotation(TransformComponent);
            break;
        case GizmoMode::Scale:
            RenderScale(TransformComponent);
            break;
        }

        if (!Input::IsMouseButtonPressed(CDM_MOUSE_BUTTON_LEFT))
        {
            context->SelectedAxis = -1;
            context->IsActive = false;
            return;
        }

        switch (mode)
        {
        case GizmoMode::Translation:
            ManipulateTranslation(TransformComponent, cameraMatrix, ndcMousePosition);
            break;
        case GizmoMode::Rotation:
            ManipulateRotation(TransformComponent, cameraMatrix, ndcMousePosition);
            break;
        case GizmoMode::Scale:
            ManipulateScale(TransformComponent, cameraMatrix, ndcMousePosition);
            break;
        }

    }

    /*
    TRANSLATION
    */
    bool Gizmo::CheckIfActiveTranslation(TransformComponent& TransformComponent, const glm::mat4& cameraMatrix, glm::vec2 ndcMousePosition)
    {
        CameraRay cameraRay = GetCameraRay(cameraMatrix, ndcMousePosition);
        glm::vec3 intersection;
        float minDistance = FLT_MAX;

        for (int i = 0; i < context->AxesCount; i++)
        {
            CameraRay axis;
            axis.Origin = TransformComponent.Translation;
            axis.Direction = context->Axes[i];
            float distance = ClosestDistanceBetweenLines(cameraRay, axis);
            if (distance < minDistance)
            {
                minDistance = distance;
                context->SelectedAxis = i;
                context->PreviousIntersection = cameraRay.Origin + cameraRay.Direction * cameraRay.t;
            }
        }

        if (minDistance > SELECTION_THRESHOLD)
        {
            context->SelectedAxis = -1;
            return false;
        }

        return true;

    }

    void Gizmo::ManipulateTranslation(TransformComponent& TransformComponent, const glm::mat4& cameraMatrix, glm::vec2 ndcMousePosition)
    {
        if (context->IsActive == false)
        {
            context->IsActive = CheckIfActiveTranslation(TransformComponent, cameraMatrix, ndcMousePosition);
            return;
        }

        CameraRay cameraRay = GetCameraRay(cameraMatrix, ndcMousePosition);
        CameraRay axis;
        axis.Origin = TransformComponent.Translation;
        axis.Direction = context->Axes[context->SelectedAxis];
        float minDistance = ClosestDistanceBetweenLines(cameraRay, axis);

        context->CurrentIntersection = cameraRay.Origin + cameraRay.Direction * cameraRay.t;
        auto delta = context->CurrentIntersection - context->PreviousIntersection;

        TransformComponent.Translation += glm::dot(delta, context->Axes[context->SelectedAxis]) * context->Axes[context->SelectedAxis];

        context->PreviousIntersection = context->CurrentIntersection;
        context->Center = TransformComponent.Translation;
    }

    void Gizmo::RenderTranslation(TransformComponent& TransformComponent)
    {
        for (int i = 0; i < 3; i++)
        {
            glm::vec3 axis_end = glm::vec3(0.f, 0.f, 0.f);
            axis_end[i] = 1.f;

            glm::vec4 axis_color = glm::vec4(0.f, 0.f, 0.f, 1.0f);
            axis_color[i] = 1.f;

            if (i == context->SelectedAxis)
            {
                axis_color = glm::vec4(1.f, 0.65f, 0.f, 1.0f);
            }

            Renderer::RenderLine(TransformComponent.Translation, TransformComponent.Translation + axis_end, axis_color);
        }
    }

    /*
        ROTATION
    */

    bool Gizmo::CheckIfActiveRotation(TransformComponent& TransformComponent, const glm::mat4& cameraMatrix, glm::vec2 ndcMousePosition)
    {
        CameraRay cameraRay = GetCameraRay(cameraMatrix, ndcMousePosition);
        glm::vec3 intersection;
        float minDistance = FLT_MAX;

        for (int i = 0; i < context->AxesCount; i++)
        {
            Circle axis;
            axis.Radius = 1.0f;
            axis.Center = TransformComponent.Translation;
            axis.orientation = TransformComponent.GetMatrix() * glm::vec4(context->Axes[i], 0.0f);
            axis.orientation = glm::normalize(axis.orientation);
            float distance = ClosestDistanceLineCircle(cameraRay, axis, intersection);
            if (distance < minDistance)
            {
                minDistance = distance;
                context->SelectedAxis = i;
                context->PreviousIntersection = intersection;
            }
        }

        if (minDistance > SELECTION_THRESHOLD)
        {
            context->SelectedAxis = -1;
            return false;
        }

        return true;
    }

    void Gizmo::ManipulateRotation(TransformComponent& TransformComponent, const glm::mat4& cameraMatrix, glm::vec2 ndcMousePosition)
    {
        if (context->IsActive == false)
        {
            context->IsActive = CheckIfActiveRotation(TransformComponent, cameraMatrix, ndcMousePosition);
            return;
        }

        CameraRay cameraRay = GetCameraRay(cameraMatrix, ndcMousePosition);
        glm::vec3 intersection;
        float minDistance = FLT_MAX;

        Circle axis;
        axis.Radius = 1.0f;
        axis.Center = TransformComponent.Translation;
        axis.orientation = TransformComponent.GetMatrix() * glm::vec4(context->Axes[context->SelectedAxis], 0.0f);
        axis.orientation = glm::normalize(axis.orientation);

        float distance = ClosestDistanceLineCircle(cameraRay, axis, intersection);
        context->CurrentIntersection = intersection;

        auto previous = glm::normalize(context->PreviousIntersection - axis.Center);
        auto current = glm::normalize(context->CurrentIntersection - axis.Center);
        float cosAngle = glm::dot(previous, current);
        cosAngle = std::min(cosAngle, 1.0f);
        auto angle = glm::acos(cosAngle);

        auto sign = glm::dot(glm::cross(previous, current), axis.orientation);
        if (sign < 0)
        {
            angle = -angle;
        }

        glm::vec3 rotationDelta = glm::vec3(0.0f);
        rotationDelta[context->SelectedAxis] = glm::degrees(angle);

        TransformComponent.Rotation += rotationDelta;
        TransformComponent.Rotation = glm::clamp(TransformComponent.Rotation, -180.0f, 180.0f);

        context->PreviousIntersection = context->CurrentIntersection;
        context->Center = TransformComponent.Translation;

    }

    void Gizmo::RenderRotation(TransformComponent& TransformComponent)
    {
        for (int i = 0; i < 3; i++)
        {
            glm::vec3 axis_end = glm::vec3(0.f, 0.f, 0.f);
            axis_end[i] = 1.f;

            axis_end = TransformComponent.GetMatrix() * glm::vec4(axis_end, 0.0f);
            axis_end = glm::normalize(axis_end);

            glm::vec4 axis_color = glm::vec4(0.f, 0.f, 0.f, 1.0f);
            axis_color[i] = 1.f;

            if (i == context->SelectedAxis)
            {
                axis_color = glm::vec4(1.f, 0.65f, 0.f, 1.0f);
            }

            RenderCircle(TransformComponent.Translation, 1.0f, axis_end, axis_color);
        }
    }

    void Gizmo::RenderCircle(glm::vec3 position, float Radius, glm::vec3 normal, glm::vec4 color)
    {
        glm::vec3 xAxis = findV(normal);

        glm::vec3 yAxis = glm::cross(normal, xAxis);

        int count = 20;
        float Delta = glm::two_pi<float>() / count;

        for (int i = 0; i < count; i++)
        {
            float angle = i * Delta;
            glm::vec3 start = position + Radius * (xAxis * cos(angle) + yAxis * sin(angle));
            angle = (i + 1) * Delta;
            glm::vec3 end = position + Radius * (xAxis * cos(angle) + yAxis * sin(angle));;
            Renderer::RenderLine(start, end, color);
        }
    }

    glm::vec3 Gizmo::findV(glm::vec3 normal) {
        glm::vec3 vp = glm::vec3(0.0f, 0.0f, 0.0f);
        if (normal.x != 0 || normal.y != 0) {
            vp = glm::vec3(0.0f, 0.0f, 1.0f);
        }
        else if (normal.x != 0 || normal.z != 0) {
            vp = glm::vec3(0.0f, 1.0f, 0.0f);
        }
        else if (normal.y != 0 || normal.z != 0) {
            vp = glm::vec3(1.0f, 0.0f, 0.0f);
        }


        glm::vec3 cp = glm::cross(normal, vp);
        return glm::normalize(cp);
    }

    /*
    SCALE
    */

    bool Gizmo::CheckIfActiveScale(TransformComponent& TransformComponent, const glm::mat4& cameraMatrix, glm::vec2 ndcMousePosition)
    {
        CameraRay cameraRay = GetCameraRay(cameraMatrix, ndcMousePosition);
        glm::vec3 intersection;
        float minDistance = FLT_MAX;

        for (int i = 0; i < context->AxesCount; i++)
        {
            CameraRay axis;
            axis.Origin = TransformComponent.Translation;
            axis.Direction = context->Axes[i];
            float distance = ClosestDistanceBetweenLines(cameraRay, axis);
            if (distance < minDistance)
            {
                minDistance = distance;
                context->SelectedAxis = i;
                context->PreviousIntersection = cameraRay.Origin + cameraRay.Direction * cameraRay.t;
            }
        }

        if (minDistance > SELECTION_THRESHOLD)
        {
            context->SelectedAxis = -1;
            return false;
        }

        return true;
    }

    void Gizmo::ManipulateScale(TransformComponent& TransformComponent, const glm::mat4& cameraMatrix, glm::vec2 ndcMousePosition)
    {
        if (context->IsActive == false)
        {
            context->IsActive = CheckIfActiveTranslation(TransformComponent, cameraMatrix, ndcMousePosition);
            return;
        }

        CameraRay cameraRay = GetCameraRay(cameraMatrix, ndcMousePosition);
        CameraRay axis;
        axis.Origin = TransformComponent.Translation;
        axis.Direction = context->Axes[context->SelectedAxis];
        float minDistance = ClosestDistanceBetweenLines(cameraRay, axis);

        context->CurrentIntersection = cameraRay.Origin + cameraRay.Direction * cameraRay.t;
        auto delta = context->CurrentIntersection - context->PreviousIntersection;

        TransformComponent.Scale += glm::dot(delta, context->Axes[context->SelectedAxis]) * context->Axes[context->SelectedAxis];

        context->PreviousIntersection = context->CurrentIntersection;
        context->Center = TransformComponent.Translation;
    }

    void Gizmo::RenderScale(TransformComponent& TransformComponent)
    {
        for (int i = 0; i < 3; i++)
        {
            glm::vec3 axis_end = glm::vec3(0.f, 0.f, 0.f);
            axis_end[i] = 1.f;

            glm::vec4 axis_color = glm::vec4(0.f, 0.f, 0.f, 1.0f);
            axis_color[i] = 1.f;

            if (i == context->SelectedAxis)
            {
                axis_color = glm::vec4(1.f, 0.65f, 0.f, 1.0f);
            }

            Renderer::RenderLine(TransformComponent.Translation, TransformComponent.Translation + axis_end, axis_color);
        }
    }

    float Gizmo::ClosestDistanceBetweenLines(CameraRay& ray1, CameraRay& ray2)
    {
        const glm::vec3 dp = ray2.Origin - ray1.Origin;
        const float v12 = glm::dot(ray1.Direction, ray1.Direction);
        const float v22 = glm::dot(ray2.Direction, ray2.Direction);
        const float v1v2 = glm::dot(ray1.Direction, ray2.Direction);

        const float det = v1v2 * v1v2 - v12 * v22;

        if (std::abs(det) > FLT_MIN)
        {
            const float inv_det = 1.f / -det;

            const float dpv1 = glm::dot(dp, ray1.Direction);
            const float dpv2 = glm::dot(-dp, ray2.Direction);

            ray1.t = inv_det * (v22 * dpv1 + v1v2 * dpv2);
            ray2.t = inv_det * (v1v2 * dpv1 + v12 * dpv2);

            return glm::length(dp + ray2.Direction * ray2.t - ray1.Direction * ray1.t);
        }
        else
        {
            const glm::vec3 a = cross(dp, ray1.Direction);
            return std::sqrt(dot(a, a) / v12);
        }
    }

    float Gizmo::ClosestDistanceLineCircle(const CameraRay& line, const Circle& c, glm::vec3& point)
    {
        Plane plane;
        plane.Point = c.Center;
        plane.Normal = c.orientation;
        CameraRay r = line;

        if (IntersectLinePlane(r, plane))
        {
            // get the ray's intersection point on the plane which
            // contains the circle
            const glm::vec3 pointOnPlane = r.Origin + r.t * r.Direction;
            // project that point on to the circle's circumference
            point = c.Center + c.Radius * glm::normalize(pointOnPlane - c.Center);
            return glm::length(pointOnPlane - point);
        }
        else
        {
            // the required point on the circle is the one closest to the camera origin
            auto diff = line.Origin - c.Center;
            auto v1 = glm::dot(diff, c.orientation) * c.orientation;

            point = c.Radius * glm::normalize(diff - v1);

            return ClosestDistanceLinePoint(line, point);
        }

        return FLT_MAX;
    }

    bool Gizmo::IntersectLinePlane(CameraRay& ray, Plane& plane)
    {
        auto ln = glm::dot(ray.Direction, plane.Normal);
        if (std::abs(ln) < 0.0000001f)
        {
            return false;
        }

        glm::vec3 diff = plane.Point - ray.Origin;
        ray.t = glm::dot(diff, plane.Normal) / ln;

        return true;

    }

    float Gizmo::ClosestDistanceLinePoint(const CameraRay& ray, glm::vec3 point)
    {
        auto vector = (ray.Origin - point) - glm::dot((ray.Origin - point), ray.Direction) * ray.Direction;

        return glm::length(vector);
    }

    CameraRay Gizmo::GetCameraRay(const glm::mat4& cameraMatrix, glm::vec2 ndcMousePosition)
    {
        glm::vec4 ray_start, ray_end;
        glm::mat4 view_proj_inverse = glm::inverse(cameraMatrix);

        ray_start = view_proj_inverse * glm::vec4(ndcMousePosition.x, ndcMousePosition.y, -1.0f, 1.f);
        ray_start *= 1.f / ray_start.w;

        ray_end = view_proj_inverse * glm::vec4(ndcMousePosition.x, ndcMousePosition.y, 1.f, 1.f);
        ray_end *= 1.f / ray_end.w;

        CameraRay ray;
        ray.Origin = ray_start;
        ray.Direction = glm::normalize(ray_end - ray_start);

        ray.t = FLT_MAX;

        return ray;
    }

}