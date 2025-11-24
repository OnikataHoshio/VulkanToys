#include "PBRPrecompute.h"


namespace HoshioEngine
{
	void PBRPrecomputeNode::InitResource()
	{
		const char* const hdri_path = "test/TestPBR/Resource/images/hdr-bg2.hdr";
		hdrImage.Create(hdri_path, VK_FORMAT_R32G32B32A32_SFLOAT, VK_FORMAT_R16G16B16A16_SFLOAT, false);
	}

	void PBRPrecomputeNode::CreateSampler()
	{
		sampler.Create(Sampler::SamplerCreateInfo());
	}

	void PBRPrecomputeNode::CreateBuffer()
	{
		uniform_buffer.Create(sizeof vertex_uniform);

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

	void PBRPrecomputeNode::CreateRenderPass()
	{
	}

	void PBRPrecomputeNode::CreateDescriptorSetLayout()
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

		descriptor_set_layout.Create(createInfo);
		VulkanPlus::Plus().DescriptorPool().AllocateDescriptorSets(descriptor_set, descriptor_set_layout);
	}

	void PBRPrecomputeNode::CreatePipelineLayout()
	{
	}

	void PBRPrecomputeNode::CreatePipeline()
	{
	}

	void PBRPrecomputeNode::CreateFramebuffers()
	{
	}

	void PBRPrecomputeNode::OtherOperations()
	{
		CmdTransferHDRIToCubeMap();
		CmdEnvPrefilter();
		CmdPrecomputeBRDF();
		CmdKullaConty(); 
	}

	void PBRPrecomputeNode::UpdateDescriptorSets()
	{
	}

	void PBRPrecomputeNode::RecordCommandBuffer()
	{
	}

	void PBRPrecomputeNode::SendDataToNextNode()
	{
	}

