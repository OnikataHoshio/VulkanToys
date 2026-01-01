#include "BlinnPhong.h"
#include "Wins/GlfwManager.h"

namespace HoshioEngine
{
	BlinnPhong::BlinnPhong(const char* file_path) : file_path(file_path)
	{


	}

	void BlinnPhong::InitResource()
	{
		Texture::FlipVerticallyOnLoad = true;
		dashachun.LoadModel(file_path);
		Texture::FlipVerticallyOnLoad = false;
		CreateFloor();

		light[0].SetIntensity(0.8f);
		light[0].SetLightColor(glm::vec3(0.8f, 0.8f, 0.2f));
		light[0].SetPosition(glm::vec3(0.5f, 1.0f, -1.0f));
		light[1].SetRotation(glm::vec3(60.0f, 0.0f, 0.0f));
	}

	void BlinnPhong::CreateSampler()
	{
		shader_info.sampler_id = VulkanPlus::Plus().CreateSampler("trilinear-sampler", Sampler::SamplerCreateInfo()).first;
		shader_info_floor.sampler_id = shader_info.sampler_id;
	}

	void BlinnPhong::CreateBuffer()
	{
		vertex_uniform_buffer.Create(sizeof vertex_uniform);
		fragment_uniform_buffer.Create(sizeof fragment_uniform);
	}

	void BlinnPhong::CreateRenderPass()
	{

	}

