#pragma once

#if SDC_VULKAN
#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include "Rendering/RenderTypes.hpp"
#include "VkPipeline.hpp"

namespace GI::detail{
    class Thread
    {
    private:
        bool destroying = false;
        std::thread worker;
        std::queue<std::function<void()>> jobQueue;
        std::mutex queueMutex;
        std::condition_variable condition;

        // Loop through all remaining jobs
        void queueLoop()
        {
            while (true)
            {
                std::function<void()> job;
                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    condition.wait(lock, [this] { return !jobQueue.empty() || destroying; });
                    if (destroying)
                    {
                        break;
                    }
                    job = jobQueue.front();
                }

                job();

                {
                    std::lock_guard<std::mutex> lock(queueMutex);
                    jobQueue.pop();
                    condition.notify_one();
                }
            }
        }

    public:
        Thread()
        {
            worker = std::thread(&Thread::queueLoop, this);
        }

        ~Thread()
        {
            if (worker.joinable())
            {
                wait();
                queueMutex.lock();
                destroying = true;
                condition.notify_one();
                queueMutex.unlock();
                worker.join();
            }
        }

        // Add a new job to the thread's queue
        void addJob(std::function<void()> function)
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            jobQueue.push(std::move(function));
            condition.notify_one();
        }

        // Wait until all work items have been finished
        void wait()
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this]() { return jobQueue.empty(); });
        }
    };

    class VKThreadPool {
    public:
        inline static auto get() -> VKThreadPool& {
            static VKThreadPool vkc;
            return vkc;
        }

        void init();
        void deinit();

        void beginCommandBuffers();
        void endCommandBuffers();

        VkCommandBuffer primaryCommandBuffer;
    private:
        VKThreadPool();
        ~VKThreadPool();

        void wait()
        {
            for (auto &thread : threads)
            {
                thread->wait();
            }
        }

        std::vector<std::unique_ptr<Thread>> threads;
        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<VkCommandPool> commandPools;
        std::vector<UniformBufferObject> ubos;

        VkCommandPool primaryCommandPool;
    };
}
#endif