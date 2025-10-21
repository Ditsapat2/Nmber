#pragma once
#include "config.h"

namespace SGE {
    class Time {
    public:
        using Seconds = double;
        Time() {
            current = glfwGetTime();
            last = current;
            elapsed=0;
        }
        static Time& get() {
            return m_instance;
        }
        void update() {
            current=glfwGetTime();
            elapsed=current-last;
            last = current;
        }
        void restart() {
            glfwSetTime(0);
            current=last=elapsed=0;
        }
        Seconds get_elapsedTime() const { return elapsed; }
    private:
        static Time m_instance;
        Seconds last, current, elapsed;
    } ;
    inline Time Time::m_instance;
}