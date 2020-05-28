#pragma once

#include <chrono>
#include <functional>
#include <thread>
#include <queue>

#include "semaphore.hpp"

namespace tempus {
namespace impl {

class timer_queue
{
public:
    timer_queue();

    ~timer_queue();

    /// Adds a new timer
    /// @returns The ID of the new timer. You can use this ID to cancel the timer.
    uint64_t add(
        int64_t milliseconds,
        std::function<void(bool)> handler);

    /// Cancels the specified timer
    /// @returns
    ///    1 if the timer was cancelled.
    ///    0 if you were too late to cancel (or the timer ID was never valid to start with)
    size_t cancel(
        uint64_t id);

    /// Cancels all timers
    /// @returns The number of timers cancelled.
    size_t cancel_all();

private:
    using clock = std::chrono::steady_clock;
    timer_queue(const timer_queue&) = delete;
    timer_queue& operator=(const timer_queue&) = delete;

    void run();

    std::pair<bool, clock::time_point> calc_wait_time(void);

    void check_work();

private:
    semaphore check_work_;
    std::thread thread_;
    bool finish_ = false;
    uint64_t id_counter_ = 0;

    struct work_item
    {
        clock::time_point end;
        uint64_t id;  // id==0 means it was cancelled
        std::function<void(bool)> handler;

        bool operator>(
            const work_item& other) const
        {
            return end > other.end;
        }
    };

    std::mutex mutex_;

    // Inheriting from priority_queue, so we can access the internal container
    class queue
        : public std::priority_queue<
                    work_item,
                    std::vector<work_item>,
                    std::greater<work_item>>
    {
    public:
        std::vector<work_item>& get_container()
        {
            return this->c;
        }

    } items_;

}; // timer_queue

} // namespace impl
} // namespace tempus
