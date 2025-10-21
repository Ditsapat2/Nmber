#include "shader.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <unordered_map>

namespace SGE {
    void Shader::m_makeShader(ShaderType type, const std::string& source) {
        m_shader = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(m_shader, 1, &src, nullptr);
        glCompileShader(m_shader);

        int success;
        glGetShaderiv(m_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            GLint length;
            glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &length);
            char* str = (char*)alloca(sizeof(char)*length);
            glGetShaderInfoLog(m_shader, length, NULL, str);
            if (type==VERTEX_SHADER)
                std::cout << "Vertex shader\'s error: \n";
            else if (type==FRAGMENT_SHADER)
                std::cout << "Fragment shader\'s error: \n";
            std::cout << str ;
        }
    }
    Shader::Shader(ShaderType type, const std::string& source) : m_type(type) {
        m_makeShader(m_type, source);
    }
    Shader::Shader(ShaderType type, std::istream&& file) {
        std::stringstream string;
        string << file.rdbuf();
        m_makeShader(m_type, string.str());
    }
    Shader::Shader(Shader&& other) {
        m_shader = other.m_shader;
        m_type = other.m_type;
        other.m_shader = NULL_SHADER;
    }
    void Shader::operator=(Shader&& other) {
        m_shader = other.m_shader;
        m_type = other.m_type;
        other.m_shader = NULL_SHADER;        
    }
    Shader::~Shader() {
        if (m_shader == NULL_SHADER) return;
        glDeleteShader(m_shader);
    }
    Shaders shaders_fromFile(std::istream&& file) {
        std::unordered_map<ShaderType, std::stringstream, hash_enum<ShaderType>> map;
        Shaders shaders;
        std::string line;
        decltype(map)::iterator it;

        while (std::getline(file, line)) {
            if (line.find("#shader vertex")!=std::string::npos) {
                map.emplace(VERTEX_SHADER, std::stringstream());
                it = map.find(VERTEX_SHADER);
                continue;
            } else if (line.find("#shader fragment")!=std::string::npos) {
                map.emplace(FRAGMENT_SHADER, std::stringstream());
                it = map.find(FRAGMENT_SHADER);
                continue;
            }
            it->second << line << '\n';
        }
        for (auto& [type, stream] : map) {
            shaders.emplace(type, Shader(type, stream.str()));
        }
        return shaders;
    }

    Program::Program(std::initializer_list<Pair> init) {
        m_program = glCreateProgram();
        for (auto& item : init) {
            m_shaders.emplace_back(item);
            glAttachShader(m_program, item.second->m_shader);
        }
        glLinkProgram(m_program);
    }
    Program::Program(Shaders& shaders)  {
        m_program = glCreateProgram();
        for (auto& [type, shader] : shaders) {
            m_shaders.emplace_back(type, &shader);
            glAttachShader(m_program, shader.m_shader);
        }
        glLinkProgram(m_program);
    }

    Program::Program(Program&& other) {
        m_program = other.m_program;
        m_shaders = std::move(other.m_shaders);
        m_uniformLocations = std::move(other.m_uniformLocations);
        other.m_program = NULL_PROGRAM;
    }
    void Program::operator=(Program&& other) {
        m_program = other.m_program;
        m_shaders = std::move(other.m_shaders);
        m_uniformLocations = std::move(other.m_uniformLocations);
        other.m_program = NULL_PROGRAM;
    }

    Program::~Program() {
        if (m_program==NULL_PROGRAM) return;
        for (auto& [type, shader] : m_shaders) {
            glDetachShader(m_program, shader->m_shader);
        }
        glDeleteProgram(m_program);
    }
    void Program::bind() {
        glUseProgram(m_program);
    }
    void Program::unbind() {
        glUseProgram(0);
    }
    int Program::uniform_location(const std::string& name) {
        if (m_uniformLocations.find(name)==m_uniformLocations.end()) {
            m_uniformLocations.emplace(name, glGetUniformLocation(m_program, name.c_str()));
        }
        return m_uniformLocations.at(name);
    }
}