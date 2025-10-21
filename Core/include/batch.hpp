#pragma once
#include "config.h"
#include "buffer.hpp"
#include "buffer_layout.hpp"
#include <vector>

namespace SGE {

    template<class TYPE>
    constexpr GLenum enum_type() ;
    template<> constexpr GLenum enum_type<unsigned>() { return GL_UNSIGNED_INT; }

    template<class VERTEX, class INDEX=unsigned>
    class Batch {
    public:
        Batch() : m_vbo(ARRAY_BUFFER, nullptr, 0), m_ibo(ELEMENT_ARRAY_BUFFER, nullptr, 0) {
        }
        void push_vertex(VERTEX&& vertex) {
            m_vertexes.emplace_back(std::move(vertex));
        }
        void push_vertex(const VERTEX& vertex) {
            m_vertexes.emplace_back(vertex);
        }
        void push_index(INDEX&& index) {
            m_indices.emplace_back(std::forward<INDEX>(index));
        }
        void push(const VERTEX* vertexes, std::size_t vertexesSize, const INDEX* indices, std::size_t indicesSize, int indicesOffset=-1) {
            for (unsigned i=0; i<indicesSize; ++i) {
                if (indicesOffset<0) {
                    push_index(indices[i]+m_vertexes.size());
                } else {
                   push_index(indices[i]+indicesOffset);
                }
            } 
            for (unsigned i=0; i<vertexesSize; ++i) {
                push_vertex(vertexes[i]);
            }
        }
        void clear() {
            m_vertexes.clear();
            m_indices.clear();
        }
        void update_buffers() {
            bool n=false;
            if (m_vertexes.size()*sizeof(VERTEX) > m_vbo.size()) {
                m_vbo.allocate_newBuffer(nullptr,m_vertexes.size()*sizeof(VERTEX)*2);
                n=true;
            }
            if (m_indices.size()*sizeof(INDEX) > m_ibo.size()) {
                m_ibo.allocate_newBuffer(nullptr, m_indices.size()*sizeof(INDEX)*2);
                n=true;
            }
            if (n) {
                m_pLayout->bind();
            }
            m_vbo.bind_with().bufferSubData(0, m_vertexes.data(), m_vertexes.size()*sizeof(VERTEX));
            m_ibo.bind_with().bufferSubData(0, m_indices.data(), m_indices.size()*sizeof(INDEX));
            if (n) {
                m_pLayout->update();
            }
        }
        void bind_layout(BufferLayout* layout) {
            m_pLayout=layout;
        }
        void bind() {
            m_ibo.bind();
            m_vbo.bind();
        }
        void draw() {
            m_pLayout->bind();
            m_ibo.bind();
            m_vbo.bind();
            glDrawElements(GL_TRIANGLES, m_indices.size(), enum_type<INDEX>(), nullptr);
        }
    private:
        std::vector<VERTEX> m_vertexes;
        std::vector<INDEX> m_indices;
        Buffer m_vbo, m_ibo;
        BufferLayout* m_pLayout=nullptr;
    } ;
    inline int gMaxBindedTextures = 8;
    template<class VERTEX>
    inline int decider_textureSlot(VERTEX* vertex) {
        return (*reinterpret_cast<int*>(vertex.texID))/gMaxBindedTextures;
    }
}