#include "IntersectionHelper.h"
#include "SurfaceUV.h"
#include "LineClipper.h"

namespace CADMageddon
{
    std::vector<std::vector<glm::vec2>>* IntersectionHelper::GetIntersectionPoints(
        Ref<SurfaceUV> s1,
        Ref<SurfaceUV> s2,
        float stepSize,
        IntersectionType& intersectionType,
        std::vector<IntersectionPoint>& intersectionPoints)
    {
        intersectionType = IntersectionType::ClosedClosed;
        glm::vec4 firstPoint;
        if (s1 == s2)
        {
            firstPoint = GetFirstPointFromOneSurface(s1);
        }
        else
        {
            firstPoint = GetFirstPointFromTwoSurfaces(s1, s2);
        }

        if (firstPoint.x == -1 || glm::any(glm::isnan(firstPoint)))
            return nullptr;

        return GetIntersectionPoints(firstPoint, s1, s2, stepSize, intersectionType, intersectionPoints);
    }

    std::vector<std::vector<glm::vec2>>* IntersectionHelper::GetIntersectionPoints(
        Ref<Cursor3D> cursor,
        Ref<SurfaceUV> s1,
        Ref<SurfaceUV> s2,
        float stepSize,
        IntersectionType& intersectionType,
        std::vector<IntersectionPoint>& intersectionPoints)
    {
        glm::vec4 firstPoint;
        if (s1 == s2)
        {
            firstPoint = GetFirstPointFromOneSurfaceCursor(s1, cursor);
        }
        else
        {
            firstPoint = GetFirstPointFromTwoSurfacesCursor(cursor, s1, s2);
        }

        intersectionType = IntersectionType::ClosedClosed;

        if (firstPoint.x == -1 || glm::any(glm::isnan(firstPoint)))
            return nullptr;

        return GetIntersectionPoints(firstPoint, s1, s2, stepSize, intersectionType, intersectionPoints);
    }

    std::vector<std::vector<glm::vec2>>* IntersectionHelper::GetIntersectionPoints(glm::vec4 firstPoint, Ref<SurfaceUV> s1, Ref<SurfaceUV> s2, float stepSize, IntersectionType& intersectionType, std::vector<IntersectionPoint>& intersectionPoints)
    {
        firstPoint = ClampParameters(firstPoint, s1, s2);

        IntersectionPoint intersectionPoint = {
           firstPoint,
           s1->GetPointAt(firstPoint.x,firstPoint.y) };

        intersectionPoints.push_back(intersectionPoint);

        std::vector<std::vector<glm::vec2>>* loops = new std::vector<std::vector<glm::vec2>>[2];
        loops[0].push_back(std::vector<glm::vec2>());
        loops[1].push_back(std::vector<glm::vec2>());
        loops[0][0].push_back({ firstPoint.x,firstPoint.y });
        loops[1][0].push_back({ firstPoint.z,firstPoint.w });

        bool reversed = false;
        while (true)
        {
            intersectionPoint = GetNextIntersectionPoint(intersectionPoint.Coords, s1, s2, stepSize, reversed);
            glm::vec2 firstParameters = glm::vec2(intersectionPoint.Coords.x, intersectionPoint.Coords.y);
            glm::vec2 secondParameters = glm::vec2(intersectionPoint.Coords.z, intersectionPoint.Coords.w);
            if (glm::length(intersectionPoint.Location - intersectionPoints.front().Location) < stepSize && intersectionPoints.size() > 2)
            {
                intersectionType = GetIntersectionType(intersectionType, intersectionPoint.Coords, s1, s2);
                GetLastTwoPoints(s1, s2, intersectionPoint, intersectionPoints, loops[0], loops[1]);
                MergeLastAndFirstLoop(loops[0]);
                MergeLastAndFirstLoop(loops[1]);
                break;
            }
            else if (CheckParameters(firstParameters, s1, loops[0]) && CheckParameters(secondParameters, s2, loops[1]))
            {
                glm::vec4 paramters(firstParameters.x, firstParameters.y, secondParameters.x, secondParameters.y);
                intersectionPoint.Coords = paramters;
                intersectionPoints.push_back(intersectionPoint);
            }
            else if (!reversed)
            {
                intersectionType = GetIntersectionType(intersectionType, intersectionPoint.Coords, s1, s2);
                intersectionPoint.Coords = ClampParameters(intersectionPoint.Coords, s1, s2);
                intersectionPoint.Location = s1->GetPointAt(intersectionPoint.Coords.x, intersectionPoint.Coords.y);
                intersectionPoints.push_back(intersectionPoint);
                loops[0].back().push_back({ intersectionPoint.Coords.x,intersectionPoint.Coords.y });
                loops[1].back().push_back({ intersectionPoint.Coords.z,intersectionPoint.Coords.w });

                intersectionPoint = intersectionPoints[0];
                reversed = true;
                std::reverse(intersectionPoints.begin(), intersectionPoints.end());
                std::reverse(loops[0].back().begin(), loops[0].back().end());
                std::reverse(loops[1].back().begin(), loops[1].back().end());
            }
            else
            {
                intersectionType = GetIntersectionType(intersectionType, intersectionPoint.Coords, s1, s2);
                intersectionPoint.Coords = ClampParameters(intersectionPoint.Coords, s1, s2);
                intersectionPoint.Location = s1->GetPointAt(intersectionPoint.Coords.x, intersectionPoint.Coords.y);
                intersectionPoints.push_back(intersectionPoint);
                loops[0].back().push_back({ intersectionPoint.Coords.x,intersectionPoint.Coords.y });
                loops[1].back().push_back({ intersectionPoint.Coords.z,intersectionPoint.Coords.w });
                LOG_INFO("Length: {}", glm::length(intersectionPoints.back().Location - intersectionPoints[0].Location));
                break;
            }
        }

        return loops;
    }

