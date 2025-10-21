#pragma once
#include "config.h"

namespace SGE {

    using BufferTarget = GLenum;
    using BufferUsage = GLenum;

    constexpr GLuint NULL_BUFFER=0;

    constexpr BufferTarget
        ARRAY_BUFFER=GL_ARRAY_BUFFER,
        ELEMENT_ARRAY_BUFFER=GL_ELEMENT_ARRAY_BUFFER
    ;
    constexpr BufferUsage
        STATIC_DRAW=GL_STATIC_DRAW
    ;

    class Buffer {
    public:
        Buffer() = default;
        Buffer(BufferTarget target, void* init, std::size_t size, BufferUsage usage=STATIC_DRAW) ;
        ~Buffer() ;
        
        void bufferData(const GLvoid* data) ;
        void bufferSubData(std::size_t offset, const GLvoid* data, std::size_t size) ;

        void bind();
        Buffer& bind_with() ;
        void unbind();

        inline std::size_t size() const { return m_size; }

        void allocate_newBuffer(void* data, std::size_t size) ;

    private:
        GLuint m_buffer=NULL_BUFFER; 
        BufferTarget m_target;
        std::size_t m_size=0;
        BufferUsage m_usage;
    };
}