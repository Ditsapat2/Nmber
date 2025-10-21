#pragma once
#include "include/buffer.hpp"
#include "include/shader.hpp"
#include "include/buffer_layout.hpp"
#include "include/vector.hpp"
#include "include/matrix.hpp"
#include "include/matrix_transform.hpp"
#include "include/texture.hpp"
#include "include/batch.hpp"

#include "include/entity.hpp"

#include "include/time.hpp"

namespace SGE {
    inline int init() {
        return glfwInit();
    }
}