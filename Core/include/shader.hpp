#pragma once
#include "config.h"
#include "matrix.hpp"
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <iostream>

namespace SGE {
    using ShaderType = GLenum;
    constexpr ShaderType
        NO_SHADER=GL_NONE,
        VERTEX_SHADER=GL_VERTEX_SHADER,
        FRAGMENT_SHADER=GL_FRAGMENT_SHADER
    ;
    constexpr GLuint
        NULL_SHADER=0,
        NULL_PROGRAM=0
    ;
    class Shader {
    public:
        Shader() = default;
        Shader(ShaderType type, const std::string& source) ;
        Shader(ShaderType type, std::istream&& file) ;
        Shader(Shader&& other) ;
        void operator=(Shader&& other) ;
        ~Shader() ;
        friend class Program;
    private:
        void m_makeShader(ShaderType type, const std::string& source) ;
    private:
        GLuint m_shader;
        ShaderType m_type;
    };

    template<class ENUM>
    struct hash_enum {
        std::size_t operator()(const ENUM& e) const {
            return std::hash<int>()(static_cast<int>(e));
        } 
    };

    using Shaders = std::unordered_map<ShaderType, Shader, hash_enum<ShaderType>>;
    Shaders shaders_fromFile(std::istream&& file) ;

    class Program {
    public:
        using Pair = std::pair<ShaderType, Shader*>;
        Program()=default;
        Program(std::initializer_list<Pair> init);
        Program(Program&& other) ;
        void operator=(Program&& other) ;
        Program(Shaders& shaders) ;
        ~Program() ;
        void bind() ;
        Program& bind_with() { bind(); return *this; }
        void unbind();
        int uniform_location(const std::string& name) ;
        template<class... Args>
        void uniform_mat4(const std::string& name, const Mat4f& mat, Args... args) {
            if (uniform_location(name)==-1) 
                std::cout << "Warning: uniform \"" << name << "\" does m=not exist\n";
            glUniformMatrix4fv(uniform_location(name), 1, GL_FALSE, mat.data());
            if constexpr (sizeof...(args)>0) {
                uniform_mat4(args...);
            }
        }
        template<class... Args>
        void uniform_1iv(const std::string& name, const int* data, int size, Args... args) {
            if (uniform_location(name)==-1) 
                std::cout << "Warning: uniform \"" << name << "\" does m=not exist\n";
            glUniform1iv(uniform_location(name), size, data);
            if constexpr (sizeof...(args)>0) {
                uniform_iv(args...);
            }            
        }
    private:
        GLuint m_program=0;
        std::unordered_map<std::string, int> m_uniformLocations;
        std::vector<Pair> m_shaders;
    };
}