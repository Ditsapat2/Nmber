#include "buffer.hpp"
namespace SGE {
       Buffer::Buffer(BufferTarget target, void* init, std::size_t size, BufferUsage usage) :
       m_target(target), m_size(size), m_usage(usage)
       {
          if (size==0) return;
            glGenBuffers(1, &m_buffer);
            glBindBuffer(m_target, m_buffer);
            glBufferData(m_target, m_size, init, m_usage);
       }
       Buffer::~Buffer() {
            if (m_buffer==NULL_BUFFER) return;
            glDeleteBuffers(1, &m_buffer);
            m_buffer = NULL_BUFFER;
       }
        
       void Buffer::bufferData(const GLvoid* data) {
            glBufferData(m_target, m_size, data, m_usage);
       }
       void Buffer::bufferSubData(std::size_t offset, const GLvoid* data, std::size_t size) {
            glBufferSubData(m_target, offset, size, data);
       }

       void Buffer::bind() {
            if (m_buffer==NULL_BUFFER) {
               glGenBuffers(1, &m_buffer);
               glBindBuffer(m_target, m_buffer);
               glBufferData(m_target, m_size, nullptr, m_usage);
            }
            else {               
               glBindBuffer(m_target, m_buffer);
            }
       }
       Buffer& Buffer::bind_with() {
            bind();    
            return *this;
       }
       void Buffer::unbind() {
            glBindBuffer(m_target, 0);
       }
       void Buffer::allocate_newBuffer(void* data, std::size_t size) {
            if (m_buffer!=NULL_BUFFER)
               glDeleteBuffers(1, &m_buffer);  
            m_size=size;
            glGenBuffers(1, &m_buffer);
            glBindBuffer(m_target, m_buffer);
            glBufferData(m_target, m_size, data, m_usage);
       }
}
