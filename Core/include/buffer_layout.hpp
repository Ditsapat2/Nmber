#pragma once
#include "config.h"
#include <vector>
namespace SGE {
    using Type = GLenum;
    constexpr Type
        FLOAT=GL_FLOAT
    ;
    constexpr GLboolean FALSE=GL_FALSE, TRUE=GL_TRUE;
    using Float = GLfloat;

    class BufferLayout {
    public:
        struct Element {
            Type type;
            std::size_t size;
            Element()=default;
            Element(Type type, std::size_t size) : type(type), size(size) {}
        };
        BufferLayout() = default;
        BufferLayout(std::initializer_list<Element> elements) ;
        ~BufferLayout() ;

        void push(Type type, std::size_t size) ;
        template<class... Args>
        void push_layout(Type type, std::size_t size, Args... args) {
            push(type, size);
            if constexpr (sizeof...(Args)>0) {
               push_layout(args...);
            }
        }
        inline void set_stride(std::size_t size) { m_stride=size; }
        void update() ;

        void bind() ;
        BufferLayout& bind_with() ;
        void unbind() ;
    private:
        GLuint m_vao=0;
        std::size_t m_stride=0;
        std::vector<Element> m_elements;
    };
}