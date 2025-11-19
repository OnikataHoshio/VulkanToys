#include "TestPBR.h"
#include "Wins/GlfwManager.h"

void TestPBR::InitResource()
{
	CreateSphere(32, 32);
}

void TestPBR::CreateSampler()
{
	shader_info.sampler_id = VulkanPlus::Plus().CreateSampler("trilinear-sampler", Sampler::SamplerCreateInfo()).first;
}

void TestPBR::CreateBuffer()
{
	vertex_uniform_buffer.Create(sizeof vertex_uniform);
	
	light_uniform_buffer.Create(sizeof light_uniform);


	for (uint32_t i = 0; i < 5; i++) 
	{
		for (uint32_t j = 0; j < 5; j++) 
		{
			sphere_uniform[i][j].roughness = i  * 0.2 + 0.1;
			sphere_uniform[i][j].metallic = j  * 0.2 + 0.1;
		}
	}

	VkDeviceSize uniformAlignment = VulkanBase::Base().PhysicalDeviceProperties().limits.minUniformBufferOffsetAlignment;
	uniformAlignment = (uniformAlignment + sizeof(SphereUniform) - 1) & ~(uniformAlignment - 1);

	sphere_uniform_buffer.Create(uniformAlignment * 25)
		.TransferData(sphere_uniform, 25, sizeof(SphereUniform), sizeof(SphereUniform), uniformAlignment);
}

void TestPBR::CreateRenderPass()
{

}

void TestPBR::CreateDescriptorSetLayout()
{
	//create sampler set layout
	{
		VkDescriptorSetLayoutBinding bindings[4] = {
			//DIFFUSE
			{
				.binding = 0,
				.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.descriptorCount = 4,
				.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
			},
			//SPECULAR
			{
				.binding = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.descriptorCount = 4,
				.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
			},
			//NORMAL
			{
				.binding = 2,
				.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.descriptorCount = 2,
				.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
			},
			//OTHER
			{
				.binding = 3,
				.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.descriptorCount = 4,
				.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
			}
		};

		VkDescriptorBindingFlags flags[4]{};
		flags[0] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
		flags[1] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
		flags[2] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
		flags[3] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;

		VkDescriptorSetLayoutBindingFlagsCreateInfo flags_ci{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
			.bindingCount = 4,
			.pBindingFlags = flags
		};


		VkDescriptorSetLayoutCreateInfo createInfo = {
			.pNext = &flags_ci,
			.bindingCount = 4,
			.pBindings = bindings
		};
		shader_info.sampler_set_layout_id = VulkanPlus::Plus().CreateDescriptorSetLayout("test-pbr-sampler-set-layout", createInfo).first;
	}

	{
		VkDescriptorSetLayoutBinding bindings[3] = {
			{
				.binding = 0,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_VERTEX_BIT
			},
			{
				.binding = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
			},
			{
				.binding = 2,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
			}
		};
		VkDescriptorSetLayoutCreateInfo createInfo = {
			.bindingCount = 3,
			.pBindings = bindings
		};
		uniform_set_layout_id = VulkanPlus::Plus().CreateDescriptorSetLayout("test-pbr-uniform-set-layot", createInfo).first;
		DescriptorSetLayout& uniform_set_layout = VulkanPlus::Plus().GetDescriptorSetLayout(uniform_set_layout_id).second[0];

		VulkanPlus::Plus().DescriptorPool().AllocateDescriptorSets(uniform_set, uniform_set_layout);
	}
}

void TestPBR::CreatePipelineLayout()
{
	DescriptorSetLayout& sampler_set_layout = VulkanPlus::Plus().GetDescriptorSetLayout(shader_info.sampler_set_layout_id).second[0];
	DescriptorSetLayout& uniform_set_layout = VulkanPlus::Plus().GetDescriptorSetLayout(uniform_set_layout_id).second[0];
	VkDescriptorSetLayout layouts[] = { sampler_set_layout, uniform_set_layout };

	VkPushConstantRange pushConstantRange = {
		.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
		.offset = 0,
		.size = sizeof(glm::vec3)
	};

	VkPipelineLayoutCreateInfo createInfo = {
		.setLayoutCount = 2,
		.pSetLayouts = layouts,
		.pushConstantRangeCount = 1,
		.pPushConstantRanges = &pushConstantRange
	};

	shader_info.pipeline_layout_id = VulkanPlus::Plus().CreatePipelineLayout("test-pbr-pipeline-layout", createInfo).first;
}

