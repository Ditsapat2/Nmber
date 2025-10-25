#include "texture.hpp"
namespace SGE {
    void Texture2D::_init(unsigned w, unsigned h, void* data, TextureFormat format) {
        glGenTextures(1, &m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (data!=nullptr) {
            glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
        } else {
            std::cout << "Invalid texture data\n";
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    Texture2D::Texture2D(unsigned w, unsigned h, void* data, TextureFormat format) {
        _init(w,h,data,format);
    }
    Texture2D::Texture2D(const std::string& filepath) {
        int w, h, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(filepath.c_str(), &w, &h, &nrChannels, 0);
        TextureFormat format;
        if (filepath.find(".jpg") != std::string::npos) {
            format = RGB;
        } else if (filepath.find(".png") != std::string::npos) {
            format = RGBA;
        }
        _init(w,h,data,format);     
        stbi_image_free(data);
    }
    Texture2D::Texture2D(Texture2D&& other) {
        m_texture = other.m_texture;
        other.m_texture = NULL_TEXTURE;
    }
    void Texture2D::operator=(Texture2D&& other) {
        m_texture = other.m_texture;
        other.m_texture = NULL_TEXTURE;        
    }
    Texture2D::~Texture2D() {
        if (m_texture==NULL_TEXTURE) return;
        glDeleteTextures(1, &m_texture);
    }
    void Texture2D::bind(unsigned slot) {
        glActiveTexture(GL_TEXTURE0+slot);
        glBindTexture(GL_TEXTURE_2D, m_texture);
    }
}