#pragma once
#include "matrix.hpp"

namespace SGE {
    template<class TYPE=float>
    Matrix<4,4, TYPE> ortho_mat(float right, float left, float top, float bottom, float far, float near) {
        return Matrix<4,4, TYPE>(typename Matrix<4,4,TYPE>::MAT_LINEAR{
            2/(right-left), 0, 0, -(right+left)/(right-left),
            0, 2/(top-bottom), 0, -(top+bottom)/(top-bottom),
            0, 0, -2/(far-near), -(far+near)/(far-near),
            0, 0, 0, 1
        });
    }
    template<unsigned N=4, class TYPE=float>
    constexpr Matrix<N, N, TYPE> identity_mat() {
        Matrix<N, N, TYPE> m(TYPE(0));
        for (unsigned i=0; i<N; ++i) {
            m.get(i,i) = TYPE(1);
        }
        return m;
    }
}