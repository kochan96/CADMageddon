#include "IntersectionHelper.h"
#include "SurfaceUV.h"

namespace CADMageddon
{
    std::vector<IntersectionPoint> IntersectionHelper::GetIntersectionPoints(
        Ref<SurfaceUV> s1,
        Ref<SurfaceUV> s2,
        float stepSize,
        IntersectionType& intersectionType)
    {
        intersectionType = IntersectionType::ClosedClosed;

        glm::vec4 firstPoint = GetFirstPointFromTwoSurfaces(s1, s2, 5);
        if (firstPoint.x == -1 || glm::any(glm::isnan(firstPoint)))
            return std::vector<IntersectionPoint>();

        std::vector<IntersectionPoint> intersectionPoints;
        IntersectionPoint intersectionPoint = {
            firstPoint,
            s1->GetPointAt(firstPoint.x,firstPoint.y) };

        intersectionPoints.push_back(intersectionPoint);

        bool reversed = false;
        while (true)
        {
            intersectionPoint = GetNextIntersectionPoint(intersectionPoint.Coords, s1, s2, stepSize, reversed);
            if (glm::length(intersectionPoint.Location - intersectionPoints.front().Location) < stepSize && intersectionPoints.size() > 2)
            {   
                intersectionType = GetIntersectionType(intersectionType, intersectionPoint.Coords, s1, s2);
                intersectionPoints.push_back(GetNextIntersectionPoint(intersectionPoint.Coords, s1, s2, stepSize, reversed));
                break;
            }
            else if (CheckParameters(intersectionPoint.Coords, s1, s2))
            {
                intersectionPoints.push_back(intersectionPoint);
            }
            else if (!reversed)
            {
                intersectionType = GetIntersectionType(intersectionPoint.Coords, s1, s2);
                intersectionPoint.Coords = ClampParameters(intersectionPoint.Coords, s1, s2);
                intersectionPoint.Location = s1->GetPointAt(intersectionPoint.Coords.x, intersectionPoint.Coords.y);
                intersectionPoints.push_back(intersectionPoint);
                intersectionPoint = intersectionPoints[0];
                reversed = true;
                std::reverse(intersectionPoints.begin(), intersectionPoints.end());
            }
            else
            {
                intersectionType = GetIntersectionType(intersectionType, intersectionPoint.Coords, s1, s2);
                intersectionPoint.Coords = ClampParameters(intersectionPoint.Coords, s1, s2);
                intersectionPoint.Location = s1->GetPointAt(intersectionPoint.Coords.x, intersectionPoint.Coords.y);
                intersectionPoints.push_back(intersectionPoint);
                LOG_INFO("Length: {}", glm::length(intersectionPoints.back().Location - intersectionPoints[0].Location));
                break;
            }
        }

        return intersectionPoints;
    }

    glm::vec4 IntersectionHelper::GetFirstPointFromTwoSurfaces(
        Ref<SurfaceUV> s1,
        Ref<SurfaceUV> s2,
        float divide)
    {
        std::vector<std::pair<float, int>> bestPos;
        std::vector<glm::vec4> startingPos;

        float u1Delta = (s1->GetMaxU() - s1->GetMinU()) / divide;
        float u2Delta = (s2->GetMaxU() - s2->GetMinU()) / divide;
        float v1Delta = (s1->GetMaxV() - s1->GetMinV()) / divide;
        float v2Delta = (s2->GetMaxV() - s2->GetMinV()) / divide;

        int index = 0;
        for (int i = 0; i <= divide; i++)
        {
            float v1 = i * v1Delta;
            for (int j = 0; j <= divide; j++)
            {
                float u1 = j * u1Delta;
                for (int k = 0; k <= divide; k++)
                {
                    float v2 = k * v2Delta;
                    for (int l = 0; l <= divide; l++)
                    {
                        float u2 = l * u2Delta;
                        float dist = GetDistance(u1, v1, s1, u2, v2, s2);
                        startingPos.push_back({ u1,v1,u2,v2 });
                        bestPos.push_back(std::make_pair(dist, index++));
                    }
                }
            }
        }

        std::sort(bestPos.begin(), bestPos.end());
        for (int i = 0; i < bestPos.size(); i++)
        {
            auto parameters = startingPos[bestPos[i].second];
            glm::vec4 pos = GradientMinimalization(parameters.x, parameters.y, s1, parameters.z, parameters.w, s2);
            float dist = glm::length(s1->GetPointAt(pos.s, pos.t) - s2->GetPointAt(pos.p, pos.q));
            if (dist < 0.01f)
                return pos;
        }

        return glm::vec4(-1.0f);
    }

