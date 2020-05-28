#include "timer_queue_impl.hpp"
#include <cassert>

namespace tempus {
namespace impl {

timer_queue::timer_queue()
{
    thread_ = std::thread([this] { run(); });
}

timer_queue::~timer_queue()
{
    cancel_all();
    // Abusing the timer queue to trigger the shutdown.
    add(0, [this](bool) { finish_ = true; });
    thread_.join();
}

uint64_t timer_queue::add(
    int64_t milliseconds,
    std::function<void(bool)> handler)
{
    work_item item;
    item.end = clock::now() + std::chrono::milliseconds(milliseconds);
    item.handler = std::move(handler);

    std::unique_lock<std::mutex> lock(mutex_);
    uint64_t id = ++id_counter_;
    item.id = id;
    items_.push(std::move(item));
    lock.unlock();

    // Something changed, so wake up timer thread
    check_work_.notify();
    return id;
}

size_t timer_queue::cancel(
    uint64_t id)
{
    // Instead of removing the item from the container (thus breaking the
    // heap integrity), we set the item as having no handler, and put
    // that handler on a new item at the top for immediate execution
    // The timer thread will then ignore the original item, since it has no
    // handler.
    std::unique_lock<std::mutex> lock(mutex_);
    for (auto&& item : items_.get_container())
    {
        if (item.id == id && item.handler)
        {
            work_item new_item;
            // Zero time, so it stays at the top for immediate execution
            new_item.end = clock::time_point();
            new_item.id = 0;  // Means it is a canceled item
            // Move the handler from item to newitem.
            // Also, we need to manually set the handler to nullptr, since
            // the standard does not guarantee moving an std::function will
            // empty it. Some STL implementation will empty it, others will
            // not.
            new_item.handler = std::move(item.handler);
            item.handler = nullptr;
            items_.push(std::move(new_item));

            lock.unlock();
            // Something changed, so wake up timer thread
            check_work_.notify();
            return 1;
        }
    }
    return 0;
}

size_t timer_queue::cancel_all()
{
    // Setting all "end" to 0 (for immediate execution) is ok,
    // since it maintains the heap integrity
    std::unique_lock<std::mutex> lock(mutex_);
    for (auto&& item : items_.get_container())
    {
        if (item.id)
        {
            item.end = clock::time_point();
            item.id = 0;
        }
    }
    auto ret = items_.size();

    lock.unlock();
    check_work_.notify();
    return ret;
}

void timer_queue::run()
{
    while (!finish_)
    {
        auto end = calc_wait_time();
        if (end.first)
        {
            // Timers found,
            // so wait until it expires (or something else changes)
            check_work_.wait_until(end.second);
        }
        else
        {
            // No timers exist, so wait forever until something changes
            check_work_.wait();
        }

        // Check and execute as much work as possible,
        // such as, all expired timers
        check_work();
    }

    // If we are shutting down, we should not have any items left,
    // since the shutdown cancels all items
    assert(items_.size() == 0);
}

std::pair<bool, timer_queue::clock::time_point> timer_queue::calc_wait_time(void)
{
    std::lock_guard<std::mutex> lock(mutex_);
    while (items_.size())
    {
        if (items_.top().handler)
        {
            // Item present, so return the new wait time
            return std::make_pair(true, items_.top().end);
        }
        else
        {
            // Discard empty handlers (they were cancelled)
            items_.pop();
        }
    }

    // No items found, so return no wait time
    // (causes the thread to wait indefinitely)
    return std::make_pair(false, clock::time_point());
}

void timer_queue::check_work()
{
    std::unique_lock<std::mutex> lock(mutex_);
    while (items_.size() && items_.top().end <= clock::now())
    {
        work_item item(std::move(items_.top()));
        items_.pop();

        lock.unlock();
        if (item.handler)
        {
            item.handler(item.id == 0);
        }
        lock.lock();
    }
}

} // namespace impl
} // namespace tempus
