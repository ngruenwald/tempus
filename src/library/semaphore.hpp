#pragma once
#ifndef __SEMAPHORE_HPP__7bd8f761_126a_4cdc_b283_09729b1b7331__
#define __SEMAPHORE_HPP__7bd8f761_126a_4cdc_b283_09729b1b7331__

#include <mutex>
#include <condition_variable>

namespace tempus {

class semaphore
{
public:
    explicit semaphore(
        unsigned int count = 0)
        : count_(count)
    {
    }

    void notify()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        count_++;
        cv_.notify_one();
    }

    void wait()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]() { return count_ > 0; });
        count_--;
    }

    template<class Clock, class Duration>
    bool wait_until(
        const std::chrono::time_point<Clock, Duration>& point)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!cv_.wait_until(lock, point, [this]() { return count_ > 0; }))
        {
            return false;
        }
        count_--;
        return true;
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    unsigned int count_;

}; // semaphore

} // namespace tempus

#endif // __SEMAPHORE_HPP__7bd8f761_126a_4cdc_b283_09729b1b7331__