	void PBRPrecomputeNode::CmdTransferHDRIToCubeMap()
	{
		descriptor_set.Write(hdrImage.DescriptorImageInfo(sampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
		const VkExtent2D envExtent = VkExtent2D{ 512, 512 };
		{
			//Create cubeAttachment
			envAttachment.Create(VK_FORMAT_R16G16B16A16_SFLOAT, envExtent, true, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);

			//Create renderPass
			VkAttachmentDescription attachmentDescription = {
				.format = VK_FORMAT_R16G16B16A16_SFLOAT,
				.samples = VK_SAMPLE_COUNT_1_BIT,
				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
				.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			};

			VkAttachmentReference attachmentReference = {
				.attachment = 0,
				.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			};


			VkSubpassDescription subpassDescription = {
				.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
				.colorAttachmentCount = 1,
				.pColorAttachments = &attachmentReference,
			};
			VkSubpassDependency subpassDependency = {
				.srcSubpass = VK_SUBPASS_EXTERNAL,
				.dstSubpass = 0,
				.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				.dstAccessMask = VK_ACCESS_SHADER_READ_BIT
			};

			VkRenderPassCreateInfo renderPassCreateInfo = {
				.attachmentCount = 1,
				.pAttachments = &attachmentDescription,
				.subpassCount = 1,
				.pSubpasses = &subpassDescription,
				.dependencyCount = 1,
				.pDependencies = &subpassDependency
			};

			RenderPass renderpass(renderPassCreateInfo);

			//Create Framebuffer for cube attachment
			std::vector<Framebuffer> cubeframebuffers(6);
			VkFramebufferCreateInfo framebufferCreateInfo = {
				.renderPass = renderpass,
				.attachmentCount = 1,
				.width = envExtent.width,
				.height = envExtent.height,
				.layers = 1,
			};
			for (uint32_t i = 0; i < 6; i++) {
				framebufferCreateInfo.pAttachments = envAttachment.AddressOfImageView(i);
				cubeframebuffers[i].Create(framebufferCreateInfo);
			}

			//Create write cubemap pipeline
			VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
				.setLayoutCount = 1,
				.pSetLayouts = descriptor_set_layout.Address()
			};
			PipelineLayout pipeline_layout(pipelineLayoutCreateInfo);

			ShaderModule vertModule("test/TestPBR/Resource/Shaders/SPIR-V/CylinderMap.vert.spv");
			ShaderModule fragModule("test/TestPBR/Resource/Shaders/SPIR-V/CylinderMap.frag.spv");
			PipelineConfigurator configurator;
			configurator.PipelineLayout(pipeline_layout)
				.RenderPass(renderpass)
				.AddVertexInputBindings(0, sizeof(float) * 3, VK_VERTEX_INPUT_RATE_VERTEX)
				.AddVertexInputAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0)
				.PrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
				.AddViewport(0.0f, 0.0f,
					envExtent.width, envExtent.height,
					0.0f, 1.0f)
				.AddScissor(VkOffset2D{}, envExtent)
				.CullMode(VK_CULL_MODE_NONE)
				.EnableDepthTest(VK_TRUE, VK_TRUE)
				.RasterizationSamples(VK_SAMPLE_COUNT_1_BIT)
				.AddAttachmentState(0b1111)
				.AddShaderStage(vertModule.ShaderStageCi(VK_SHADER_STAGE_VERTEX_BIT))
				.AddShaderStage(fragModule.ShaderStageCi(VK_SHADER_STAGE_FRAGMENT_BIT))
				.UpdatePipelineCreateInfo();
			Pipeline pipeline(configurator);

			//Record commandbuffer
			{
				//prepare uniform data
				//right(+X)  left(-X)  Top(+Y)   Bottom(-Y)   Front(+Z)    Back(-Z)
				glm::mat4 views[6];
				views[0] = glm::lookAt(glm::vec3(0.0f), glm::vec3(1, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f));
				views[1] = glm::lookAt(glm::vec3(0.0f), glm::vec3(-1, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f));
				views[2] = glm::lookAt(glm::vec3(0.0f), glm::vec3(0, 1, 0), glm::vec3(0.0f, 0.0f, 1.0f));
				views[3] = glm::lookAt(glm::vec3(0.0f), glm::vec3(0, -1, 0), glm::vec3(0.0f, 0.0f, -1.0f));
				views[4] = glm::lookAt(glm::vec3(0.0f), glm::vec3(0, 0, -1), glm::vec3(0.0f, 1.0f, 0.0f));
				views[5] = glm::lookAt(glm::vec3(0.0f), glm::vec3(0, 0, 1), glm::vec3(0.0f, 1.0f, 0.0f));

				vertex_uniform.model = glm::mat4(1.0f);
				//glm::infinitePerspectiveRH_ZO(glm::radians(90), 1.0f, 0.1f);
				vertex_uniform.proj = FlipVertical(glm::infinitePerspectiveRH_ZO(glm::radians(90.0f), 1.0f, 0.1f));

				const CommandBuffer& commandBuffer = VulkanPlus::Plus().CommandBuffer_Graphics();
				VkRect2D renderArea = { {}, envExtent };
				VkClearValue clearValue = {
					.color = { 0.f, 0.f, 0.f, 1.f } ,
				};
				VkDeviceSize offset = 0;

				for (size_t layerLevel = 0; layerLevel < 6; layerLevel++) {
					vertex_uniform.view = views[layerLevel];
					uniform_buffer.TransferData(&vertex_uniform, sizeof vertex_uniform);
					VkDescriptorBufferInfo bufferInfo = {
						.buffer = uniform_buffer,
						.offset = 0,
						.range = sizeof vertex_uniform
					};
					descriptor_set.Write(bufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0);

					//record commandbuffer

					commandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
					renderpass.Begin(commandBuffer, cubeframebuffers[layerLevel], renderArea, clearValue);
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout,
						0, 1, descriptor_set.Address(), 0, nullptr);
					vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertex_buffer.Address(), &offset);
					vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
					vkCmdDraw(commandBuffer, 36, 1, 0, 0);
					renderpass.End(commandBuffer);
					commandBuffer.End();

					VulkanPlus::Plus().ExecuteCommandBuffer_Graphics(commandBuffer);
				}

				commandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
				ImageUtils::CmdImagePipelineBarrier(commandBuffer, envAttachment.Image(),
					VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 6 },
					ImageBarrierInfo{ VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0 , VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL },
					ImageBarrierInfo{ VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL });
				ImageUtils::CmdGenerateMipmap2D(commandBuffer, envAttachment.Image(),
					envExtent, envAttachment.MipLevelCount(), 6,
					ImageBarrierInfo{ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL });
				commandBuffer.End();
				VulkanPlus::Plus().ExecuteCommandBuffer_Graphics(commandBuffer);
			}
		}
	}

	void PBRPrecomputeNode::CmdEnvPrefilter()
	{
		descriptor_set.Write(envAttachment.DescriptorImageInfo(sampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
		const VkExtent2D envExtent = VkExtent2D{ 128, 128 };
		{
			//Create cubeAttachment
			envPrefilterAttachment.Create(VK_FORMAT_R16G16B16A16_SFLOAT, envExtent, true, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_SAMPLED_BIT);

			//Create renderPass
			VkAttachmentDescription attachmentDescription = {
				.format = VK_FORMAT_R16G16B16A16_SFLOAT,
				.samples = VK_SAMPLE_COUNT_1_BIT,
				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
				.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			};

			VkAttachmentReference attachmentReference = {
				.attachment = 0,
				.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			};


			VkSubpassDescription subpassDescription = {
				.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
				.colorAttachmentCount = 1,
				.pColorAttachments = &attachmentReference,
			};
			VkSubpassDependency subpassDependency = {
				.srcSubpass = VK_SUBPASS_EXTERNAL,
				.dstSubpass = 0,
				.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				.dstAccessMask = VK_ACCESS_SHADER_READ_BIT
			};

			VkRenderPassCreateInfo renderPassCreateInfo = {
				.attachmentCount = 1,
				.pAttachments = &attachmentDescription,
				.subpassCount = 1,
				.pSubpasses = &subpassDescription,
				.dependencyCount = 1,
				.pDependencies = &subpassDependency
			};

			RenderPass renderpass(renderPassCreateInfo);

			const uint32_t MipLevelCount = 5;
			std::vector<std::vector<Framebuffer>> cubeFramebuffers(MipLevelCount);
			for (uint32_t i = 0; i < MipLevelCount; i++)
			{
				cubeFramebuffers[i].resize(6);
				VkFramebufferCreateInfo framebufferCreateInfo = {
					.renderPass = renderpass,
					.attachmentCount = 1,
					.width = envExtent.width >> i,
					.height = envExtent.height >> i,
					.layers = 1,
				};
				for (uint32_t j = 0; j < 6; j++)
				{
					framebufferCreateInfo.pAttachments = envPrefilterAttachment.AddressOfImageView(j, i);
					cubeFramebuffers[i][j].Create(framebufferCreateInfo);
				}
			}

			//Create write cubemap pipeline
			VkPushConstantRange pushConstantRange = {
				.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
				.offset = 0,
				.size = sizeof(float)
			};
			VkPipelineLayoutCreateInfo pipelineCreateInfo = {
				.setLayoutCount = 1,
				.pSetLayouts = descriptor_set_layout.Address(),
				.pushConstantRangeCount = 1,
				.pPushConstantRanges = &pushConstantRange
			};

			PipelineLayout pipeline_layout(pipelineCreateInfo);

			ShaderModule vertModule("test/TestPBR/Resource/Shaders/SPIR-V/EnvPrefilter.vert.spv");
			ShaderModule fragModule("test/TestPBR/Resource/Shaders/SPIR-V/EnvPrefilter.frag.spv");
			std::vector<Pipeline> pipelines(MipLevelCount);
			for (uint32_t i = 0; i < MipLevelCount; i++)
			{
				VkExtent2D mipEnvExtent =
				{
					.width = envExtent.width >> i,
					.height = envExtent.height >> i
				};

				PipelineConfigurator configurator;
				configurator.PipelineLayout(pipeline_layout)
					.RenderPass(renderpass)
					.AddVertexInputBindings(0, sizeof(float) * 3, VK_VERTEX_INPUT_RATE_VERTEX)
					.AddVertexInputAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0)
					.PrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
					.AddViewport(0.0f, 0.0f,
						mipEnvExtent.width, mipEnvExtent.height,
						0.0f, 1.0f)
					.AddScissor(VkOffset2D{}, mipEnvExtent)
					.CullMode(VK_CULL_MODE_NONE)
					.EnableDepthTest(VK_TRUE, VK_TRUE)
					.RasterizationSamples(VK_SAMPLE_COUNT_1_BIT)
					.AddAttachmentState(0b1111)
					.AddShaderStage(vertModule.ShaderStageCi(VK_SHADER_STAGE_VERTEX_BIT))
					.AddShaderStage(fragModule.ShaderStageCi(VK_SHADER_STAGE_FRAGMENT_BIT))
					.UpdatePipelineCreateInfo();
				pipelines[i].Create(configurator);
			}


			//Record commandbuffer
			{
				//prepare uniform data
				//right(+X)  left(-X)  Top(+Y)   Bottom(-Y)   Front(+Z)    Back(-Z)
				glm::mat4 views[6];
				views[0] = glm::lookAt(glm::vec3(0.0f), glm::vec3(1, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f));
				views[1] = glm::lookAt(glm::vec3(0.0f), glm::vec3(-1, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f));
				views[2] = glm::lookAt(glm::vec3(0.0f), glm::vec3(0, 1, 0), glm::vec3(0.0f, 0.0f, 1.0f));
				views[3] = glm::lookAt(glm::vec3(0.0f), glm::vec3(0, -1, 0), glm::vec3(0.0f, 0.0f, -1.0f));
				views[4] = glm::lookAt(glm::vec3(0.0f), glm::vec3(0, 0, -1), glm::vec3(0.0f, 1.0f, 0.0f));
				views[5] = glm::lookAt(glm::vec3(0.0f), glm::vec3(0, 0, 1), glm::vec3(0.0f, 1.0f, 0.0f));

				vertex_uniform.model = glm::mat4(1.0f);
				//glm::infinitePerspectiveRH_ZO(glm::radians(90), 1.0f, 0.1f);
				vertex_uniform.proj = FlipVertical(glm::infinitePerspectiveRH_ZO(glm::radians(90.0f), 1.0f, 0.1f));

				const CommandBuffer& commandBuffer = VulkanPlus::Plus().CommandBuffer_Graphics();

				VkClearValue clearValue = {
					.color = { 0.f, 0.f, 0.f, 1.f } ,
				};
				VkRect2D renderArea = { {},
					envExtent
				};
				VkDeviceSize offset = 0;

				float roughness = 0.0;

				for (size_t layerLevel = 0; layerLevel < 6; layerLevel++) {
					vertex_uniform.view = views[layerLevel];
					uniform_buffer.TransferData(&vertex_uniform, sizeof vertex_uniform);
					VkDescriptorBufferInfo bufferInfo = {
						.buffer = uniform_buffer,
						.offset = 0,
						.range = sizeof vertex_uniform
					};
					descriptor_set.Write(bufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0);

					//record commandbuffer
					for (size_t mipLevel = 0; mipLevel < MipLevelCount; mipLevel++)
					{
						renderArea = { {},
							{envExtent.width >> mipLevel, envExtent.height >> mipLevel}
						};
						roughness = 1.0f * mipLevel / (MipLevelCount - 1);
						commandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
						renderpass.Begin(commandBuffer, cubeFramebuffers[mipLevel][layerLevel], renderArea, clearValue);
						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout,
							0, 1, descriptor_set.Address(), 0, nullptr);
						vkCmdPushConstants(commandBuffer, pipeline_layout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(float), &roughness);
						vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertex_buffer.Address(), &offset);
						vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines[mipLevel]);
						vkCmdDraw(commandBuffer, 36, 1, 0, 0);
						renderpass.End(commandBuffer);
						commandBuffer.End();
						VulkanPlus::Plus().ExecuteCommandBuffer_Graphics(commandBuffer);
					}
				}
			}
		}
	}

	void PBRPrecomputeNode::CmdPrecomputeBRDF()
	{
		const VkExtent2D envExtent = VkExtent2D{ 512, 512 };

		preBRDFTexture.Create(VK_FORMAT_R16G16_SFLOAT, envExtent, false, 1u, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_SAMPLED_BIT);

		VkAttachmentDescription attachmentDescription = {
			.format = VK_FORMAT_R16G16_SFLOAT,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		};

		VkAttachmentReference attachmentReference = {
			.attachment = 0,
			.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		};

		VkSubpassDescription subpassDescription = {
			.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
			.colorAttachmentCount = 1,
			.pColorAttachments = &attachmentReference,
		};
		VkSubpassDependency subpassDependency = {
			.srcSubpass = VK_SUBPASS_EXTERNAL,
			.dstSubpass = 0,
			.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.dstAccessMask = VK_ACCESS_SHADER_READ_BIT
		};

		VkRenderPassCreateInfo renderPassCreateInfo = {
			.attachmentCount = 1,
			.pAttachments = &attachmentDescription,
			.subpassCount = 1,
			.pSubpasses = &subpassDescription,
			.dependencyCount = 1,
			.pDependencies = &subpassDependency
		};

		RenderPass renderpass(renderPassCreateInfo);

		//Create Framebuffer for cube attachment
		Framebuffer framebuffer;
		VkFramebufferCreateInfo framebufferCreateInfo = {
			.renderPass = renderpass,
			.attachmentCount = 1,
			.width = envExtent.width,
			.height = envExtent.height,
			.layers = 1,
		};
		framebufferCreateInfo.pAttachments = preBRDFTexture.AddressOfImageView();
		framebuffer.Create(framebufferCreateInfo);

		//Create write cubemap pipeline
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
			
		};
		PipelineLayout pipeline_layout(pipelineLayoutCreateInfo);

		ShaderModule vertModule("test/TestPBR/Resource/Shaders/SPIR-V/Default.vert.spv");
		ShaderModule fragModule("test/TestPBR/Resource/Shaders/SPIR-V/PreBRDF.frag.spv");
		PipelineConfigurator configurator;
		configurator.PipelineLayout(pipeline_layout)
			.RenderPass(renderpass)
			.AddVertexInputBindings(0, sizeof(DefaultVertex), VK_VERTEX_INPUT_RATE_VERTEX)
			.AddVertexInputAttribute(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(DefaultVertex, position))
			.AddVertexInputAttribute(1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(DefaultVertex, texCoord))
			.PrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP)
			.AddViewport(0.0f, 0.0f,
				envExtent.width, envExtent.height,
				0.0f, 1.0f)
			.AddScissor(VkOffset2D{}, envExtent)
			.CullMode(VK_CULL_MODE_NONE)
			.EnableDepthTest(VK_TRUE, VK_TRUE)
			.RasterizationSamples(VK_SAMPLE_COUNT_1_BIT)
			.AddAttachmentState(0b1111)
			.AddShaderStage(vertModule.ShaderStageCi(VK_SHADER_STAGE_VERTEX_BIT))
			.AddShaderStage(fragModule.ShaderStageCi(VK_SHADER_STAGE_FRAGMENT_BIT))
			.UpdatePipelineCreateInfo();
		Pipeline pipeline(configurator);

		const CommandBuffer& commandBuffer = VulkanPlus::Plus().CommandBuffer_Graphics();
		VkRect2D renderArea = { {}, envExtent };
		VkClearValue clearValue = {
			.color = { 0.f, 0.f, 0.f, 1.f } ,
		};
		VkDeviceSize offset = 0;

		{
			commandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
			renderpass.Begin(commandBuffer, framebuffer, renderArea, clearValue);
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, VulkanPlus::Plus().DefaultVertexBuffer().Address(), &offset);
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
			vkCmdDraw(commandBuffer, 4, 1, 0, 0);
			renderpass.End(commandBuffer);
			commandBuffer.End();

			VulkanPlus::Plus().ExecuteCommandBuffer_Graphics(commandBuffer);
		}

	}

	void PBRPrecomputeNode::CmdKullaConty()
	{
		const VkExtent2D envExtent = VkExtent2D{ 512, 512 };

		kullaContyTexture.Create(VK_FORMAT_R16_SFLOAT, envExtent, false, 1u, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_SAMPLED_BIT);

		VkAttachmentDescription attachmentDescription = {
			.format = VK_FORMAT_R16_SFLOAT,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		};

		VkAttachmentReference attachmentReference = {
			.attachment = 0,
			.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		};

		VkSubpassDescription subpassDescription = {
			.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
			.colorAttachmentCount = 1,
			.pColorAttachments = &attachmentReference,
		};
		VkSubpassDependency subpassDependency = {
			.srcSubpass = VK_SUBPASS_EXTERNAL,
			.dstSubpass = 0,
			.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.dstAccessMask = VK_ACCESS_SHADER_READ_BIT
		};

		VkRenderPassCreateInfo renderPassCreateInfo = {
			.attachmentCount = 1,
			.pAttachments = &attachmentDescription,
			.subpassCount = 1,
			.pSubpasses = &subpassDescription,
			.dependencyCount = 1,
			.pDependencies = &subpassDependency
		};

		RenderPass renderpass(renderPassCreateInfo);

		//Create Framebuffer for cube attachment
		Framebuffer framebuffer;
		VkFramebufferCreateInfo framebufferCreateInfo = {
			.renderPass = renderpass,
			.attachmentCount = 1,
			.width = envExtent.width,
			.height = envExtent.height,
			.layers = 1,
		};
		framebufferCreateInfo.pAttachments = kullaContyTexture.AddressOfImageView();
		framebuffer.Create(framebufferCreateInfo);

		//Create write cubemap pipeline
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

		DescriptorSetLayout descriptorSetLayout(createInfo);
		DescriptorSet descriptorSet;
		VulkanPlus::Plus().DescriptorPool().AllocateDescriptorSets(descriptorSet, descriptorSetLayout);
		descriptorSet.Write(preBRDFTexture.DescriptorImageInfo(sampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);


		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
			.setLayoutCount = 1,
			.pSetLayouts = descriptorSetLayout.Address(),
		};

		PipelineLayout pipeline_layout(pipelineLayoutCreateInfo);

		ShaderModule vertModule("test/TestPBR/Resource/Shaders/SPIR-V/Default.vert.spv");
		ShaderModule fragModule("test/TestPBR/Resource/Shaders/SPIR-V/KullaConty.frag.spv");
		PipelineConfigurator configurator;
		configurator.PipelineLayout(pipeline_layout)
			.RenderPass(renderpass)
			.AddVertexInputBindings(0, sizeof(DefaultVertex), VK_VERTEX_INPUT_RATE_VERTEX)
			.AddVertexInputAttribute(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(DefaultVertex, position))
			.AddVertexInputAttribute(1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(DefaultVertex, texCoord))
			.PrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP)
			.AddViewport(0.0f, 0.0f,
				envExtent.width, envExtent.height,
				0.0f, 1.0f)
			.AddScissor(VkOffset2D{}, envExtent)
			.CullMode(VK_CULL_MODE_NONE)
			.EnableDepthTest(VK_TRUE, VK_TRUE)
			.RasterizationSamples(VK_SAMPLE_COUNT_1_BIT)
			.AddAttachmentState(0b1111)
			.AddShaderStage(vertModule.ShaderStageCi(VK_SHADER_STAGE_VERTEX_BIT))
			.AddShaderStage(fragModule.ShaderStageCi(VK_SHADER_STAGE_FRAGMENT_BIT))
			.UpdatePipelineCreateInfo();
		Pipeline pipeline(configurator);

		const CommandBuffer& commandBuffer = VulkanPlus::Plus().CommandBuffer_Graphics();
		VkRect2D renderArea = { {}, envExtent };
		VkClearValue clearValue = {
			.color = { 0.f, 0.f, 0.f, 1.f } ,
		};
		VkDeviceSize offset = 0;

		float F0 = 0.04;

		{
			commandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
			renderpass.Begin(commandBuffer, framebuffer, renderArea, clearValue);
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, VulkanPlus::Plus().DefaultVertexBuffer().Address(), &offset);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout, 0, 1,
				descriptorSet.Address(), 0, nullptr);
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
			vkCmdDraw(commandBuffer, 4, 1, 0, 0);
			renderpass.End(commandBuffer);
			commandBuffer.End();

			VulkanPlus::Plus().ExecuteCommandBuffer_Graphics(commandBuffer);
		}
	}

	void PBRPrecomputeNode::ImguiRender()
	{
	}

	

}