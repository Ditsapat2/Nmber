#include "../../Core/sge.hpp"

namespace Nmber {

    constexpr unsigned GAME_SCENE=0;
    struct Vertex2D {
        float x,y,z;
        float r,g,b,a;
        float texID;
        float texCoordX, texCoordY;
    } ;

    class Game {
    public:
        explicit Game(const std::string& name, int w, int h) ;
        ~Game() ;
        void run() ;
        static void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
            glViewport(0,0,width,height);
        }
    private:
        GLFWwindow* m_window;
        SGE::Mat4f m_projMat, m_viewMat, m_modelMat;
    } ;
    
    struct GameResources {
        std::vector<SGE::Components::Register> scenes;
    } ;

    struct GraphicResources {
        SGE::BufferLayout layout;
        SGE::Program program;
        std::vector<SGE::Batch<Vertex2D>> renderer2D; 
    } ;

    struct TextureResources {
        std::vector<SGE::Texture2D> textures2D;
        std::vector<SGE::SpriteTextureAlias> sprite_sheets;
    } ;

    class Resources {
    public:
        Resources() = default;
        static inline Resources& get() { return m_instance; }
        static int add_texture2D(SGE::Texture2D&& texture, int idx=-1) {
            return get()._add_texture2D(std::move(texture), idx);
        }
        static int load_texture2D(const std::string& filePath, int idx=-1) {
            return get()._load_texture2D(filePath, idx);
        }
        static SGE::Texture2D& get_texture2D(int idx) {
            return get()._get_texture2D(idx);
        }
        static int add_spriteTextureAlias(int slot, float x1,float y1, float x2, float y2, int idx=-1) { 
            return get()._add_spriteTextureAlias(slot, x1,y1,x2,y2, idx); 
        }
        static SGE::SpriteTextureAlias& get_spriteTextureAlias(int idx) {
            return get().m_textures.sprite_sheets.at(idx);
        }
        static int add_scene(int idx=-1) {
            return get()._add_scene(idx);
        }
        static SGE::Components::Register& get_scene(int idx) {
            return get()._get_scene(idx);
        }
        static GraphicResources& get_graphic() { return get().m_graphic; }

    private:
        int _add_texture2D(SGE::Texture2D&& texture, int idx) ;
        int _load_texture2D(const std::string& filePath, int idx) ;
        SGE::Texture2D& _get_texture2D(int idx) ;
        int _add_spriteTextureAlias(int slot, float x1,float y1, float x2, float y2, int idx) ;
        int _add_scene(int idx) ;
        SGE::Components::Register& _get_scene(int idx) ;
    private:
        static Resources m_instance;
        TextureResources m_textures;
        GameResources m_game;
        GraphicResources m_graphic;
    } ;
    inline Resources Resources::m_instance;
}