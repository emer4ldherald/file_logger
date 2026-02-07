#include "task_executor.h"

namespace Executor {
void TaskExecutor::ProcessTasks() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock lock(mtx_);
            cv_.wait(lock, [this]() {
                return completed_.load() || !task_queue_.empty();
            });

            if (completed_.load() && task_queue_.empty()) {
                break;
            }

            task = std::move(task_queue_.front());
            task_queue_.pop();
        }
        task();
    }
}

void TaskExecutor::Break() {
    completed_.store(true);
    cv_.notify_all();

    for (std::thread& thread : workers_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

TaskExecutor::TaskExecutor(uint32_t thread_number) : completed_(false) {
    for (uint32_t i = 0; i != thread_number; ++i) {
        workers_.emplace_back([this]() { ProcessTasks(); });
    }
}

TaskExecutor::~TaskExecutor() { Break(); }

void TaskExecutor::Submit(std::function<void()> task) {
    {
        std::lock_guard lock(mtx_);
        task_queue_.push(std::move(task));
    }

    cv_.notify_one();
}
}  // namespace Executor