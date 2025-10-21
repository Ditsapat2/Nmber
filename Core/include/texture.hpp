#pragma once
#include "config.h"
namespace SGE {
    using TextureFormat = GLenum;
    constexpr GLuint NULL_TEXTURE=0;
    constexpr TextureFormat
        RGB = GL_RGB,
        RGBA = GL_RGBA
    ;
    struct SpriteTextureAlias {
        int slot;
        float x1,y1,x2,y2;
        SpriteTextureAlias()=default;
        SpriteTextureAlias(int slot, float x1,float y1,float x2,float y2) :
            slot(slot),x1(x1),y1(y1),x2(x2),y2(y2) {}
    } ;
    class Texture2D {
    public:
        Texture2D()=default;
        Texture2D(unsigned w, unsigned h, void* data, TextureFormat format) ;
        Texture2D(const std::string& filepath) ;
        ~Texture2D() ;

        void bind(unsigned slot) ;
        void unbind() ;
    private:
        void _init(unsigned w, unsigned h, void* data, TextureFormat format) ;
    private:
        GLuint m_texture;
    } ;
}