    void IntersectionHelper::MergeLastAndFirstLoop(std::vector<std::vector<glm::vec2>>& loop)
    {
        if (loop.size() == 1)
        {
            loop.back().push_back(loop.front().front());
            return;
        }

        for (auto point : loop.front())
        {
            loop.back().push_back(point);
        }

        loop.front().clear();
    }

    void IntersectionHelper::GetLastTwoPoints(
        Ref<SurfaceUV> s1,
        Ref<SurfaceUV> s2,
        IntersectionPoint& last,
        std::vector<IntersectionPoint>& intersectionPoints,
        std::vector<std::vector<glm::vec2>>& firstLoops,
        std::vector<std::vector<glm::vec2>>& secondLoops)
    {
        glm::vec2 firstParameters(last.Coords.x, last.Coords.y);
        glm::vec2 secondParameters(last.Coords.z, last.Coords.w);
        CheckParameters(firstParameters, s1, firstLoops) && CheckParameters(secondParameters, s2, secondLoops);
        glm::vec4 paramters(firstParameters.x, firstParameters.y, secondParameters.x, secondParameters.y);
        last.Coords = paramters;
        intersectionPoints.push_back(last);
        intersectionPoints.push_back(intersectionPoints.front());
    }

    glm::vec4 IntersectionHelper::GetFirstPointFromOneSurfaceCursor(Ref<SurfaceUV> s1, Ref<Cursor3D> cursor, float divide)
    {
        std::vector<std::pair<float, int>> bestPos;
        std::vector<glm::vec4> startPos;
        int index = 0;

        for (int i = 0; i <= divide; i++)
            for (int j = 0; j <= divide; j++)
                for (int k = 0; k <= divide; k++)
                    for (int l = 0; l <= divide; l++)
                        if (i != k || j != l) {
                            glm::vec4 newPos(i / divide, j / divide, k / divide, l / divide);
                            float dist = GetDistance(newPos, s1, cursor);
                            startPos.emplace_back(newPos);
                            bestPos.emplace_back(std::make_pair(dist, index++));
                        }
        std::sort(bestPos.begin(), bestPos.end());
        for (int i = 0; i < bestPos.size(); i++) {
            glm::vec4 pos = GradientMinimalization(startPos[bestPos[i].second], s1, s1);
            glm::vec4 pos2 = pos;
            if (s1->GetRollU()) {
                pos2.s = pos2.s - std::floorf(pos2.s);
                pos2.p = pos2.p - std::floorf(pos2.p);
            }
            else {
                pos2.s = glm::clamp(pos2.s, 0.f, 1.f);
                pos2.p = glm::clamp(pos2.p, 0.f, 1.f);
            }
            if (s1->GetRollV()) {
                pos2.t = pos2.t - std::floorf(pos2.t);
                pos2.q = pos2.q - std::floorf(pos2.q);
            }
            else {
                pos2.t = glm::clamp(pos2.t, 0.f, 1.f);
                pos2.q = glm::clamp(pos2.q, 0.f, 1.f);
            }

            float dist = glm::length(s1->GetPointAt(pos.s, pos.t) -
                s1->GetPointAt(pos.p, pos.q));
            //if (dist < 0.00001f)
            //	continue;		
            float coordDist = glm::length(glm::vec2(pos2.s - pos2.p, pos2.t - pos2.q));
            if (std::max(std::abs(pos2.s - pos2.p), std::abs(pos2.t - pos2.q)) > 0.999f)
                continue;
            if (dist < 0.01f && coordDist > 0.01f)
                return pos;
            LOG_INFO("checked {} of {}  possible begginnings", i, bestPos.size());
        }
        return glm::vec4(-1);
    }