	void BlinnPhong::CreateDescriptorSetLayout()
	{
		{
			VkDescriptorSetLayoutBinding binding = {
				.binding = 0,
				.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
			};

			VkDescriptorSetLayoutCreateInfo createInfo = {
				.bindingCount = 1,
				.pBindings = &binding
			};

			shader_info.sampler_set_layout_id = VulkanPlus::Plus().CreateDescriptorSetLayout("model-sampler-set-layout", createInfo).first;
			shader_info_floor.sampler_set_layout_id = shader_info.sampler_set_layout_id;
		}

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
					.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
					.descriptorCount = 1,
					.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
				}
			};
			VkDescriptorSetLayoutCreateInfo createInfo = {
				.bindingCount = 2,
				.pBindings = bindings
			};
			uniform_set_layout.Create(createInfo);
			VulkanPlus::Plus().DescriptorPool().AllocateDescriptorSets(uniform_set, uniform_set_layout);
		}
	}

	void BlinnPhong::CreatePipelineLayout()
	{
		DescriptorSetLayout& sampler_set_layout = VulkanPlus::Plus().GetDescriptorSetLayout(shader_info.sampler_set_layout_id).second[0];
		VkDescriptorSetLayout layouts[] = { sampler_set_layout, uniform_set_layout };

		VkPipelineLayoutCreateInfo createInfo = {
			.setLayoutCount = 2,
			.pSetLayouts = layouts
		};

		shader_info.pipeline_layout_id = VulkanPlus::Plus().CreatePipelineLayout("model-pipeline-layout", createInfo).first;
		shader_info_floor.pipeline_layout_id = shader_info.pipeline_layout_id;

	}

	void BlinnPhong::CreatePipeline()
	{
		auto Create = [&] {
			static ShaderModule vertModule("test/Shadow/Resource/Shaders/SPIR-V/Blinn-Phong.vert.spv");
			static ShaderModule fragModule("test/Shadow/Resource/Shaders/SPIR-V/Blinn-Phong.frag.spv");
			static ShaderModule vertModule_floor("test/Shadow/Resource/Shaders/SPIR-V/Floor.vert.spv");
			static ShaderModule fragModule_floor("test/Shadow/Resource/Shaders/SPIR-V/Floor.frag.spv");

			{
				PipelineConfigurator configurator;
				std::vector<VertexInputAttribute> vertex_input_attributes = dashachun.GetVertexInputeAttributes();
				uint32_t stride = dashachun.GetVertexInputAttributesStride();
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
				shader_info.pipeline_id = VulkanPlus::Plus().CreatePipeline("test-model-pipeline", configurator).first;
			}
			{
				PipelineConfigurator configurator;
				std::vector<VertexInputAttribute> vertex_input_attributes = floor.GetVertexInputeAttributes();
				uint32_t stride = floor.GetVertexInputAttributesStride();
				configurator.AddVertexInputBindings(0, stride, VK_VERTEX_INPUT_RATE_VERTEX);
				for (auto& attribute : vertex_input_attributes) {
					configurator.AddVertexInputAttribute(
						attribute.location, attribute.binding, attribute.format, attribute.offset);
				}

				PipelineLayout& pipeline_layout = VulkanPlus::Plus().GetPipelineLayout(shader_info_floor.pipeline_layout_id).second[0];
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
					.AddShaderStage(vertModule_floor.ShaderStageCi(VK_SHADER_STAGE_VERTEX_BIT))
					.AddShaderStage(fragModule_floor.ShaderStageCi(VK_SHADER_STAGE_FRAGMENT_BIT))
					.UpdatePipelineCreateInfo();
				shader_info_floor.pipeline_id = VulkanPlus::Plus().CreatePipeline("floor-model-pipeline", configurator).first;
			}
			};

		auto Destroy = [&] {
			VulkanPlus::Plus().DestroyPipeline(shader_info.pipeline_id);
			VulkanPlus::Plus().DestroyPipeline(shader_info_floor.pipeline_id);
			};

		VulkanBase::Base().AddCallback_CreateSwapchain(Create);
		VulkanBase::Base().AddCallback_DestroySwapchain(Destroy);

		Create();
	}

	void BlinnPhong::CreateFramebuffers()
	{
	}

	void BlinnPhong::OtherOperations()
	{
		dashachun.SetupModel(shader_info);
		floor.SetupModel(shader_info_floor);
	}

	void BlinnPhong::UpdateDescriptorSets()
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

		fragment_uniform.CameraPos = glm::vec4(GlfwWindow::camera.Position(),1.0f);
		fragment_uniform.PointLightPos = glm::vec4(light[0].Position(), 1.0f);
		fragment_uniform.I_p = glm::vec4(light[0].GetLightColorPlusIntensity(), 1.0f);
		fragment_uniform.SunLightDir = glm::vec4(light[1].GetLightDirection(), 1.0f);
		fragment_uniform.I_s = light[1].GetLightColorPlusIntensity();
		fragment_uniform.Shininess = 32.0f;

		fragment_uniform_buffer.TransferData(&fragment_uniform, sizeof fragment_uniform);

		bufferInfo = {
		   .buffer = fragment_uniform_buffer,
		   .offset = 0,
		   .range = sizeof fragment_uniform
		};

		uniform_set.Write(bufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);
	}

	void BlinnPhong::RecordCommandBuffer()
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

		VkDeviceSize offset = 0;

		renderPass.Begin(commandBuffer, framebuffer, renderArea, clearValues);
		PipelineLayout& pipeline_layout = VulkanPlus::Plus().GetPipelineLayout(shader_info.pipeline_layout_id).second[0];
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout,
			1, 1, uniform_set.Address(), 0, nullptr);
		dashachun.Render(shader_info);
		floor.Render(shader_info_floor);
		renderPass.End(commandBuffer);
	}

	void BlinnPhong::CreateFloor(float length, float width)
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<TextureInfo> textures;

		float half_x = width / 2.0f;
		float half_z = length / 2.0f;

		Vertex vertex;

		vertex.SetNormal(glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec3 rectangle_vertices[4] =
		{
			glm::vec3(-half_x, 0.0, -half_z),
			glm::vec3(half_x, 0.0, -half_z),
			glm::vec3(half_x, 0.0,  half_z),
			glm::vec3(-half_x, 0.0,  half_z)
		};

		for (size_t i = 0; i < 4; i++) {
			vertex.SetPosition(rectangle_vertices[i]);
			vertices.push_back(vertex);
		}

		indices.assign({ 0, 2, 1, 0, 3, 2 });

		std::vector<Mesh> meshes;
		meshes.emplace_back(vertices, indices, textures);
		floor.LoadModel(meshes);
	}

	void BlinnPhong::SendDataToNextNode()
	{

	}

	void BlinnPhong::ImguiRender()
	{
	}
}