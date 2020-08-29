#pragma once
#include "cadpch.h"
#include <glm\glm.hpp>
#include "Core/Base.h"

typedef unsigned int GLenum;

namespace CADMageddon
{
    class OpenGLShader
    {
    public:
        OpenGLShader(const std::string& filepath);
        OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
        virtual ~OpenGLShader();

        void Bind() const;
        void Unbind() const;

        void SetInt(const std::string& name, int value);
        void SetIntArray(const std::string& name, int* values, uint32_t count);
        void SetFloat(const std::string& name, float value);
        void SetFloat3(const std::string& name, const glm::vec3& value);
        void SetFloat3Array(const std::string& name, glm::vec3 * values, uint32_t count);
        void SetFloat4(const std::string& name, const glm::vec4& value);
        void SetMat4(const std::string& name, const glm::mat4& value);
        void SetBool(const std::string& name, bool value);

        const std::string& GetName() const { return m_Name; }

        void UploadUniformInt(const std::string& name, int value);
        void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);

        void UploadUniformFloat(const std::string& name, float value);
        void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
        void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
        void UploadUniformFloat3Array(const std::string& name, glm::vec3* values, uint32_t count);
        void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

        void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
        void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
    private:
        std::string ReadFile(const std::string& filepath);
        std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
        void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
    private:
        uint32_t m_RendererID;
        std::string m_Name;
    };

    class ShaderLibrary
    {
    public:
        Ref<OpenGLShader> Load(const std::string& name, const std::string& filepath);
        Ref<OpenGLShader> Get(const std::string& name);

    private:
        std::unordered_map<std::string, Ref<OpenGLShader>> m_Shaders;
    };
}