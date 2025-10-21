#include "buffer_layout.hpp"
namespace SGE {

    inline std::size_t type_size(Type type) {
        switch (type) {
            case FLOAT: return sizeof(Float);

            default: return 0;
        }
    }
    
    BufferLayout::BufferLayout(std::initializer_list<Element> elements) {}
    BufferLayout::~BufferLayout() {
        glDeleteVertexArrays(1, &m_vao);
    }

    void BufferLayout::push(Type type, std::size_t size) {
        m_elements.emplace_back(type, size);
        m_stride+=type_size(type)*size;
    }
    void BufferLayout::update() {
        std::size_t offset=0;
        std::size_t i=0;
        for (auto& [type, size] : m_elements) {
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, size, type, FALSE, m_stride, reinterpret_cast<const void*>(offset));
            ++i;
            offset+=type_size(type)*size;
        }
        unbind();
    }
    void BufferLayout::bind() {
        if (m_vao==0) glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
    }
    BufferLayout& BufferLayout::bind_with() {
        bind();
        return *this;
    }
    void BufferLayout::unbind() {
        glBindVertexArray(0);
    }
}