void TestPBR::CreatePipeline()
{
	auto Create = [&] {
		static ShaderModule vertModule("test/TestPBR/Resource/Shaders/SPIR-V/PBR.vert.spv");
		static ShaderModule fragModule("test/TestPBR/Resource/Shaders/SPIR-V/PBR.frag.spv");
		PipelineConfigurator configurator;
		std::vector<VertexInputAttribute> vertex_input_attributes = sphere.GetVertexInputeAttributes();
		uint32_t stride = sphere.GetVertexInputAttributesStride();
		configurator.AddVertexInputBindings(0, stride, VK_VERTEX_INPUT_RATE_VERTEX);
		for (auto& attribute : vertex_input_attributes) {
			configurator.AddVertexInputAttribute(
				attribute.location, attribute.binding, attribute.format, attribute.offset);
		}

		PipelineLayout& pipeline_layout = VulkanPlus::Plus().GetPipelineLayout(shader_info.pipeline_layout_id).second[0];
		const RenderPass& renderpass = VulkanPlus::Plus().SwapchainRenderPassWithDepthStencil();
		configurator.PipelineLayout(pipeline_layout)
			.RenderPass(renderpass)
			.PrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
			.AddViewport(0.0f, 0.0f,
				static_cast<float>(VulkanBase::Base().SwapchainCi().imageExtent.width),
				static_cast<float>(VulkanBase::Base().SwapchainCi().imageExtent.height),
				0.0f, 1.0f)
			.AddScissor(VkOffset2D{}, VulkanBase::Base().SwapchainCi().imageExtent)
			.CullMode(VK_CULL_MODE_BACK_BIT)
			.FrontFace(VK_FRONT_FACE_COUNTER_CLOCKWISE)
			.EnableDepthTest(VK_TRUE, VK_TRUE)
			.RasterizationSamples(VK_SAMPLE_COUNT_1_BIT)
			.AddAttachmentState(0b1111)
			.AddShaderStage(vertModule.ShaderStageCi(VK_SHADER_STAGE_VERTEX_BIT))
			.AddShaderStage(fragModule.ShaderStageCi(VK_SHADER_STAGE_FRAGMENT_BIT))
			.UpdatePipelineCreateInfo();
		shader_info.pipeline_id = VulkanPlus::Plus().CreatePipeline("test-pbr-pipeline", configurator).first;
		};

	auto Destroy = [&] {
		VulkanPlus::Plus().DestroyPipeline(shader_info.pipeline_id);
		};

	VulkanBase::Base().AddCallback_CreateSwapchain(Create);
	VulkanBase::Base().AddCallback_DestroySwapchain(Destroy);

	Create();
}

void TestPBR::CreateFramebuffers()
{
	
}

void TestPBR::OtherOperations()
{
	sphere.SetupModel(shader_info);

	VkDescriptorBufferInfo bufferInfo = {
		.buffer = sphere_uniform_buffer,
		.offset = 0,
		.range = sizeof(SphereUniform)
	};
	uniform_set.Write(bufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 2);
}

void TestPBR::UpdateDescriptorSets()
{
	vertex_uniform.model = glm::mat4(1.0f);
	vertex_uniform.view = GlfwWindow::camera.ViewTransform();
	vertex_uniform.proj = GlfwWindow::camera.PerspectiveTransform();

	vertex_uniform_buffer.TransferData(&vertex_uniform, sizeof vertex_uniform);

	VkDescriptorBufferInfo bufferInfo = {
		.buffer = vertex_uniform_buffer,
		.offset = 0,
		.range = sizeof vertex_uniform
	};

	uniform_set.Write(bufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0);


	light_uniform_buffer.TransferData(&light_uniform, sizeof light_uniform);

	bufferInfo = {
		.buffer = light_uniform_buffer,
		.offset = 0,
		.range = sizeof light_uniform
	};

	uniform_set.Write(bufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);

}

