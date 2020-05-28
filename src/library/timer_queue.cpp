#include <tempus/timer_queue.hpp>
#include <cassert>

#include "timer_queue_impl.hpp"

namespace tempus {

timer_queue::timer_queue()
    : impl_(new impl::timer_queue())
{
}

timer_queue::~timer_queue()
{
    delete impl_;
}

uint64_t timer_queue::add(
    int64_t milliseconds,
    std::function<void(bool)> handler)
{
    return impl_->add(milliseconds, handler);
}

size_t timer_queue::cancel(
    uint64_t id)
{
    return impl_->cancel(id);
}

size_t timer_queue::cancel_all()
{
    return impl_->cancel_all();
}

} // namespace tempus
