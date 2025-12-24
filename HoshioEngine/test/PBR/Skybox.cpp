#include "Skybox.h"
#include "Wins/GlfwManager.h"

void HoshioEngine::Skybox::InitResource()
{
}

void HoshioEngine::Skybox::CreateSampler()
{
	sampler_id = VulkanPlus::Plus().HasSampler("trilinear-sampler") ?
		VulkanPlus::Plus().GetSampler("trilinear-sampler").first :
		VulkanPlus::Plus().CreateSampler("trilinear-sampler", Sampler::SamplerCreateInfo()).first;
}

void HoshioEngine::Skybox::CreateBuffer()
{
	uniform_buffer.Create(sizeof skybox_uniform);

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

void HoshioEngine::Skybox::CreateRenderPass()
{
	VkAttachmentDescription attachmentDescriptions[2] = {
		{
			.format = VulkanBase::Base().SwapchainCi().imageFormat,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		},
		{
			.format = VK_FORMAT_D24_UNORM_S8_UINT,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE,
			.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		}
	};

	VkAttachmentReference attachmentReferences[2] = {
		{
			.attachment = 0,
			.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		},
		{
			.attachment = 1,
			.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		}
	};

	VkSubpassDescription subpassDescription = {
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments = attachmentReferences,
		.pDepthStencilAttachment = attachmentReferences + 1
	};

	VkSubpassDependency subpassDependency = {
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
	};

	VkRenderPassCreateInfo renderPassCreateInfo = {
		.attachmentCount = 2,
		.pAttachments = attachmentDescriptions,
		.subpassCount = 1,
		.pSubpasses = &subpassDescription,
		.dependencyCount = 1,
		.pDependencies = &subpassDependency
	};

	renderPass.Create(renderPassCreateInfo);
}

void HoshioEngine::Skybox::CreateDescriptorSetLayout()
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

	descriptorSetLayout.Create(createInfo);
	VulkanPlus::Plus().DescriptorPool().AllocateDescriptorSets(descriptorSet, descriptorSetLayout);
}

void HoshioEngine::Skybox::CreatePipelineLayout()
{
	VkPipelineLayoutCreateInfo createInfo = {
		.setLayoutCount = 1,
		.pSetLayouts = descriptorSetLayout.Address()
	};

	pipelineLayout.Create(createInfo);
}

void HoshioEngine::Skybox::CreatePipeline()
{
	auto Create = [&] {
		static ShaderModule vertModule("test/PBR/Resource/Shaders/SPIR-V/TestCubeMap.vert.spv");
		static ShaderModule fragModule("test/PBR/Resource/Shaders/SPIR-V/TestCubeMap.frag.spv");
		PipelineConfigurator configurator;

		configurator.PipelineLayout(pipelineLayout)
			.RenderPass(renderPass)
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
		pipeline.Create( configurator);
		};

	auto Destroy = [&] {
		pipeline.~Pipeline();
		};

	VulkanBase::Base().AddCallback_CreateSwapchain(Create);
	VulkanBase::Base().AddCallback_DestroySwapchain(Destroy);

	Create();
}

void HoshioEngine::Skybox::CreateFramebuffers()
{

}

void HoshioEngine::Skybox::OtherOperations()
{

}

void HoshioEngine::Skybox::UpdateDescriptorSets()
{
	skybox_uniform.model = glm::translate(glm::mat4(1.0f), GlfwWindow::camera.Position());
	//cubemap_uniform.model = glm::mat4(1.0f);
	skybox_uniform.view = GlfwWindow::camera.ViewTransform();
	skybox_uniform.proj = GlfwWindow::camera.PerspectiveTransform();

	uniform_buffer.TransferData(&skybox_uniform, sizeof skybox_uniform);

	VkDescriptorBufferInfo bufferInfo = {
		.buffer = uniform_buffer,
		.offset = 0,
		.range = sizeof skybox_uniform
	};

	descriptorSet.Write(bufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0);
}

void HoshioEngine::Skybox::RecordCommandBuffer()
{
	const CommandBuffer& commandBuffer = VulkanPlus::Plus().CommandBuffer_Graphics();
	const Framebuffer& framebuffer = VulkanPlus::Plus().CurrentSwapchainFramebufferWithDepthStencil();

	VkRect2D renderArea = { {}, VulkanBase::Base().SwapchainExtent() };
	VkClearValue clearValues[2] = {
		{.color = { 0.f, 0.f, 0.f, 1.f } },
		{.depthStencil = { 1.f, 0 } }
	};
	VkDeviceSize offset = 0;

	renderPass.Begin(commandBuffer, framebuffer, renderArea, clearValues);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
		0, 1, descriptorSet.Address(), 0, nullptr);
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertex_buffer.Address(), &offset);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	vkCmdDraw(commandBuffer, 36, 1, 0, 0);
	renderPass.End(commandBuffer);
}

void HoshioEngine::Skybox::SendDataToNextNode()
{
}

void HoshioEngine::Skybox::ImguiRender()
{
}