    IntersectionPoint IntersectionHelper::GetNextIntersectionPoint(
        glm::vec4 parameters,
        Ref<SurfaceUV> s1,
        Ref<SurfaceUV> s2,
        float stepSize,
        bool reversed)
    {
        static const float epsValue = 1e-4f;

        float r = 1.0f;
        if (reversed)
            r = -1.0f;

        auto initialPoint = s1->GetPointAt(parameters.x, parameters.y);
        glm::vec4 previousPos = parameters;
        glm::vec4 nextPos = parameters;
        const int MaxIterations = 20;

        auto s1Pos = s1->GetPointAt(nextPos.x, nextPos.y);
        auto s2Pos = s2->GetPointAt(nextPos.z, nextPos.w);

        for (int i = 0; i <= MaxIterations; i++)
        {
            auto s1TangentU = s1->GetTangentUAt(nextPos.x, nextPos.y);
            auto s1TangentV = s1->GetTangentVAt(nextPos.x, nextPos.y);
            auto s2TangentU = s2->GetTangentUAt(nextPos.z, nextPos.w);
            auto s2TangentV = s2->GetTangentVAt(nextPos.z, nextPos.w);

            auto s1Normal = glm::cross(s1TangentU, s1TangentV);
            auto s2Normal = glm::cross(s2TangentU, s2TangentV);
            auto t = glm::normalize(glm::cross(s1Normal, s2Normal));
            t *= r;

            glm::vec4 jacobianColumns[4];
            jacobianColumns[0] = glm::vec4(s1TangentU, glm::dot(s1TangentU, t));
            jacobianColumns[1] = glm::vec4(s1TangentV, glm::dot(s1TangentV, t));
            jacobianColumns[2] = glm::vec4(-s2TangentU, 0);
            jacobianColumns[3] = glm::vec4(-s2TangentV, 0);

            glm::mat4 jacobian(jacobianColumns[0], jacobianColumns[1], jacobianColumns[2], jacobianColumns[3]);

            auto f = glm::vec4(s1Pos - s2Pos, glm::dot(s1Pos - initialPoint, t) - stepSize);
            nextPos = nextPos - glm::inverse(jacobian) * f;
            s1Pos = s1->GetPointAt(nextPos.x, nextPos.y);
            s2Pos = s2->GetPointAt(nextPos.z, nextPos.w);

            if (glm::length(f) < epsValue || glm::length(s1Pos - initialPoint) <= stepSize)
            {
                return { nextPos,s1->GetPointAt(nextPos.x,nextPos.y) };
            }
        }

        return { nextPos,s1->GetPointAt(nextPos.x,nextPos.y) };
    }

    glm::vec4 IntersectionHelper::GradientMinimalization(
        float uStart1,
        float vStart1,
        Ref<SurfaceUV> s1,
        float uStart2,
        float vStart2,
        Ref<SurfaceUV> s2)
    {
        //Nonlinear_conjugate_gradient_method
        //https://en.wikipedia.org/wiki/Nonlinear_conjugate_gradient_method

        glm::vec4 minValues(s1->GetMinU(), s1->GetMinV(), s2->GetMinU(), s2->GetMinV());
        glm::vec4 maxValues(s1->GetMaxU(), s1->GetMaxV(), s2->GetMaxU(), s2->GetMaxV());

        const int MAX_ITERATIONS = 100;
        glm::vec4 parameters(uStart1, vStart1, uStart2, vStart2);
        const float alfaMin = 0.0f;
        const float alfaMax = 0.2f;

        auto direction = GetNegativeGradient(parameters.x, parameters.y, s1, parameters.z, parameters.w, s2);
        auto alfa = GoldenRatioSearch(alfaMin, alfaMax, parameters, direction, s1, s2);
        parameters = parameters + alfa * direction;

        glm::vec4 sDirection = direction;
        float beta;
        glm::vec4 lastDirection;
        glm::vec4 lastSDirection = direction;

        for (int i = 0; i < MAX_ITERATIONS; i++)
        {
            lastDirection = direction;
            direction = GetNegativeGradient(parameters.x, parameters.y, s1, parameters.z, parameters.w, s2);
            beta = glm::dot(direction, direction - lastDirection) / glm::dot(lastDirection, lastDirection);
            beta = std::max(0.0f, beta);
            sDirection = direction + beta * lastSDirection;
            alfa = GoldenRatioSearch(alfaMin, alfaMax, parameters, sDirection, s1, s2);
            parameters = parameters + alfa * sDirection;
            parameters = ClampParameters(parameters, s1, s2);
            if (alfa < 0.0000001f)
                return parameters;

            lastSDirection = sDirection;

            /*direction = GetNegativeGradient(parameters.x, parameters.y, s1, parameters.z, parameters.w, s2);
            alfa = GoldenRatioSearch(alfaMin, alfaMax, parameters, direction, s1, s2);
            parameters += alfa * direction;
            if (alfa < 0.0000001f)
                return parameters;*/
        }

        return parameters;
    }

