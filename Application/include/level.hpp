#pragma once
#include "nlohmann/json.hpp"
#include "Core/sge.hpp"

namespace Nmber {
    using JSON = nlohmann::json;

    struct TileLayout {
        float tileWidth, tileHeight;
        float x,y;
    } ;
    struct TextureResources;
    int make_tileset(int slot, int width, int height, TextureResources& textures, int idx=-1) ;
    class Level {
    public:
        Level()=default;
        Level(std::istream& string) ;
        ~Level() ;

        int load_level(SGE::Components::Register& reg, TileLayout layout, int offsetX=0, int offsetY=0, int offsetID=0) ;
    private:
        JSON m_data;
        std::vector<SGE::Components::EntityObject> m_tiles;
    } ;
}