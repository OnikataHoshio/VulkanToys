#include "TestCubeMap.h"
#include "Wins/GlfwManager.h"

void TestCubeMap::InitResource()
{
	const char* const filepaths[6] = {
		"test/TestPBR/Resource/images/cubemap/right.jpg",   // +X
		"test/TestPBR/Resource/images/cubemap/left.jpg",    // -X
		"test/TestPBR/Resource/images/cubemap/top.jpg",     // +Y
		"test/TestPBR/Resource/images/cubemap/bottom.jpg",  // -Y
		"test/TestPBR/Resource/images/cubemap/front.jpg",   // -Z
		"test/TestPBR/Resource/images/cubemap/back.jpg"     // +Z
	};
	/*right(+x) left(-x) top(+y) bottom(-y) front(-z) back(+z)*/

	cubemap.Create(filepaths, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, true);
}

void TestCubeMap::CreateSampler()
{
	sampler_id = VulkanPlus::Plus().HasSampler("trilinear-sampler") ?
		VulkanPlus::Plus().GetSampler("trilinear-sampler").first : 
		VulkanPlus::Plus().CreateSampler("trilinear-sampler", Sampler::SamplerCreateInfo()).first;
}

void TestCubeMap::CreateBuffer()
{
	uniform_buffer.Create(sizeof cubemap_uniform);

	float cubemapVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	vertex_buffer.Create(sizeof cubemapVertices)
		.TransferData(cubemapVertices, sizeof cubemapVertices);

}

void TestCubeMap::CreateRenderPass()
{
	

}

void TestCubeMap::CreateDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding bindings[2] = {
		{
			.binding = 0,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT
		},
		{
			.binding = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
		}
	};

	VkDescriptorSetLayoutCreateInfo createInfo = {
		.bindingCount = 2,
		.pBindings = bindings
	};

	descriptor_set_layout_id = VulkanPlus::Plus().CreateDescriptorSetLayout("test-cubemap-set-layout", createInfo).first;
	DescriptorSetLayout& descriptor_set_layout = VulkanPlus::Plus().GetDescriptorSetLayout(descriptor_set_layout_id).second[0];
	VulkanPlus::Plus().DescriptorPool().AllocateDescriptorSets(descriptor_set, descriptor_set_layout);
}

void TestCubeMap::CreatePipelineLayout()
{
	DescriptorSetLayout& descriptor_set_layout = VulkanPlus::Plus().GetDescriptorSetLayout(descriptor_set_layout_id).second[0];

	VkPipelineLayoutCreateInfo createInfo = {
		.setLayoutCount = 1,
		.pSetLayouts = descriptor_set_layout.Address()
	};

	pipeline_layout_id = VulkanPlus::Plus().CreatePipelineLayout("test-cubemap-pipeline-layout", createInfo).first;
}

void TestCubeMap::CreatePipeline()
{
	auto Create = [&] {
		static ShaderModule vertModule("test/TestPBR/Resource/Shaders/SPIR-V/TestCubeMap.vert.spv");
		static ShaderModule fragModule("test/TestPBR/Resource/Shaders/SPIR-V/TestCubeMap.frag.spv");
		PipelineConfigurator configurator;

		PipelineLayout& pipeline_layout = VulkanPlus::Plus().GetPipelineLayout(pipeline_layout_id).second[0];
		const RenderPass& renderpass = VulkanPlus::Plus().SwapchainRenderPassWithDepthStencil();
		configurator.PipelineLayout(pipeline_layout)
			.RenderPass(renderpass)
			.AddVertexInputBindings(0, sizeof(float) * 3, VK_VERTEX_INPUT_RATE_VERTEX)
			.AddVertexInputAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0)
			.PrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
			.AddViewport(0.0f, 0.0f,
				static_cast<float>(VulkanBase::Base().SwapchainCi().imageExtent.width),
				static_cast<float>(VulkanBase::Base().SwapchainCi().imageExtent.height),
				0.0f, 1.0f)
			.AddScissor(VkOffset2D{}, VulkanBase::Base().SwapchainCi().imageExtent)
			.CullMode(VK_CULL_MODE_NONE)
			.EnableDepthTest(VK_TRUE, VK_TRUE)
			.RasterizationSamples(VK_SAMPLE_COUNT_1_BIT)
			.AddAttachmentState(0b1111)
			.AddShaderStage(vertModule.ShaderStageCi(VK_SHADER_STAGE_VERTEX_BIT))
			.AddShaderStage(fragModule.ShaderStageCi(VK_SHADER_STAGE_FRAGMENT_BIT))
			.UpdatePipelineCreateInfo();
		pipeline_id = VulkanPlus::Plus().CreatePipeline("test-cubemap-pipeline", configurator).first;
		};

	auto Destroy = [&] {
		VulkanPlus::Plus().DestroyPipeline(pipeline_id);
		};

	VulkanBase::Base().AddCallback_CreateSwapchain(Create);
	VulkanBase::Base().AddCallback_DestroySwapchain(Destroy);

	Create();
}

void TestCubeMap::CreateFramebuffers()
{

}

void TestCubeMap::OtherOperations()
{
	Sampler& sampler = VulkanPlus::Plus().GetSampler(sampler_id).second[0];
	descriptor_set.Write(cubemap.DescriptorImageInfo(sampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
}

void TestCubeMap::UpdateDescriptorSets()
{
	cubemap_uniform.model = glm::translate(glm::mat4(1.0f), GlfwWindow::camera.Position());
	cubemap_uniform.view = GlfwWindow::camera.ViewTransform();
	cubemap_uniform.proj = GlfwWindow::camera.PerspectiveTransform();

	uniform_buffer.TransferData(&cubemap_uniform, sizeof cubemap_uniform);

	VkDescriptorBufferInfo bufferInfo = {
		.buffer = uniform_buffer,
		.offset = 0,
		.range = sizeof cubemap_uniform
	};
	
	descriptor_set.Write(bufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0);
}

void TestCubeMap::RecordCommandBuffer()
{
	uint32_t current_image_ind = VulkanBase::Base().CurrentImageIndex();

	const RenderPass& renderPass = VulkanPlus::Plus().SwapchainRenderPassWithDepthStencil();
	const CommandBuffer& commandBuffer = VulkanPlus::Plus().CommandBuffer_Graphics();
	const Framebuffer& framebuffer = VulkanPlus::Plus().CurrentSwapchainFramebufferWithDepthStencil();
	PipelineLayout& pipeline_layout = VulkanPlus::Plus().GetPipelineLayout(pipeline_layout_id).second[0];
	DescriptorSetLayout& uniform_set_layout = VulkanPlus::Plus().GetDescriptorSetLayout(descriptor_set_layout_id).second[0];
	Pipeline& pipeline = VulkanPlus::Plus().GetPipeline(pipeline_id).second[0];

	VkRect2D renderArea = { {}, VulkanBase::Base().SwapchainExtent() };
	VkClearValue clearValues[2] = {
		{.color = { 0.f, 0.f, 0.f, 1.f } },
		{.depthStencil = { 1.f, 0 } }
	};
	VkDeviceSize offset = 0;

	renderPass.Begin(commandBuffer, framebuffer, renderArea, clearValues);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout,
		0, 1, descriptor_set.Address(), 0, nullptr);
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertex_buffer.Address(), &offset);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	vkCmdDraw(commandBuffer, 36, 1, 0, 0);
	renderPass.End(commandBuffer);
}

void TestCubeMap::SendDataToNextNode()
{
}

void TestCubeMap::ImguiRender()
{
}
