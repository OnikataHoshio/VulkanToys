#define STB_IMAGE_IMPLEMENTATION
#include "Wins/GlfwManager.h"

#include "Engine/ShaderEditor/RenderGraph/DrawScreenNode.h"
#include "Engine/ShaderEditor/RenderGraph/RenderGraph.h"
#include "Engine/Panel/Editor/EditorGUIManager.h"
#include "Engine/Panel/Editor/EditorInspectorPanel.h"
#include "test/SimplePathTrace/SimplePathTrace.h"
#include "test/Test3D/Test3D.h"
#include "test/TestModel/TestModel.h"
#include "test/TestCurve/CurvePanel.h"
#include "test/PBR/PBR.h"
#include "test/PBR/TestCubeMap.h"
#include "test/PBR/PBRPrecompute.h"
#include "test/PBR/PBRRenderGraph.h"
#include "test/Tessellation/TestTessellation.h"
using namespace HoshioEngine;

int main() {
	{
		if (!GlfwWindow::InitializeWindow({1840, 1024 }))
			return -1;

		Fence& fence = VulkanPlus::Plus().CreateFences("test-fence", 1).second[0];

		Semaphore semaphore_image_available;
		Semaphore semaphore_render_over;

		VulkanPlus::Plus().CreateTexture2D("test", "res/images/icon-1024.png", VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, true);
		Texture2D& texture = VulkanPlus::Plus().GetTexture2D("test").second[0];
		Texture2D bg("res/images/kayoko-bg.png", VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, true);

		Sampler& sampler_linear = VulkanPlus::Plus().CreateSampler("sampler_linear", Sampler::SamplerCreateInfo()).second[0];

		const CommandBuffer& commandBuffer = VulkanPlus::Plus().CommandBuffer_Graphics();


		{
			std::unique_ptr<RenderNode> drawScreenNode =
				std::make_unique<DrawScreenNode>(sampler_linear, bg);
			drawScreenNode->Init();

			TestTesselation testtess;
			testtess.Init();

			//PBRRenderGraph pbrRenderGraph;
			//pbrRenderGraph.ExecutePrecompute();

			//EditorGUIManager::Instance().editorPanels.push_back(std::make_unique<CurvePanel>());

			while (!glfwWindowShouldClose(GlfwWindow::pWindow)) {
				while (glfwGetWindowAttrib(GlfwWindow::pWindow, GLFW_ICONIFIED))
					glfwWaitEvents();

				VulkanBase::Base().SwapImage(semaphore_image_available);

				auto i = VulkanBase::Base().CurrentImageIndex();

				commandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

				testtess.Render();

				commandBuffer.End();

				EditorGUIManager::Instance().Render();

				VkCommandBuffer commandBuffers[] = { commandBuffer, EditorGUIManager::Instance().GCommandBuffer() };
				VulkanBase::Base().SubmitCommandBuffer_Graphics(commandBuffers, semaphore_image_available, semaphore_render_over, fence);
				VulkanBase::Base().PresentImage(semaphore_render_over);

				glfwPollEvents();
				GlfwWindow::UpdateWindow();

				fence.Wait();
				fence.Reset();
			}

			VulkanBase::Base().WaitIdle();
		}
		VulkanBase::Base().WaitIdle();

	}
	return 0;
}






