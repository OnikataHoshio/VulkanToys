#include "TestModel.h"
#include "Wins/GlfwManager.h"

namespace HoshioEngine {
	TestModel::TestModel(const char* file_path) : model(file_path)
	{
	}

	void TestModel::UpdateDescriptorSets()
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
		//bufferInfo.buffer = fragment_uniform_buffer;
		//bufferInfo.offset = 0;
		//bufferInfo.range = 4;
		//uniform_set.Write(bufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);
	}

	void TestModel::RecordCommandBuffer()
	{
		uint32_t current_image_ind = VulkanBase::Base().CurrentImageIndex();

		const RenderPass& renderPass = VulkanPlus::Plus().SwapchainRenderPassWithDepthStencil();
		const CommandBuffer& commandBuffer = VulkanPlus::Plus().CommandBuffer_Graphics();
		const Framebuffer& framebuffer = VulkanPlus::Plus().CurrentSwapchainFramebufferWithDepthStencil();

		VkRect2D renderArea = {{}, VulkanBase::Base().SwapchainExtent() };
		VkClearValue clearValues[2] = {
			{.color = { 0.f, 0.f, 0.f, 1.f } },
			{.depthStencil = { 1.f, 0 } }
		};

		renderPass.Begin(commandBuffer, framebuffer, renderArea, clearValues);
		PipelineLayout& pipeline_layout = VulkanPlus::Plus().GetPipelineLayout(shader_info.pipeline_layout_id).second[0];
		DescriptorSetLayout& uniform_set_layout = VulkanPlus::Plus().GetDescriptorSetLayout(uniform_set_layout_id).second[0];
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout,
			1, 1, uniform_set.Address(), 0, nullptr);
		model.Render(shader_info);
		renderPass.End(commandBuffer);
	}

	void TestModel::InitResource()
	{
	}

	void TestModel::CreateSampler()
	{
		//create sampler
		shader_info.sampler_id = VulkanPlus::Plus().CreateSampler("trilinear-sampler", Sampler::SamplerCreateInfo()).first;
	}
	void TestModel::CreateBuffer()
	{
		
		vertex_uniform_buffer.Create(sizeof vertex_uniform);
			

		//fragment_uniform_buffer.Create(4);
	}

	void TestModel::CreateRenderPass()
	{

	}
	void TestModel::CreateDescriptorSetLayout()
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
			shader_info.sampler_set_layout_id = VulkanPlus::Plus().CreateDescriptorSetLayout("model-sampler-set-layout", createInfo).first;
		}

		//create uniform set layout
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
			uniform_set_layout_id = VulkanPlus::Plus().CreateDescriptorSetLayout("model-uniform-set-layot", createInfo).first;
			DescriptorSetLayout& uniform_set_layout = VulkanPlus::Plus().GetDescriptorSetLayout(uniform_set_layout_id).second[0];
			VulkanPlus::Plus().DescriptorPool().AllocateDescriptorSets(uniform_set, uniform_set_layout);
		}
	}

	void TestModel::CreatePipelineLayout()
	{
		DescriptorSetLayout& sampler_set_layout = VulkanPlus::Plus().GetDescriptorSetLayout(shader_info.sampler_set_layout_id).second[0];
		DescriptorSetLayout& uniform_set_layout = VulkanPlus::Plus().GetDescriptorSetLayout(uniform_set_layout_id).second[0];
		VkDescriptorSetLayout layouts[] = { sampler_set_layout, uniform_set_layout};

		VkPipelineLayoutCreateInfo createInfo = {
			.setLayoutCount = 2,
			.pSetLayouts = layouts
		};

		shader_info.pipeline_layout_id = VulkanPlus::Plus().CreatePipelineLayout("model-pipeline-layout", createInfo).first;

	}

	void TestModel::CreatePipeline()
	{
		auto Create = [&] {
			static ShaderModule vertModule("test/TestModel/Resource/Shaders/SPIR-V/Blinn-Phong.vert.spv");
			static ShaderModule fragModule("test/TestModel/Resource/Shaders/SPIR-V/Blinn-Phong.frag.spv");

			PipelineConfigurator configurator;
			std::vector<VertexInputAttribute> vertex_input_attributes = model.GetVertexInputeAttributes();
			uint32_t stride = model.GetVertexInputAttributesStride();
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
			shader_info.pipeline_id = VulkanPlus::Plus().CreatePipeline("test-model-pipeline",configurator).first;
			};

		auto Destroy = [&] {
			VulkanPlus::Plus().DestroyPipeline(shader_info.pipeline_id);
			};

		VulkanBase::Base().AddCallback_CreateSwapchain(Create);
		VulkanBase::Base().AddCallback_DestroySwapchain(Destroy);

		Create();
	}

	void TestModel::CreateFramebuffers()
	{
		
	}

	void TestModel::OtherOperations()
	{
		model.SetupModel(shader_info);

	}

	void TestModel::SendDataToNextNode()
	{

	}

	void TestModel::ImguiRender()
	{

	}

}