    float IntersectionHelper::GetDistance(
        float uStart1,
        float vStart1,
        Ref<SurfaceUV> s1,
        float uStart2,
        float vStart2,
        Ref<SurfaceUV> s2)
    {
        auto pos1 = s1->GetPointAt(uStart1, vStart1);
        auto pos2 = s2->GetPointAt(uStart2, vStart2);

        return glm::dot(pos1 - pos2, pos1 - pos2);
    }

    glm::vec4 IntersectionHelper::GetNegativeGradient(
        float uStart1,
        float vStart1,
        Ref<SurfaceUV> s1,
        float uStart2,
        float vStart2,
        Ref<SurfaceUV> s2)
    {
        glm::vec4 gradient;
        auto pos1 = s1->GetPointAt(uStart1, vStart1);
        auto pos2 = s2->GetPointAt(uStart2, vStart2);
        auto diff = pos1 - pos2;

        gradient.x = 2.0f * glm::dot(s1->GetTangentUAt(uStart1, vStart1), diff);
        gradient.y = 2.0f * glm::dot(s1->GetTangentVAt(uStart1, vStart1), diff);
        gradient.z = -2.0f * glm::dot(diff, s2->GetTangentUAt(uStart2, vStart2));
        gradient.w = -2.0f * glm::dot(diff, s2->GetTangentVAt(uStart2, vStart2));

        return -glm::normalize(gradient);
    }

    float IntersectionHelper::GoldenRatioSearch(
        float alfaMin,
        float alfaMax,
        glm::vec4 x,
        glm::vec4 direction,
        Ref<SurfaceUV> s1,
        Ref<SurfaceUV> s2,
        float tolerance)
    {
        //https://en.wikipedia.org/wiki/Golden-section_search

        static const float invPhi = 0.5f * (sqrtf(5) - 1);
        static const float invPhi2 = 0.5f * (3 - sqrtf(5));
        float h = alfaMax - alfaMin;
        if (h <= tolerance)
            return(alfaMax + alfaMin) / 2.0f;

        int n = int(std::ceil(std::log(tolerance / h) / std::log(invPhi))); //required steps to achive tolerance 

        float c = alfaMin + invPhi2 * h;
        float d = alfaMin + invPhi * h;

        auto xLeft = x + c * direction;
        auto xRight = x + d * direction;
        auto valueLeft = GetDistance(xLeft.x, xLeft.y, s1, xLeft.z, xLeft.w, s2);
        auto valueRight = GetDistance(xRight.x, xRight.y, s1, xRight.z, xRight.w, s2);

        for (int k = 0; k < n; k++)
        {
            if (valueLeft < valueRight)
            {
                alfaMax = d;
                d = c;
                xRight = xLeft;
                valueRight = valueLeft;
                h = invPhi * h;
                c = alfaMin + invPhi2 * h;
                xLeft = x + c * direction;
                valueLeft = GetDistance(xLeft.x, xLeft.y, s1, xLeft.z, xLeft.w, s2);
            }
            else
            {
                alfaMin = c;
                c = d;
                xLeft = xRight;
                valueLeft = valueRight;
                h = invPhi * h;
                d = alfaMin * invPhi * h;
                xRight = alfaMin + d * direction;
                valueRight = GetDistance(xRight.x, xRight.y, s1, xRight.z, xRight.w, s2);
            }
        }

        if (valueLeft < valueRight)
        {
            return (alfaMin + d) / 2.0f;
        }
        else
        {
            return (c + alfaMax) / 2.0f;
        }
    }