    glm::vec4 IntersectionHelper::GetFirstPointFromOneSurface(Ref<SurfaceUV> s1, float divide)
    {
        std::vector<std::pair<float, int>> bestPos;
        std::vector<glm::vec4> startPos;
        int index = 0;

        for (int i = 0; i <= divide; i++)
            for (int j = 0; j <= divide; j++)
                for (int k = 0; k <= divide; k++)
                    for (int l = 0; l <= divide; l++)
                        if (i != k || j != l) {
                            glm::vec4 newPos(i / divide, j / divide, k / divide, l / divide);
                            float dist = GetDistance(newPos, s1, s1);
                            startPos.emplace_back(newPos);
                            bestPos.emplace_back(std::make_pair(dist, index++));
                        }
        std::sort(bestPos.begin(), bestPos.end());
        for (int i = 0; i < bestPos.size(); i++) {
            glm::vec4 pos = GradientMinimalization(startPos[bestPos[i].second], s1, s1);
            glm::vec4 pos2 = pos;
            if (s1->GetRollU()) {
                pos2.s = pos2.s - std::floorf(pos2.s);
                pos2.p = pos2.p - std::floorf(pos2.p);
            }
            else {
                pos2.s = glm::clamp(pos2.s, 0.f, 1.f);
                pos2.p = glm::clamp(pos2.p, 0.f, 1.f);
            }
            if (s1->GetRollV()) {
                pos2.t = pos2.t - std::floorf(pos2.t);
                pos2.q = pos2.q - std::floorf(pos2.q);
            }
            else {
                pos2.t = glm::clamp(pos2.t, 0.f, 1.f);
                pos2.q = glm::clamp(pos2.q, 0.f, 1.f);
            }

            float dist = glm::length(s1->GetPointAt(pos.s, pos.t) -
                s1->GetPointAt(pos.p, pos.q));
            //if (dist < 0.00001f)
            //	continue;		
            float coordDist = glm::length(glm::vec2(pos2.s - pos2.p, pos2.t - pos2.q));
            if (std::max(std::abs(pos2.s - pos2.p), std::abs(pos2.t - pos2.q)) > 0.999f)
                continue;
            if (dist < 0.01f && coordDist > 0.01f)
                return pos;

            LOG_INFO("checked {} of {}  possible begginnings", i, bestPos.size());
        }
        return glm::vec4(-1);
    }

