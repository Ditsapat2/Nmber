#include "GLAD/glad.h"
#include "GLFW/glfw3.h"
#include "Core/sge.hpp"
#include "include/game.hpp"
#include "include/component.hpp"
#include <iostream>

struct Vertex {
    float x,y;
    float r,g,b,a;
    float texID;
    float texCoordX, texCoordY;
} ;

SGE::Mat4f proj;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    proj = SGE::ortho_mat(width, 0.f, height, 0.f, 1.f, -1.f);
}  
using namespace Nmber;

int main(void)
{
    SGE::init();
    Game game("GL", 640,480);
    game.run();
    std::cout << "Application end\n";
    glfwTerminate();
    return 0;
}