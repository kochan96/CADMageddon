#include "Cursor3D.h"

glm::vec3 CADMageddon::Cursor3D::GetPointAt(float u, float v)
{
    return getPosition();
}

glm::vec3 CADMageddon::Cursor3D::GetTangentUAt(float u, float v)
{
    return glm::vec3(0);
}

glm::vec3 CADMageddon::Cursor3D::GetTangentVAt(float u, float v)
{
    return glm::vec3(0);
}

float CADMageddon::Cursor3D::GetMinU() const
{
    return 0.0f;
}

float CADMageddon::Cursor3D::GetMaxU() const
{
    return 1.0f;
}

float CADMageddon::Cursor3D::GetMinV() const
{
    return 0.0f;
}

float CADMageddon::Cursor3D::GetMaxV() const
{
    return 1.0f;
}

int CADMageddon::Cursor3D::GetUDivision() const
{
    return 0;
}

int CADMageddon::Cursor3D::GetVDivision() const
{
    return 0;
}
