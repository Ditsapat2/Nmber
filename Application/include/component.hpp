#pragma once
#include "Core/sge.hpp"

namespace Nmber {
    using Force = SGE::Vec<3, float>;
    using Position = SGE::Vec<3, float>;
    using Size = SGE::Vec<2, float>;

    using KeyCallback = void(*)(GLFWwindow* window, void*, int, int, int, int) ;

    struct Transform {
        float x,y;
        float w,h;
        Transform()=default;
        Transform(float x,float y, float w,float h) : x(x), y(y), w(w), h(h) {}
    } ;
    struct Layer {
        int layer;
        Layer()=default;
        Layer(int layer) : layer(layer) {}
    } ;
    struct DrawResource {
        int id=-1;
        float r=1.f,g=1.f,b=1.f,a=1.f;
        DrawResource()=default;
        DrawResource(int id) : id(id) {}
        DrawResource(float r,float g,float b,float a) : r(r), g(g), b(b), a(a) {}
    } ;

    struct KeyActions {
        KeyCallback callbacks;
    } ;
}