#pragma once

#include <thread>
#include <condition_variable>
#include <stdint.h>

class FPSMeter {
public:
    /// @param interval means interval between the fps prints.
    FPSMeter(float interval);
    ~FPSMeter();

    /// Call this function once on every frame.
    void frame() noexcept;

private:
    uint32_t frames_count = 0;
    float interval;
    std::unique_ptr<std::thread> thread;
    std::mutex mutex;
    std::condition_variable cv;
    bool working = true;

    void measure_loop();
};
