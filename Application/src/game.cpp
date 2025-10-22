#include "../include/game.hpp"
#include "../include/component.hpp"
#include "../include/level.hpp"

namespace Nmber {
    Game::~Game() {

    }
    Game::Game(const std::string& name, int w, int h) {
        std::cout << "HEllo\n";
        if (!glfwInit()) {

        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        m_window = glfwCreateWindow(w, h, name.c_str(), NULL, NULL);
        if (!m_window)
        {
            glfwTerminate();
            return ;
        }
        glfwMakeContextCurrent(m_window);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return ;
        }
        auto a = Resources::load_texture2D("../../../image/sixseven.jpg");
        Resources::get_texture2D(a).bind(0);
        Resources::add_spriteTextureAlias(0, 0,0, 1,1);

        Resources::add_scene(GAME_SCENE);
        m_modelMat = SGE::identity_mat();
        m_viewMat = SGE::identity_mat();
        m_projMat = SGE::ortho_mat(w,0, h,0, -1,10);
        glfwSetFramebufferSizeCallback(m_window, frameBufferSizeCallback);
    }
    void Game::run() {
        
        auto player = Resources::get_scene(GAME_SCENE).construct_entity(
            Transform{0,0,0, 100,100},
            DrawResource(0)
        );
    
       Resources::get_graphic().renderer2D.emplace_back();

        Resources::get_graphic().renderer2D[0].bind_layout(&Resources::get_graphic().layout);
        Resources::get_graphic().renderer2D[0].bind();
        Resources::get_graphic().layout.push_layout(
            SGE::FLOAT, 3,
            SGE::FLOAT, 4,
            SGE::FLOAT, 1,
            SGE::FLOAT, 2
        );
       Resources::get_graphic().layout.set_stride(sizeof(Vertex2D));

       auto shaders = SGE::shaders_fromFile(std::ifstream("../../../Application/shader/test.glsl"));
       Resources::get_graphic().program = SGE::Program(shaders);

       Resources::get_graphic().program.bind_with().uniform_mat4(
            "u_proj", m_projMat,
            "u_view", m_viewMat,
            "u_model", m_modelMat
       );
       int t[] = { 0 };
       Resources::get_graphic().program.bind_with().uniform_1iv(
            "u_textures", t, 1
       );

       Resources::get_scene(GAME_SCENE).construct_entity(
        Transform{100,100,.1f, 100,100},
        DrawResource(1.f,0.f,0.f,1.f)
       );
       Level level(std::ifstream("../../../Application/map/test.tmj"));
       level.load_level(Resources::get_scene(GAME_SCENE));
       glEnable(GL_DEPTH_TEST);
       glDepthFunc(GL_LESS);
       SGE::Time::get().restart();
        while (!glfwWindowShouldClose(m_window))
        {
            SGE::Time::get().update();
            auto delta = SGE::Time::get().get_elapsedTime();
            glClearColor(0.431, 0.862, 0.941, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
          //  Resources::get_graphic().layout.bind();
            // ibo.bind();
            // vbo.bind();
            auto [t] = Resources::get_scene(GAME_SCENE).get<Transform>(player);
            SGE::Vec<2,float> f({0,0});
            auto force = SGE::position_view(f);
            float s=100.f;
            if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) {
                force.x+=s;
            } if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) {
                force.y+=s;
            } if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) {
                force.x+=-s;
            } if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) {
                force.y+=-s;
            }
            double rad = std::atan2(force.y, force.x);
            t.x+=std::abs(force.x)*std::cos(rad)*delta;
            t.y+=std::abs(force.y)*std::sin(rad)*delta;

            Resources::get_graphic().renderer2D[0].clear();
            for (auto [t,d] : Resources::get_scene(GAME_SCENE).view<Transform, DrawResource>()) {
                float texID;
                if (d.id==-1) texID=-1;
                else texID = Resources::get_spriteTextureAlias(d.id).slot;
                Vertex2D v[] = {
                    t.x,t.y,t.z, d.r,d.g,d.b,d.a, texID, 0.f,0.f,
                    t.x,t.y+t.h,t.z, d.r,d.g,d.b,d.a, texID, 0.f,1.f,
                    t.x+t.w,t.y,t.z, d.r,d.g,d.b,d.a, texID, 1.f,0.f,
                    t.x+t.w,t.y+t.h,t.z, d.r,d.g,d.b,d.a, texID, 1.f,1.f
                } ;
                unsigned i[] = {0,1,2,1,2,3};

                Resources::get_graphic().renderer2D[0].push(v,4,i,6);
            }
            int w,h;
            glfwGetFramebufferSize(m_window, &w,&h);
            m_projMat=SGE::ortho_mat(w,0,h,0,-1,1);
            Resources::get_graphic().program.bind_with().uniform_mat4(
                "u_proj", m_projMat
            );
            Resources::get_graphic().renderer2D[0].update_buffers();

            Resources::get_graphic().renderer2D[0].draw();
            /* Swap front and back buffers */
            glfwSwapBuffers(m_window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }
    int Resources::_add_texture2D(SGE::Texture2D&& texture, int idx) {
        if (idx<0) {
            m_textures.textures2D.emplace_back(std::move(texture));
            return m_textures.textures2D.size()-1;
        }
        if (idx>=m_textures.textures2D.size()) m_textures.textures2D.resize(idx+1);
        m_textures.textures2D[idx] = std::move(texture);
        return idx;
    }
    int Resources::_load_texture2D(const std::string& filePath, int idx) {
        if (idx<0) {
            m_textures.textures2D.emplace_back(filePath);
            return m_textures.textures2D.size()-1;
        }
        if (idx>=m_textures.textures2D.size()) m_textures.textures2D.resize(idx+1);
        m_textures.textures2D[idx] = SGE::Texture2D(filePath);
        return idx;
    }
    SGE::Texture2D& Resources::_get_texture2D(int idx) {
        return m_textures.textures2D.at(idx);
    }
    int Resources::_add_spriteTextureAlias(int slot, float x1,float y1, float x2, float y2, int idx) {
        if (idx<0) {
            m_textures.sprite_sheets.emplace_back(slot,x1,y1,x2,y2);
            return m_textures.sprite_sheets.size()-1;
        }
        if (idx>=m_textures.sprite_sheets.size()) m_textures.sprite_sheets.resize(idx+1);
        m_textures.sprite_sheets.emplace_back(slot,x1,y1,x2,y2);
        return idx;
    }
    int Resources::_add_scene(int idx) {
        if (idx<0) {
            m_game.scenes.emplace_back();
            return m_game.scenes.size()-1;
        }
        if (idx>=m_game.scenes.size()) m_game.scenes.resize(idx+1);
        m_game.scenes[idx] = SGE::Components::Register();
        return idx;
    }
    SGE::Components::Register& Resources::_get_scene(int idx) {
        return m_game.scenes.at(idx);
    }
}