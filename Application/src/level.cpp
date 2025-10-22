#include "../include/level.hpp"
#include "../include/component.hpp"

namespace Nmber {
    Level::Level(std::istream&& file) {
        file >> m_data;
    }
    Level::~Level() {
    }
    int Level::load_level(SGE::Components::Register& reg) {
        for (auto& layer : m_data.at("layers")) {
        for (auto& chunk : layer.at("chunks")) {
            float w, h;
            w = chunk.at("width").get<int>();
            h = chunk.at("height").get<int>();
            int sx,sy;
            float x,y;
            sx = chunk.at("x").get<int>();
            x=sx;
            sy = chunk.at("y").get<int>();
            y=sy;
            int i=0, j=0;
            float z=0;
            for (auto& item : chunk.at("data")) {
                auto id = item.get<int>();
                if (id>0) {
                    m_tiles.emplace_back(reg, reg.construct_entity(
                        Transform{x,y,z, w,h},
                        DrawResource(1.f,0.f,0.f,1.f)
                    )
                    );
                    x+=w;
                }
                ++i;
                if (i>=w) {
                    i=0;
                    ++j;
                    x=sx;
                    y+=h;
                }
                z+=.1f;
            }
        }
        }
    }
}