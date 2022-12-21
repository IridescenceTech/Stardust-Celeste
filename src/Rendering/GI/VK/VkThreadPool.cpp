#include "Rendering/GI/VK/VkThreadPool.hpp"
#include "Rendering/GI/VK/VkUtil.hpp"

#if SDC_VULKAN
namespace GI::detail {
    void createCommandBuffer(VkCommandPool cmdPool, VkCommandBuffer& cmdBuf) {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = cmdPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(VKContext::get().logicalDevice, &allocInfo, &cmdBuf) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers!");
        }
    }

    void createCommandPool(VkCommandPool& cmdPool) {
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(VKContext::get().physicalDevice);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(VKContext::get().logicalDevice, &poolInfo, nullptr, &cmdPool) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create secondary command pool!");
        }
    }

    void VKThreadPool::init() {
        auto numThreads = std::thread::hardware_concurrency();
        SC_ASSERT(numThreads > 0, "Not enough threads for concurrency!");
        SC_APP_INFO("Creating Worker Pool with {} threads!", numThreads);

        threads.clear();
        for (auto i = 0; i < numThreads; i++)
        {
            threads.push_back(std::make_unique<Thread>());
        }

        commandPools.resize(numThreads);
        commandBuffers.resize(numThreads);
        ubos.resize(numThreads);

        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(VKContext::get().physicalDevice);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(VKContext::get().logicalDevice, &poolInfo, nullptr, &primaryCommandPool) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create primary command pool!");
        }

        createCommandBuffer(primaryCommandPool, primaryCommandBuffer);

        for(size_t i = 0; i < numThreads; i++) {
            createCommandPool(commandPools[i]);
            createCommandBuffer(commandPools[i], commandBuffers[i]);

            ubos[i].model = glm::mat4(1.0f);
            ubos[i].projview = glm::mat4(1.0f);
        }
    }

    void VKThreadPool::deinit() {
        vkDestroyCommandPool(VKContext::get().logicalDevice, primaryCommandPool, nullptr);
    }

    void VKThreadPool::beginCommandBuffers() {
        vkResetCommandBuffer(primaryCommandBuffer, 0);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(primaryCommandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = VKPipeline::get().renderPass;
        renderPassInfo.framebuffer = VKPipeline::get().swapChainFramebuffers[VKPipeline::get().imageIndex];

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = VKContext::get().swapChainExtent;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{VKPipeline::get().clearColor.r, VKPipeline::get().clearColor.g, VKPipeline::get().clearColor.b, VKPipeline::get().clearColor.a}};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(primaryCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Inheritance info for the secondary command buffers
        VkCommandBufferInheritanceInfo inheritanceInfo = {};
        inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
        inheritanceInfo.renderPass = VKPipeline::get().renderPass;
        // Secondary command buffer also use the currently active framebuffer
        inheritanceInfo.framebuffer = VKPipeline::get().swapChainFramebuffers[VKPipeline::get().imageIndex];

        //Start all threads buffers
        for(size_t i = 0; i < threads.size(); i++) {
            //Initial Job
            threads[i]->addJob([=] {
                VkCommandBufferBeginInfo commandBufferBeginInfo;
                commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
                commandBufferBeginInfo.pInheritanceInfo = &inheritanceInfo;

                VkCommandBuffer cmdBuffer = commandBuffers[i];
                if (vkBeginCommandBuffer(cmdBuffer, &beginInfo) != VK_SUCCESS) {
                    throw std::runtime_error("Failed to begin recording command buffer!");
                }

                vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VKPipeline::get().graphicsPipeline);

                VkViewport viewport{};
                viewport.x = 0.0f;
                viewport.y = 0.0f;
                viewport.width = static_cast<float>(VKContext::get().swapChainExtent.width);
                viewport.height = static_cast<float>(VKContext::get().swapChainExtent.height);
                viewport.minDepth = 0.0f;
                viewport.maxDepth = 1.0f;
                vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

                VkRect2D scissor{};
                scissor.offset = {0, 0};
                scissor.extent = VKContext::get().swapChainExtent;
                vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

                VkColorBlendEquationEXT blendEquationExt;
                blendEquationExt.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
                blendEquationExt.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                blendEquationExt.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                blendEquationExt.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
                blendEquationExt.colorBlendOp = VK_BLEND_OP_ADD;
                blendEquationExt.alphaBlendOp = VK_BLEND_OP_ADD;

                auto fn = reinterpret_cast<PFN_vkCmdSetColorBlendEquationEXT>(vkGetDeviceProcAddr(detail::VKContext::get().logicalDevice, "vkCmdSetColorBlendEquationEXT"));
                fn(cmdBuffer, 0, 1, &blendEquationExt);
            });
        }

    }

    void VKThreadPool::endCommandBuffers() {
        for(size_t i = 0; i < threads.size(); i++){
            threads[i]->addJob([=] {
                if(vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
                    throw std::runtime_error("Failed to record command buffer thread!");
            });
        }

        wait();

        vkCmdExecuteCommands(primaryCommandBuffer, commandBuffers.size(), commandBuffers.data());
        vkCmdEndRenderPass(primaryCommandBuffer);
        if (vkEndCommandBuffer(primaryCommandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to record command buffer!");
        }

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {VKPipeline::get().imageAvailableSemaphores};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &primaryCommandBuffer;

        VkSemaphore signalSemaphores[] = {VKPipeline::get().renderFinishedSemaphores};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(VKContext::get().graphicsQueue, 1, &submitInfo, VKPipeline::get().inFlightFence) != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {VKContext::get().swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &VKPipeline::get().imageIndex;
        presentInfo.pResults = nullptr; // Optional
        vkQueuePresentKHR(VKContext::get().presentQueue, &presentInfo);
    }

    VKThreadPool::VKThreadPool() {

    }

    VKThreadPool::~VKThreadPool() {

    }

}
#endif