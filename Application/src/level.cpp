#include "../include/level.hpp"
#include "../include/component.hpp"
#include "../include/game.hpp"

namespace Nmber {
    int make_tileset(int slot, int width, int height, TextureResources& textures, int idx) {
        float x,y;
        y=1.f;
        float w=1.f/width, h=1.f/height;
        int r;
        if (idx<0) r = textures.sprite_sheets.size();
        else r = idx;
        for (int i=0; i<height; ++i) {
            x=0.f;
            for (int j=0; j<width; ++j) {
                if (idx<0) textures.sprite_sheets.emplace_back(slot, x,y-h, x+w,y);
                x+=w;
            }
            y-=h;
        }
        return r;
    }
    Level::Level(std::istream& file) : m_data(JSON::parse(file)) {
    }
    Level::~Level() {
    }
    int Level::load_level(SGE::Components::Register& reg, TileLayout layout, int offsetX, int offsetY, int offsetID) {
        int zLayer=0;
        float locTileWidth = m_data.at("tilewidth").get<int>();
        float locTileHeight = m_data.at("tileheight").get<int>();
        const int offset_formatID = 1;

        for (auto& layer : m_data.at("layers")) {
            int locOffsetX, locOffsetY;
            if (layer.contains("offsetx")) {
                locOffsetX=layer.at("offsetx").get<int>()*(layout.tileWidth/locTileWidth);
            } else locOffsetX=0;
            if (layer.contains("offsety")) {
                locOffsetY=layer.at("offsety").get<int>()*(layout.tileHeight/locTileHeight);
            } else locOffsetY=0;

        for (auto& chunk : layer.at("chunks")) {
            float w, h;
            w = chunk.at("width").get<int>();
            h = chunk.at("height").get<int>();
            int sx,sy;
            float x,y;

            sx = chunk.at("x").get<int>()+offsetX;
            sy = chunk.at("y").get<int>()+offsetY;
            y=layout.y-sy*layout.tileHeight-locOffsetY;
            int i=0, j=0;
            for (j=0; j<h; ++j) {
                x=layout.x+sx*layout.tileWidth+locOffsetX;
            for (i=0; i<w; ++i) {
                auto id = chunk.at("data")[i+(h-j-1)*w].get<int>();
                if (id>0) {
                    m_tiles.emplace_back(reg, reg.construct_entity(
                        Transform(x,y, layout.tileWidth,layout.tileHeight),
                        DrawResource(id+offsetID-offset_formatID),
                        Layer(zLayer)
                    )
                    );
                }
                x+=layout.tileWidth;
            }
                y+=layout.tileHeight;
            }
        }
            ++zLayer;
        }
        return 0;
    }
}