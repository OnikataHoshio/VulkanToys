#include "TestTessellation.h"

void TestTesselation::InitResource()
{
}

void TestTesselation::CreateSampler()
{
	sampler.Create(Sampler::SamplerCreateInfo());
}

void TestTesselation::CreateBuffer()
{
	vertex_buffer.Create(sizeof tessVertices)
		.TransferData(tessVertices, sizeof tessVertices);
}

void TestTesselation::CreateRenderPass()
{
	
}

void TestTesselation::CreateDescriptorSetLayout()
{
}

void TestTesselation::CreatePipelineLayout()
{
	VkPipelineLayoutCreateInfo createInfo = {};
	pipelineLayout.Create(createInfo);
}

void TestTesselation::CreatePipeline()
{
	auto Create = [&] {
		static ShaderModule vertModule("test/Tessellation/Resource/Shaders/SPIR-V/test-tesse/test-tessellation.vert.spv");
		static ShaderModule tescModule("test/Tessellation/Resource/Shaders/SPIR-V/test-tesse/test-tessellation.tesc.spv");
		static ShaderModule teseModule("test/Tessellation/Resource/Shaders/SPIR-V/test-tesse/test-tessellation.tese.spv");
		static ShaderModule fragModule("test/Tessellation/Resource/Shaders/SPIR-V/test-tesse/test-tessellation.frag.spv");

		PipelineConfigurator configurator;
		const RenderPass& renderpass = VulkanPlus::Plus().SwapchainRenderPass();
		configurator.PipelineLayout(pipelineLayout)
			.RenderPass(renderpass)
			.AddVertexInputBindings(0, sizeof(float) * 2, VK_VERTEX_INPUT_RATE_VERTEX)
			.AddVertexInputAttribute(0, 0, VK_FORMAT_R32G32_SFLOAT, 0)
			.PrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_PATCH_LIST)
			.TessPatchControlPoints(3) // 一个三角细分
			.AddViewport(0.0f, 0.0f,
				static_cast<float>(VulkanBase::Base().SwapchainCi().imageExtent.width),
				static_cast<float>(VulkanBase::Base().SwapchainCi().imageExtent.height),
				0.0f, 1.0f)
			.AddScissor(VkOffset2D{}, VulkanBase::Base().SwapchainCi().imageExtent)
			.CullMode(VK_CULL_MODE_NONE)
			.EnableDepthTest(VK_FALSE, VK_FALSE)
			.RasterizationSamples(VK_SAMPLE_COUNT_1_BIT)
			.AddAttachmentState(0b1111)
			.PolygonMode(VK_POLYGON_MODE_LINE)
			.LineWidth(1.0f)
			.AddShaderStage(vertModule.ShaderStageCi(VK_SHADER_STAGE_VERTEX_BIT))
			.AddShaderStage(tescModule.ShaderStageCi(VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT))
			.AddShaderStage(teseModule.ShaderStageCi(VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT))
			.AddShaderStage(fragModule.ShaderStageCi(VK_SHADER_STAGE_FRAGMENT_BIT))
			.UpdatePipelineCreateInfo();
		pipeline.Create(configurator);
		};

	auto Destroy = [&] {
		pipeline.~Pipeline();
		};

	VulkanBase::Base().AddCallback_CreateSwapchain(Create);
	VulkanBase::Base().AddCallback_DestroySwapchain(Destroy);

	Create();
}

void TestTesselation::CreateFramebuffers()
{
}

void TestTesselation::OtherOperations()
{
}

void TestTesselation::UpdateDescriptorSets()
{
}

void TestTesselation::RecordCommandBuffer()
{
	const RenderPass& renderPass = VulkanPlus::Plus().SwapchainRenderPass();
	const CommandBuffer& commandBuffer = VulkanPlus::Plus().CommandBuffer_Graphics();
	const Framebuffer& framebuffer = VulkanPlus::Plus().CurrentSwapchainFramebuffer();

	VkRect2D renderArea = { {}, VulkanBase::Base().SwapchainExtent() };
	VkClearValue clearValue = { .color = { 0.2f, 0.2f, 0.2f, 1.f } };
	VkDeviceSize offset = 0;

	renderPass.Begin(commandBuffer, framebuffer, renderArea, clearValue);
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertex_buffer.Address(), &offset);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	vkCmdDraw(commandBuffer, 6, 1, 0, 0);
	renderPass.End(commandBuffer);

}

void TestTesselation::SendDataToNextNode()
{
}

void TestTesselation::ImguiRender()
{
}
