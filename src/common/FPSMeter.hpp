#pragma once

#include <thread>
#include <condition_variable>
#include <stdint.h>

class FPSMeter {
public:
    /// @param interval means interval between the fps updates.
    FPSMeter(float interval);
    ~FPSMeter();

    /// Call this function once on every frame.
    void frame() noexcept;
    inline float get_fps() noexcept {
        return cur_fps;
    }

private:
    uint32_t frames_count = 0;
    float interval;
    std::unique_ptr<std::thread> thread;
    std::mutex mutex;
    std::condition_variable cv;
    bool working = true;

    float cur_fps = 0.0f;

    void measure_loop();
};
