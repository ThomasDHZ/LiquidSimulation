#include <platform.h>
#include "SystemClock.h"
#include "FrameTimer.h"
#include <ImGuiSystem.h>
#include <VulkanSystem.h>
#include "VulkanWindow.h"
#include <DebugSystem.h>
#include <ShaderSystem.h>
#include <EngineConfigSystem.h>
#include <LevelSystem.h>
#include <from_json.h>
#include "FluidSimulation.h"
#include "CudaSystem.h"
#include "Liquid.cuh"

FluidSimulation fluidSimulation = FluidSimulation();

void UpdateImGui();
void LoadScene();
void Update(float deltaTime);
void Draw(float deltaTime);
void Destroy();

int main()
{
    renderSystem.UsingImGui = true;
    SystemClock systemClock = SystemClock();
    FrameTimer deltaTime = FrameTimer();

    std::cout << "Base Directory: " << std::filesystem::current_path() << std::endl;
    debugSystem.SetRootDirectory("../Assets");
    try
    {
        vulkan.VulkanSetUp(configSystem.WindowResolution, configSystem.RenderResolution);
        bufferSystem.SetUpVmaAllocation();
        if (renderSystem.UsingImGui)  imGuiSystem.StartUp();
        memoryPoolSystem.StartUp();
        shaderSystem.CompileShaders(configSystem.ShaderSourceDirectory.c_str(), configSystem.CompiledShaderOutputDirectory.c_str());
        levelSystem.PresentingAttachmentTextureId = VkGuid("de6ca646-874f-4986-9daf-912dbc3aa7d0");
        LoadScene();
        while (!vulkanWindow.ShouldClose())
        {
            const float frameTime = deltaTime.GetFrameTime();
            vulkanWindow.PollEvents();
            Update(frameTime);
            UpdateImGui();
            Draw(frameTime);
            deltaTime.EndFrameTime();
        }
        Destroy();
    }
    catch (const VulkanError& e)
    {
        fprintf(stderr, "%s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        fprintf(stderr, "STD EXCEPTION: %s\n", e.what());
        return -1;
    }
    return 0;
}

void UpdateImGui()
{
    imGuiSystem.StartFrame();
    imGuiSystem.FpsDisplay();
    imGuiSystem.EndFrame();
}

void LoadScene()
{
    RenderPassLoader renderPassLoader = fileSystem.LoadJsonFile("RenderPass/HdrRenderPass.json").get<RenderPassLoader>();
    levelSystem.LoadRenderPass(renderPassLoader);
    fluidSimulation.StartUp(64 * 64);
}

void Update(float deltaTime)
{
    fluidSimulation.Update(deltaTime);
}

void Draw(float deltaTime)
{
    VkCommandBuffer commandBuffer = vulkan.Swapchain().StartFrame();
    if (commandBuffer == VK_NULL_HANDLE)
    {
        renderSystem.RecreateSwapchain(vulkanWindow.GetWindowHandle(), deltaTime);
        return;
    }
    Vector<RenderPassNode> renderNodes = levelSystem.CreateDrawCommands(commandBuffer, deltaTime);
    renderSystem.Draw(commandBuffer, renderNodes);
    renderSystem.PresentToSwapChain(commandBuffer, levelSystem.PresentingAttachmentTextureId);
    if (renderSystem.UsingImGui) imGuiSystem.Draw(commandBuffer);
    vulkan.Swapchain().EndFrame(commandBuffer);
}

void Destroy()
{
    vkDeviceWaitIdle(vulkan.LogicalDevice());

    if (renderSystem.UsingImGui) imGuiSystem.Destroy();
    renderSystem.Destroy();
    textureSystem.Destroy();
    meshSystem.Destroy();
    materialSystem.Destroy();
    // bufferSystem.DestroyAllBuffers();
    memorySystem.ReportLeaks();
    cudaSystem.ReportLeaks();

    vulkan.Destroy();
    vulkanWindow.Close();
}
