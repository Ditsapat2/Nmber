#pragma once
#include "nlohmann/json.hpp"
#include "Core/sge.hpp"

namespace Nmber {
    using JSON = nlohmann::json;
    class Level {
    public:
        Level()=default;
        Level(std::istream&& file) ;
        ~Level() ;

        int load_level(SGE::Components::Register& reg) ;
    private:
        JSON m_data;
        std::vector<SGE::Components::EntityObject> m_tiles;
    } ;
}