#include "FPSMeter.hpp"

FPSMeter::FPSMeter(float interval) : interval(interval) {
    thread = std::make_unique<std::thread>(&FPSMeter::measure_loop, this);
}

FPSMeter::~FPSMeter() {
    working = false;
    cv.notify_all();
    thread->detach();
}

void FPSMeter::frame() noexcept {
    frames_count++;
}

void FPSMeter::measure_loop() {
    while (working) {
        cur_fps = static_cast<float>(frames_count) / interval;

        frames_count = 0;
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait_for(lock, std::chrono::duration<float>(interval));
    }
}
