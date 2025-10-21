#pragma once
#include "vector.hpp"
#include "config.h"

namespace SGE {
    template<Dimension ROW, Dimension COL, class TYPE>
    class Matrix {
    public:
        using MAT = std::array<std::array<TYPE, ROW>, COL>;
        using MAT_LINEAR = std::array<TYPE, ROW*COL>;
        Matrix() = default;
        Matrix(MAT&& mat) : m_elements(std::move(mat)) {}
        Matrix(MAT_LINEAR&& mat) { // ROW MAJOR
            for (Dimension i=0; i<ROW; ++i) {
                for (Dimension j=0; j<COL; ++j) {
                    get(i,j)=mat[i*COL+j];
                }
            }
        }
        Matrix(TYPE&& data) {
            for (auto& row : m_elements) {
                for (auto& item : row) {
                    item = std::forward<TYPE>(data);
                }
            }
        }
        inline TYPE& get(unsigned row, unsigned col) { return m_elements[col][row]; } //ROW MAJOR
        TYPE* data() { return &m_elements[0][0]; } 
        const TYPE* data() const { return &m_elements[0][0]; } 
    private:
        MAT m_elements; //COLUMN MAJOR
    } ;
    using Mat4f = Matrix<4,4,float>;
    template<class... Args, Dimension R1, Dimension D1, class T1, Dimension R2, Dimension D2, class T2>
    auto matrix_multiply(Args&&... args, Matrix<R1, D1, T1>&& m1, Matrix<R2, D2, T2>&& m2) {
        if constexpr (sizeof...(Args)>0) {
            return matrix_multiply(std::forward<Args>(args)...);
        }
        return matrix_multiply2(std::forward<Matrix<R1,D1,T1>>(m1), std::forward<Matrix<R2,D2,T2>>(m2));
    }
    template<Dimension R1, Dimension D1, class T1, Dimension R2, Dimension D2, class T2>
    Matrix<D1,R2,T1> matrix_multiply2( Matrix<R1, D1, T1>&& m1, Matrix<R2, D2, T2>&& m2) ;

}