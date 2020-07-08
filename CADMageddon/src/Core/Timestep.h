#pragma once
namespace CADMageddon
{
    class Timestep
    {
    public:
        Timestep(float time = 0.0f) :m_time(time) {}

        operator float() const { return m_time; }

        float getSeconds() const { return m_time; }

        float getMiliSeconds() const { return m_time * 1000.0f; }

    private:
        float m_time;
    };
}