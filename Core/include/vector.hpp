#pragma once
#include <cmath>
#include <array>

namespace SGE {
    using Dimension = unsigned;
    template<Dimension D, class TYPE>
    class Vec {
    public:
        Vec() = default;
        Vec(std::array<TYPE, D>&& arr) : m_components(std::move(arr)) {}
        template<class... Args>
        static Vec make(Args&&... args) {
            return Vec({std::forward<TYPE>(args)...});
        }
        inline TYPE& operator[](unsigned idx) { return m_components[idx]; }
        void operator+=(const Vec& v) {
            for (unsigned i=0; i<D; ++i) {
                m_components[i] += v.m_components[i];
            }
        }
    private:
        std::array<TYPE, D> m_components;
    } ;
    template<Dimension D, class TYPE>
    struct Position_vec ;

    template<class TYPE>
    struct Position_vec<2, TYPE> {
        TYPE x,y;
    } ;
    template<class TYPE>
    struct Position_vec<3, TYPE> {
        TYPE x,y,z;
    } ;
    template<class TYPE>
    struct Position_vec<4, TYPE> {
        TYPE x,y,z,w;
    } ;

    template<Dimension D, class TYPE>
    Position_vec<D, TYPE>& position_view(Vec<D, TYPE>& vec) {
        return *reinterpret_cast<Position_vec<D, TYPE>*>(&vec);
    }
}