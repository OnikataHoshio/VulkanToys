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
#include "test/DiscretizeNurbs/DeBoor.h"
#include "test/DiscretizeNurbs/DeBoorNrubs.h"
using namespace HoshioEngine;

double TestPerformance(DeBoorNurbs& deboorNurbs, const CommandBuffer& commandBuffer);
double TestRetrieveData(DeBoorNurbs& deboorNurbs, const CommandBuffer& commandBuffer);
int main() {
	{
		if (!GlfwWindow::InitializeWindow({1840, 1024 }))
			return -1;

		Fence fence;

		Semaphore semaphore_image_available;
		Semaphore semaphore_render_over;

		Texture2D bg("res/images/kayoko-bg.png", VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, true);

		Sampler& sampler = VulkanPlus::Plus().CreateSampler("sampler_linear", Sampler::SamplerCreateInfo()).second[0];

		const CommandBuffer& commandBuffer = VulkanPlus::Plus().CommandBuffer_Graphics();

		{
			DrawScreenNode drawScreenNode(sampler, bg);
			drawScreenNode.Init();

			//PBRRenderGraph pbrRenderGraph;
			//pbrRenderGraph.ExecutePrecompute();

			DeBoorNurbs deboorNurbs;
			deboorNurbs.Init();

			double compute_cost = TestPerformance(deboorNurbs, commandBuffer);
			double transfer_cost = TestRetrieveData(deboorNurbs, commandBuffer);
			std::cout << "total time per iteration: " << (compute_cost + transfer_cost) << " ms" << std::endl;

			// EditorGUIManager::Instance().editorPanels.push_back(std::make_unique<DeBoor>());

			//while (!glfwWindowShouldClose(GlfwWindow::pWindow)) {
			//	while (glfwGetWindowAttrib(GlfwWindow::pWindow, GLFW_ICONIFIED))
			//		glfwWaitEvents();

			//	VulkanBase::Base().SwapImage(semaphore_image_available);

			//	auto i = VulkanBase::Base().CurrentImageIndex();

			//	commandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

			//	drawScreenNode.Render();

			//	commandBuffer.End();

			//	EditorGUIManager::Instance().Render();

			//	VkCommandBuffer commandBuffers[] = { commandBuffer, EditorGUIManager::Instance().GCommandBuffer() };
			//	VulkanBase::Base().SubmitCommandBuffer_Graphics(commandBuffers, semaphore_image_available, semaphore_render_over, fence);
			//	VulkanBase::Base().PresentImage(semaphore_render_over);

			//	glfwPollEvents();
			//	GlfwWindow::UpdateWindow();

			//	fence.Wait();
			//	fence.Reset();
			//}


			VulkanBase::Base().WaitIdle();
		}
		VulkanBase::Base().WaitIdle();

	}
	return 0;
}


double TestPerformance(DeBoorNurbs& deboorNurbs,const CommandBuffer& commandBuffer)
{
	Fence fence;

	const int iterations = 1000; // 测试次数
	auto start = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < iterations; i++) {
		commandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		deboorNurbs.Render();
		commandBuffer.End();

		VkCommandBuffer commandBuffers[] = { commandBuffer };
		VulkanBase::Base().SubmitCommandBuffer_Compute(commandBuffers, fence);

		fence.Wait();
		fence.Reset();
	}

	auto end = std::chrono::high_resolution_clock::now();
	double elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();
	std::cout << "ComputeShader average time per iteration: " << (elapsed_ms / iterations) << " ms" << std::endl;
	return elapsed_ms / iterations;

}

double TestRetrieveData(DeBoorNurbs& deboorNurbs, const CommandBuffer& commandBuffer)
{

	const int iterations = 1000; // 测试次数
	auto start = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < iterations; i++) {
		deboorNurbs.RetrieveData();
	}

	auto end = std::chrono::high_resolution_clock::now();
	double elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();
	std::cout << "Retridata average time per iteration: " << (elapsed_ms / iterations) << " ms" << std::endl;
	return elapsed_ms/iterations;
}




