#pragma once
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

namespace Executor {
class TaskExecutor {
public:
    TaskExecutor(uint32_t thread_number);
    ~TaskExecutor();
    void Submit(std::function<void()> task);

private:
    std::condition_variable cv_;
    std::atomic<bool> completed_;
    std::mutex mtx_;

    std::queue<std::function<void()>> task_queue_;
    std::vector<std::thread> workers_;

    void ProcessTasks();
    void Break();
};
}  // namespace Executor