void TestPBR::RecordCommandBuffer()
{
	uint32_t current_image_ind = VulkanBase::Base().CurrentImageIndex();

	const RenderPass& renderPass = VulkanPlus::Plus().SwapchainRenderPassWithDepthStencil();
	const CommandBuffer& commandBuffer = VulkanPlus::Plus().CommandBuffer_Graphics();
	const Framebuffer& framebuffer = VulkanPlus::Plus().CurrentSwapchainFramebufferWithDepthStencil();

	VkRect2D renderArea = { {}, VulkanBase::Base().SwapchainExtent() };
	VkClearValue clearValues[2] = {
		{.color = { 0.4f, 0.4f, 0.4f, 1.f } },
		{.depthStencil = { 1.f, 0 } }
	};

	const uint32_t sphere_id = 0;
	VkDeviceSize uniformAlignment = VulkanBase::Base().PhysicalDeviceProperties().limits.minUniformBufferOffsetAlignment;
	uniformAlignment = (uniformAlignment + sizeof(SphereUniform) - 1) & ~(uniformAlignment - 1);
	const uint32_t dynamicOffset = sphere_id * uniformAlignment;

	renderPass.Begin(commandBuffer, framebuffer, renderArea, clearValues);
	PipelineLayout& pipeline_layout = VulkanPlus::Plus().GetPipelineLayout(shader_info.pipeline_layout_id).second[0];
	DescriptorSetLayout& uniform_set_layout = VulkanPlus::Plus().GetDescriptorSetLayout(uniform_set_layout_id).second[0];
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout,
		1, 1, uniform_set.Address(), 1, &dynamicOffset);
	vkCmdPushConstants(commandBuffer, pipeline_layout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(glm::vec3), &GlfwWindow::camera.Position());
	sphere.Render(shader_info);
	renderPass.End(commandBuffer);
}

void TestPBR::SendDataToNextNode()
{

}

void TestPBR::CreateSphere(uint32_t segments, uint32_t rings, float radius)
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<TextureInfo> textures;

	float segment_step = 2 * glm::pi<float>()/segments;
	float ring_step = glm::pi<float>() / rings;
	
	for (uint32_t y = 0; y <= rings; y++) {
		for (uint32_t x = 0; x <= segments; x++) {
			float theta = y * ring_step;
			float phi = x * segment_step;

			glm::vec3 vector;
			vector.x = radius * glm::sin(theta) * glm::cos(phi);
			vector.y = radius * glm::cos(theta);
			vector.z = radius * glm::sin(theta) * glm::sin(phi);

			Vertex vertex;
			vertex.SetPosition(vector);
			vertex.SetNormal(glm::normalize(vector));

			glm::vec3 tangent;
			tangent.x = -radius * sin(theta) * sin(phi);
			tangent.y = 0.0f;
			tangent.z = radius * sin(theta) * cos(phi);
			tangent = glm::normalize(tangent);
			vertex.AddColor(glm::vec4(tangent, 1.0f));

			glm::vec3 bitangent;
			bitangent.x = radius * cos(theta) * cos(phi);
			bitangent.y = -radius * sin(theta);
			bitangent.z = radius * cos(theta) * sin(phi);
			bitangent = glm::normalize(bitangent);
			vertex.AddColor(glm::vec4(bitangent, 1.0f));

			vertex.AddUV(glm::vec2((float)x / segments, (float)y / rings));
			vertices.push_back(vertex);
		}
	}

	for (uint32_t y = 0; y < rings; y++) {
		for (uint32_t x = 0; x < segments; x++) {

			uint32_t i0 = y * (segments + 1) + x;      
			uint32_t i1 = i0 + 1;                      
			uint32_t i2 = (y + 1) * (segments + 1) + x; 
			uint32_t i3 = i2 + 1;

			indices.push_back(i0);
			indices.push_back(i1);
			indices.push_back(i2);

			indices.push_back(i1);
			indices.push_back(i3);
			indices.push_back(i2);
		}
	}
	std::vector<Mesh> meshes;
	meshes.emplace_back(vertices, indices, textures);
	sphere.LoadModel(meshes);
}

void TestPBR::ImguiRender()
{
}
