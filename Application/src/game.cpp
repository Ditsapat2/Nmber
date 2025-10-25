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
        Resources::add_spriteTextureAlias(0, 0,0, 1,1);

        Resources::add_scene(GAME_SCENE);
        m_modelMat = SGE::identity_mat();
        m_viewMat = SGE::identity_mat();
        m_projMat = SGE::ortho_mat(w,0, h,0, -10,10);
        glfwSetFramebufferSizeCallback(m_window, frameBufferSizeCallback);
    }
    void Game::run() {
        
        auto player = Resources::get_scene(GAME_SCENE).construct_entity(
            Transform(0,0, 100,100),
            DrawResource(0),
            Layer(1)
        );
    
        Resources::get_graphic().renderer2D.emplace_back();

        Resources::get_graphic().renderer2D[0].bind_layout(&Resources::get_graphic().layout);
        Resources::get_graphic().renderer2D[0].bind();
        Resources::get_graphic().layout.push_layout(
            SGE::FLOAT, 2,
            SGE::FLOAT, 4,
            SGE::FLOAT, 1,
            SGE::FLOAT, 2
        );
        Resources::get_graphic().layout.set_stride(sizeof(Vertex2D));

        auto shaders = SGE::shaders_fromFile(std::ifstream("../../../Application/shader/test.glsl"));
        Resources::get_graphic().program = SGE::Program(shaders);

        std::ifstream file("../../../Application/map/test.tmj");
        Level level(file);
        auto tileIMG = Resources::load_texture2D("C:/Users/Lenovo Thinkpad T14/Pictures/Saved Pictures/terrain_atlas.png");

        int TILE = make_tileset(1, 32,32, Resources::get_texture());
        level.load_level(Resources::get_scene(GAME_SCENE), {32.f,32.f, 0.f,0.f}, 16, 64, TILE);

        Resources::get_graphic().program.bind_with().uniform_mat4(
            "u_proj", m_projMat,
            "u_view", m_viewMat,
            "u_model", m_modelMat
        );
        int t[] = { 0,1,2,3,4,5,6,7,8,9 };
        Resources::get_graphic().program.bind_with().uniform_1iv(
            "u_textures", t, 10
        );
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


        SGE::Time::get().restart();
        while (!glfwWindowShouldClose(m_window))
        {
            Resources::get_texture2D(0).bind(0);
            Resources::get_texture2D(1).bind(1);
            SGE::Time::get().update();
            auto delta = SGE::Time::get().get_elapsedTime();
            glClearColor(0.431, 0.862, 0.941, 1.0);
            glClear(GL_COLOR_BUFFER_BIT);
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

            std::vector<SGE::Components::EntityObject> entities;
            Resources::get_graphic().renderer2D[0].clear();
            for (auto [entity,t,d,l] : Resources::get_scene(GAME_SCENE).entity_view<Transform, DrawResource, Layer>()) {
                if (!Resources::get_scene(GAME_SCENE).has<Layer>(entity))
                    std::cout << entity << " false\n";
                entities.emplace_back(Resources::get_scene(GAME_SCENE), entity);
            }

            auto depthSort = [](SGE::Components::EntityObject& a, SGE::Components::EntityObject& b) {
                auto [la,ta] = a.owner()->get<Layer, Transform>(a.entity());
                auto [lb,tb] = b.owner()->get<Layer, Transform>(b.entity());

                if (la.layer==lb.layer) {
                    return ta.y>tb.y;
                }
                return la.layer < lb.layer;
            } ;
            std::sort(entities.begin(), entities.end(), depthSort);
            for (auto& entity : entities) {
                auto [t,d] = Resources::get_scene(GAME_SCENE).get<Transform, DrawResource>(entity.entity());
                float texID, x1,y1, x2,y2;
                if (d.id==-1) {
                    texID=-1;
                    x1=0.f;
                    y1=0.f;
                    x2=1.f;
                    y2=1.f;
                }
                else {
                    auto& alias = Resources::get_spriteTextureAlias(d.id);
                    texID = alias.slot;
                    x1=alias.x1;
                    y1=alias.y1;
                    x2=alias.x2;
                    y2=alias.y2;
                }

                Vertex2D v[] = {
                    t.x,t.y, d.r,d.g,d.b,d.a, (float)texID, (float)x1,(float)y1,
                    t.x,t.y+t.h, d.r,d.g,d.b,d.a, (float)texID, (float)x1,(float)y2,
                    t.x+t.w,t.y, d.r,d.g,d.b,d.a, (float)texID, (float)x2,(float)y1,
                    t.x+t.w,t.y+t.h, d.r,d.g,d.b,d.a, (float)texID, (float)x2,(float)y2
                } ;
                unsigned i[] = {0,1,2,1,2,3};

                Resources::get_graphic().renderer2D[0].push(v,4,i,6);                
            }
            int w,h;
            glfwGetFramebufferSize(m_window, &w,&h);
            m_projMat=SGE::ortho_mat(w,0,h,0,-10,10);
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