    glm::vec4 IntersectionHelper::ClampParameters(glm::vec4 parameters, Ref<SurfaceUV> s1, Ref<SurfaceUV> s2)
    {
        if (!s1->GetRollU())
        {
            parameters.x = glm::clamp(parameters.x, s1->GetMinU(), s1->GetMaxU());
        }

        if (!s1->GetRollV())
        {
            parameters.y = glm::clamp(parameters.y, s1->GetMinV(), s1->GetMaxV());
        }

        if (!s2->GetRollU())
        {
            parameters.z = glm::clamp(parameters.z, s2->GetMinU(), s2->GetMaxU());
        }

        if (!s2->GetRollV())
        {
            parameters.w = glm::clamp(parameters.w, s2->GetMinV(), s2->GetMaxV());
        }

        return parameters;
    }

    bool IntersectionHelper::CheckParameters(glm::vec4& parameters, Ref<SurfaceUV> s1, Ref<SurfaceUV> s2)
    {
        bool isU1OutOfRange = !s1->GetRollU() && (parameters.x < s1->GetMinU() || parameters.x > s1->GetMaxU());
        bool isV1OutOfRange = !s1->GetRollV() && (parameters.y < s1->GetMinV() || parameters.y > s1->GetMaxV());

        bool isU2OutOfRange = !s2->GetRollU() && (parameters.z < s2->GetMinU() || parameters.z > s2->GetMaxU());
        bool isV2OutOfRange = !s2->GetRollV() && (parameters.w < s2->GetMinV() || parameters.w > s2->GetMaxV());

        return !isU1OutOfRange && !isV1OutOfRange && !isU2OutOfRange && !isV2OutOfRange;
    }

    IntersectionType IntersectionHelper::GetIntersectionType(glm::vec4& parameters, Ref<SurfaceUV> s1, Ref<SurfaceUV> s2)
    {
        bool isU1OutOfRange = !s1->GetRollU() && (parameters.x < s1->GetMinU() || parameters.x > s1->GetMaxU());
        bool isV1OutOfRange = !s1->GetRollV() && (parameters.y < s1->GetMinV() || parameters.y > s1->GetMaxV());

        bool isU2OutOfRange = !s2->GetRollU() && (parameters.z < s2->GetMinU() || parameters.z > s2->GetMaxU());
        bool isV2OutOfRange = !s2->GetRollV() && (parameters.w < s2->GetMinV() || parameters.w > s2->GetMaxV());

        if (isU1OutOfRange || isV1OutOfRange)
            return IntersectionType::ClosedOpen;
        else if (isU2OutOfRange || isV2OutOfRange)
            return IntersectionType::OpenClosed;

        return IntersectionType::OpenOpen; // should never happend;
    }
    IntersectionType IntersectionHelper::GetIntersectionType(
        IntersectionType intersectionType,
        glm::vec4& parameters,
        Ref<SurfaceUV> s1,
        Ref<SurfaceUV> s2)
    {
        bool isU1OutOfRange = !s1->GetRollU() && (parameters.x < s1->GetMinU() || parameters.x > s1->GetMaxU());
        bool isV1OutOfRange = !s1->GetRollV() && (parameters.y < s1->GetMinV() || parameters.y > s1->GetMaxV());

        bool isU2OutOfRange = !s2->GetRollU() && (parameters.z < s2->GetMinU() || parameters.z > s2->GetMaxU());
        bool isV2OutOfRange = !s2->GetRollV() && (parameters.w < s2->GetMinV() || parameters.w > s2->GetMaxV());

        if ((isU1OutOfRange || isV1OutOfRange))
        {
            if (intersectionType == IntersectionType::ClosedOpen)
            {
                return IntersectionType::ClosedOpen;
            }
            else
            {
                return IntersectionType::OpenOpen;
            }
        }

        if (isU2OutOfRange || isV2OutOfRange)
        {
            if (intersectionType == IntersectionType::OpenClosed)
            {
                return IntersectionType::OpenClosed;
            }
            else
            {
                return IntersectionType::OpenOpen;
            }
        }

        return IntersectionType::ClosedClosed;
    }

    IntersectionPoint IntersectionHelper::GetLastIntersectionPoint(IntersectionType intersectionType, IntersectionPoint parameters, Ref<SurfaceUV> s1, Ref<SurfaceUV> s2)
    {
        IntersectionPoint p;

        return p;
    }
}