    glm::vec4 IntersectionHelper::GetFirstPointFromTwoSurfacesCursor(Ref<Cursor3D> cursor, Ref<SurfaceUV> s1, Ref<SurfaceUV> s2, float divide)
    {
        float minDist[] = { 10000000, 10000000 };
        glm::vec4 pos(0.5f), newPos;
        for (int i = 0; i <= divide; i++)
            for (int j = 0; j <= divide; j++) {
                newPos = glm::vec4(i / divide, j / divide, 0.0f, 0.0f);
                float dist[2] = { GetDistance(newPos,s1,cursor),
                    GetDistance(newPos,s2,cursor) };
                if (dist[0] < minDist[0]) {
                    minDist[0] = dist[0];
                    pos.x = newPos.x;
                    pos.y = newPos.y;
                }
                if (dist[1] < minDist[1]) {
                    minDist[1] = dist[1];
                    pos.z = newPos.x;
                    pos.w = newPos.y;
                }
            }
        glm::vec4 s1Pos = GradientMinimalization(pos, s1, cursor);
        glm::vec4 s2Pos = GradientMinimalization(pos, cursor, s2);
        pos = glm::vec4(s1Pos.x, s1Pos.y, s2Pos.z, s2Pos.w);
        for (int i = 0; i < 20; i++)
        {
            pos = GradientMinimalization(pos, s1, s2);
            float dist = glm::length(s1->GetPointAt(pos.x, pos.y) -
                s2->GetPointAt(pos.z, pos.w));
            if (dist < 0.01f)
                return pos;
        }

        return glm::vec4(-1);
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
                        glm::vec4 parameters(u1, v1, u2, v2);
                        float dist = GetDistance(parameters, s1, s2);
                        startingPos.push_back(parameters);
                        bestPos.push_back(std::make_pair(dist, index++));
                    }
                }
            }
        }

        std::sort(bestPos.begin(), bestPos.end());
        for (int i = 0; i < bestPos.size(); i++)
        {
            auto parameters = startingPos[bestPos[i].second];
            glm::vec4 pos = GradientMinimalization(parameters, s1, s2);
            float dist = glm::length(s1->GetPointAt(pos.s, pos.t) - s2->GetPointAt(pos.p, pos.q));
            if (dist < 0.01f)
                return pos;

            LOG_INFO("checked {} of {}  possible begginnings", i, bestPos.size());
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
        glm::vec4 parameters,
        Ref<SurfaceUV> s1,
        Ref<SurfaceUV> s2)
    {
        //Nonlinear_conjugate_gradient_method
        //https://en.wikipedia.org/wiki/Nonlinear_conjugate_gradient_method

        glm::vec4 minValues(s1->GetMinU(), s1->GetMinV(), s2->GetMinU(), s2->GetMinV());
        glm::vec4 maxValues(s1->GetMaxU(), s1->GetMaxV(), s2->GetMaxU(), s2->GetMaxV());

        const int MAX_ITERATIONS = 200;
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
            if (alfa < 1e-6f)
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
        glm::vec4 parameters,
        Ref<SurfaceUV> s1,
        Ref<SurfaceUV> s2)
    {
        auto pos1 = s1->GetPointAt(parameters.x, parameters.y);
        auto pos2 = s2->GetPointAt(parameters.z, parameters.w);

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
        auto valueLeft = GetDistance(xLeft, s1, s2);
        auto valueRight = GetDistance(xRight, s1, s2);

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
                valueLeft = GetDistance(xLeft, s1, s2);
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
                valueRight = GetDistance(xRight, s1, s2);
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

    glm::vec4 IntersectionHelper::ClampParameters(glm::vec4& parameters, Ref<SurfaceUV> s1, Ref<SurfaceUV> s2)
    {
        bool isU1OutOfRange = parameters.x <s1->GetMinU() || parameters.x >s1->GetMaxU();
        bool isV1OutOfRange = parameters.y <s1->GetMinV() || parameters.y > s1->GetMaxV();

        bool isU2OutOfRange = parameters.z <s2->GetMinU() || parameters.z >s2->GetMaxU();
        bool isV2OutOfRange = parameters.w <s2->GetMinV() || parameters.w > s2->GetMaxV();

        if (isU1OutOfRange)
        {
            if (s1->GetRollU())
            {
                parameters.x = parameters.x - std::floor(parameters.x);
            }
            else
            {
                parameters.x = glm::clamp(parameters.x, s1->GetMinU(), s1->GetMaxU());
            }
        }

        if (isV1OutOfRange)
        {
            if (s1->GetRollV())
            {
                parameters.y = parameters.y - std::floor(parameters.y);
            }
            else
            {
                parameters.y = glm::clamp(parameters.y, s1->GetMinV(), s1->GetMaxV());
            }
        }

        if (isU2OutOfRange)
        {
            if (s2->GetRollU())
            {
                parameters.z = parameters.z - std::floor(parameters.z);
            }
            else
            {
                parameters.z = glm::clamp(parameters.z, s2->GetMinU(), s2->GetMaxU());
            }
        }

        if (isV2OutOfRange)
        {
            if (s2->GetRollV())
            {
                parameters.w = parameters.w - std::floor(parameters.w);
            }
            else
            {
                parameters.w = glm::clamp(parameters.w, s2->GetMinV(), s2->GetMaxV());
            }
        }

        return parameters;
    }

    bool IntersectionHelper::CheckParameters(glm::vec2& parameters, Ref<SurfaceUV> surface, std::vector<std::vector<glm::vec2>>& loops)
    {
        bool isUOutOfRange = parameters.x <surface->GetMinU() || parameters.x >surface->GetMaxU();
        bool isVOutOfRange = parameters.y <surface->GetMinV() || parameters.y > surface->GetMaxV();

        if (isUOutOfRange && isVOutOfRange && surface->GetRollU() && surface->GetRollV())
        {
            auto last = loops.back().back();
            glm::vec2 uBoundary = glm::vec2(surface->GetMinU(), surface->GetMaxU());
            glm::vec2 vBoundary = glm::vec2(surface->GetMinV(), surface->GetMaxV());
            glm::vec2 intersection;
            glm::vec2 intersection2;

            LineClipper::ClipLine(last, parameters, uBoundary, vBoundary, intersection, intersection2);
            parameters.x -= std::floor(parameters.x);
            parameters.y -= std::floor(parameters.y);
            loops.back().push_back(intersection);
            loops.push_back(std::vector<glm::vec2>());
            loops.back().push_back(intersection2);
            loops.back().push_back(parameters);
            isUOutOfRange = false;
            isVOutOfRange = false;
        }
        else if (isUOutOfRange && surface->GetRollU())
        {
            auto last = loops.back().back();
            glm::vec2 uBoundary = glm::vec2(surface->GetMinU(), surface->GetMaxU());
            glm::vec2 vBoundary = glm::vec2(surface->GetMinV(), surface->GetMaxV());
            glm::vec2 intersection;
            glm::vec2 intersection2;

            LineClipper::ClipLine(last, parameters, uBoundary, vBoundary, intersection, intersection2);
            parameters.x -= std::floor(parameters.x);
            loops.back().push_back(intersection);
            loops.push_back(std::vector<glm::vec2>());
            loops.back().push_back(intersection2);
            loops.back().push_back(parameters);
            isUOutOfRange = false;
        }
        else if (isVOutOfRange && surface->GetRollV())
        {
            auto last = loops.back().back();
            glm::vec2 uBoundary = glm::vec2(surface->GetMinU(), surface->GetMaxU());
            glm::vec2 vBoundary = glm::vec2(surface->GetMinV(), surface->GetMaxV());
            glm::vec2 intersection;
            glm::vec2 intersection2;

            LineClipper::ClipLine(last, parameters, uBoundary, vBoundary, intersection, intersection2);
            parameters.y -= std::floor(parameters.y);
            loops.back().push_back(intersection);
            loops.push_back(std::vector<glm::vec2>());
            loops.back().push_back(intersection2);
            loops.back().push_back(parameters);

            isVOutOfRange = false;
        }
        else if (!isVOutOfRange && !isUOutOfRange)
        {
            loops.back().push_back(parameters);
        }

        return !isUOutOfRange && !isVOutOfRange;
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

        bool isFirstOutOfRange = isU1OutOfRange || isV1OutOfRange;
        bool isSecondOutOfRange = isU2OutOfRange || isV2OutOfRange;


        if (isFirstOutOfRange && isSecondOutOfRange)
        {
            if (intersectionType == IntersectionType::ClosedClosed)
                return IntersectionType::ClosedClosed;
            else if (intersectionType == IntersectionType::ClosedOpen)
                return IntersectionType::ClosedOpen;
            else if (intersectionType == IntersectionType::OpenClosed)
                return IntersectionType::OpenClosed;
            else
                return IntersectionType::OpenOpen;
        }

        if (isFirstOutOfRange)
        {
            if (intersectionType == IntersectionType::ClosedClosed)
            {
                return IntersectionType::ClosedOpen;
            }
            else if (intersectionType == IntersectionType::ClosedOpen)
            {
                return IntersectionType::ClosedOpen;
            }
            else
            {
                return IntersectionType::OpenOpen;
            }
        }
        if (isSecondOutOfRange)
        {
            if (intersectionType == IntersectionType::ClosedClosed)
            {
                return IntersectionType::OpenClosed;
            }
            else if (intersectionType == IntersectionType::OpenClosed)
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
}
