#pragma once
#ifndef __TIMER_QUEUE_HPP__206fa939_f915_4e2a_89ca_61879089f478__
#define __TIMER_QUEUE_HPP__206fa939_f915_4e2a_89ca_61879089f478__

//
// include files
//

#include <cstdint>
#include <functional>

//
// forward declarations
//

namespace tempus { namespace impl { class timer_queue; } }

//
// code
//

namespace tempus {

/// Timer Queue
///
/// Allows execution of handlers at a specified time in the future.
/// Guarantees:
///   * All handlers are executed ONCE, even if canceled (abort parameter will be set to true)
///   * If timer_queue is destroyed, it will cancel all handlers.
///   * Handlers are ALWAYS executed in the timer_queue worker thread.
///   * Handlers execution order is NOT guaranteed.
class timer_queue
{
public:
    /// Initializes the timer queue.
    timer_queue();

    /// Destroys the timer queue.
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
    impl::timer_queue* impl_;   ///< Implementation pointer

}; // timer_queue

} // namespace tempus

#endif // __TIMER_QUEUE_HPP__206fa939_f915_4e2a_89ca_61879089f478__
