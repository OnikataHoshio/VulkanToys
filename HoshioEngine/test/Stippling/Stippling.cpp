#include "Stippling.h"

void Stippling::InitResource()
{
	PictureTexture.Create("test/Stippling/Resource/images/2K.png", VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, false);
	//PictureTexture.Create("test/Stippling/Resource/images/1024x1024-2.png", VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, false);
	BlueNoiseTexture.Create("test/Stippling/Resource/images/BlueNoise.png", VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, false);
	PureWhiteTexture.Create("test/Stippling/Resource/images/PureWhite.png", VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, false);
	PureBlackTexture.Create("test/Stippling/Resource/images/PureBlack.png", VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, false);
	PaperTexture.Create("test/Stippling/Resource/images/Paper2.jpg", VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, false);
	BrushTexture.Create("test/Stippling/Resource/images/Brush5.jpg", VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, false);
	PictureExtent2D = PictureTexture.Extent();
}

void Stippling::CreateSampler()
{
	LinearSampler.Create(Sampler::SamplerCreateInfo());
	NearestSampler.Create(Sampler::SamplerCreateInfo(
		VK_FILTER_NEAREST, 
		VK_FILTER_NEAREST,
		VK_SAMPLER_MIPMAP_MODE_NEAREST));

	VkSamplerCreateInfo SamplerCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.magFilter = VK_FILTER_LINEAR,
			.minFilter = VK_FILTER_LINEAR,
			.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
			.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT, 
			.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT, 
			.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.mipLodBias = 0.f,
			.anisotropyEnable = VK_FALSE,
			.maxAnisotropy = VulkanBase::Base().PhysicalDeviceProperties().limits.maxSamplerAnisotropy,
			.compareEnable = VK_FALSE,
			.compareOp = VK_COMPARE_OP_ALWAYS,
			.minLod = 0.f,
			.maxLod = VK_LOD_CLAMP_NONE,
			.borderColor = {},
			.unnormalizedCoordinates = VK_FALSE
	};
	LinearRepeatSampler.Create(SamplerCreateInfo);
}

void Stippling::CreateBuffer()
{
}

void Stippling::CreateRenderPass()
{
	VkAttachmentDescription PassCommonAttachmentDescription = {};
	PassCommonAttachmentDescription.format = VK_FORMAT_R32_SFLOAT;
	PassCommonAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	PassCommonAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	PassCommonAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	PassCommonAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkAttachmentReference PassCommonAttachmentReference = {};
	PassCommonAttachmentReference.attachment = 0;
	PassCommonAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription PassCommonSubpassDescription = {};
	PassCommonSubpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	PassCommonSubpassDescription.colorAttachmentCount = 1;
	PassCommonSubpassDescription.pColorAttachments = &PassCommonAttachmentReference;

	VkSubpassDependency PassCommonSubpassDependency = {};
	PassCommonSubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	PassCommonSubpassDependency.dstSubpass = 0;
	PassCommonSubpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	PassCommonSubpassDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	PassCommonSubpassDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	PassCommonSubpassDependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	VkRenderPassCreateInfo PassCommonRenderPassCreateInfo = {};
	PassCommonRenderPassCreateInfo.attachmentCount = 1;
	PassCommonRenderPassCreateInfo.pAttachments = &PassCommonAttachmentDescription;
	PassCommonRenderPassCreateInfo.subpassCount = 1;
	PassCommonRenderPassCreateInfo.pSubpasses = &PassCommonSubpassDescription;
	PassCommonRenderPassCreateInfo.dependencyCount = 1;
	PassCommonRenderPassCreateInfo.pDependencies = &PassCommonSubpassDependency;

	PassCommonRenderPass.Create(PassCommonRenderPassCreateInfo);
}

void Stippling::CreateDescriptorSetLayout()
{
	//SingleSampler2D
	VkDescriptorSetLayoutBinding SingleSampler2DBinding = {};
	SingleSampler2DBinding.binding = 0;
	SingleSampler2DBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	SingleSampler2DBinding.descriptorCount = 1;
	SingleSampler2DBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo SingleSampler2DSetLayoutCreateInfo = {};
	SingleSampler2DSetLayoutCreateInfo.bindingCount = 1;
	SingleSampler2DSetLayoutCreateInfo.pBindings = &SingleSampler2DBinding;

	SingleSampler2DSetLayout.Create(SingleSampler2DSetLayoutCreateInfo);
	VulkanPlus::Plus().DescriptorPool().AllocateDescriptorSets(EdgeTestSet, SingleSampler2DSetLayout);
	VulkanPlus::Plus().DescriptorPool().AllocateDescriptorSets(NMSSet, SingleSampler2DSetLayout);

	//DoubleSampler2D
	VkDescriptorSetLayoutBinding DoubleSampler2DBindings[2] = {};
	DoubleSampler2DBindings[0].binding = 0;
	DoubleSampler2DBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	DoubleSampler2DBindings[0].descriptorCount = 1;
	DoubleSampler2DBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
						  
	DoubleSampler2DBindings[1].binding = 1;
	DoubleSampler2DBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	DoubleSampler2DBindings[1].descriptorCount = 1;
	DoubleSampler2DBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo DoubleSampler2DSetLayoutCreateInfo = {};
	DoubleSampler2DSetLayoutCreateInfo.bindingCount = 2;
	DoubleSampler2DSetLayoutCreateInfo.pBindings = DoubleSampler2DBindings;

	DoubleSampler2DSetLayout.Create(DoubleSampler2DSetLayoutCreateInfo);
	VulkanPlus::Plus().DescriptorPool().AllocateDescriptorSets(StipplingSet, DoubleSampler2DSetLayout);
	VulkanPlus::Plus().DescriptorPool().AllocateDescriptorSets(LineWidthAdaptedSet, DoubleSampler2DSetLayout);


	VkDescriptorSetLayoutBinding BlendSetLayoutBindings[4] = {};
	BlendSetLayoutBindings[0].binding = 0;
	BlendSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	BlendSetLayoutBindings[0].descriptorCount = 1;
	BlendSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	
	BlendSetLayoutBindings[1].binding = 1;
	BlendSetLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	BlendSetLayoutBindings[1].descriptorCount = 1;
	BlendSetLayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	
	BlendSetLayoutBindings[2].binding = 2;
	BlendSetLayoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	BlendSetLayoutBindings[2].descriptorCount = 1;
	BlendSetLayoutBindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	BlendSetLayoutBindings[3].binding = 3;
	BlendSetLayoutBindings[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	BlendSetLayoutBindings[3].descriptorCount = 1;
	BlendSetLayoutBindings[3].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo BlendSetLayoutCreateInfo = {};
	BlendSetLayoutCreateInfo.bindingCount = 4;
	BlendSetLayoutCreateInfo.pBindings = BlendSetLayoutBindings;

	BlendSetLayout.Create(BlendSetLayoutCreateInfo);
	VulkanPlus::Plus().DescriptorPool().AllocateDescriptorSets(BlendSet, BlendSetLayout);

}

void Stippling::CreatePipelineLayout()
{
	//SingleSampler2D
	VkPipelineLayoutCreateInfo SingleSampler2DPipelineLayoutCreateInfo = {};
	SingleSampler2DPipelineLayoutCreateInfo.setLayoutCount = 1;
	SingleSampler2DPipelineLayoutCreateInfo.pSetLayouts = SingleSampler2DSetLayout.Address();
	EdgeTestPipelineLayout.Create(SingleSampler2DPipelineLayoutCreateInfo);

	//NMS
	VkPushConstantRange NMSPushConstantRange = {
		.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
		.offset = 0,
		.size = sizeof NMS_PC
	};
	SingleSampler2DPipelineLayoutCreateInfo.pushConstantRangeCount = 1;
	SingleSampler2DPipelineLayoutCreateInfo.pPushConstantRanges = &NMSPushConstantRange;
	NMSPipelineLayout.Create(SingleSampler2DPipelineLayoutCreateInfo);

	//DoubleSampler2D
	VkPipelineLayoutCreateInfo DoubleSampler2DPipelineLayoutCreateInfo = {};
	DoubleSampler2DPipelineLayoutCreateInfo.setLayoutCount = 1;
	DoubleSampler2DPipelineLayoutCreateInfo.pSetLayouts = DoubleSampler2DSetLayout.Address();
	

	//LineWidthAdapted
	VkPushConstantRange LineWidthAdaptedPushConstantRange = {
		.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
		.offset = 0,
		.size = sizeof LineWidthAdapted_PC
	};
	DoubleSampler2DPipelineLayoutCreateInfo.pushConstantRangeCount = 1;
	DoubleSampler2DPipelineLayoutCreateInfo.pPushConstantRanges = &LineWidthAdaptedPushConstantRange;
	LineWidthAdaptedPipelineLayout.Create(DoubleSampler2DPipelineLayoutCreateInfo);

	//Stippling
	VkPushConstantRange StipplingPushConstantRange = {
		.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
		.offset = 0,
		.size = sizeof Stippling_PC
	};
	DoubleSampler2DPipelineLayoutCreateInfo.pPushConstantRanges = &StipplingPushConstantRange;
	StipplingPipelineLayout.Create(DoubleSampler2DPipelineLayoutCreateInfo);

	//Blend
	VkPipelineLayoutCreateInfo BlendPipelineLayoutCreateInfo = {};
	BlendPipelineLayoutCreateInfo.setLayoutCount = 1;
	BlendPipelineLayoutCreateInfo.pSetLayouts = BlendSetLayout.Address();
	BlendPipelineLayout.Create(BlendPipelineLayoutCreateInfo);
}

void Stippling::CreatePipeline()
{
	ShaderModule DefaultVertexModule("test/Stippling/Resource/shaders/SPIR-V/Default.vert.spv");
	ShaderModule EdgeTestFragmentModule("test/Stippling/Resource/shaders/SPIR-V/EdgeTest.frag.spv");
	ShaderModule NMSFragmentModule("test/Stippling/Resource/shaders/SPIR-V/NMS.frag.spv");
	ShaderModule StipplingFragmentModule("test/Stippling/Resource/shaders/SPIR-V/Stippling.frag.spv");
	ShaderModule LineWidthAdaptedFragmentModule("test/Stippling/Resource/shaders/SPIR-V/LineWidthAdapted.frag.spv");
	ShaderModule BlendFragmentModule("test/Stippling/Resource/shaders/SPIR-V/Blend.frag.spv");

	{
		PipelineConfigurator configurator;
		configurator.PipelineLayout(EdgeTestPipelineLayout)
			.RenderPass(PassCommonRenderPass)
			.AddVertexInputBindings(0, sizeof(DefaultVertex), VK_VERTEX_INPUT_RATE_VERTEX)
			.AddVertexInputAttribute(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(DefaultVertex, position))
			.AddVertexInputAttribute(1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(DefaultVertex, texCoord))
			.PrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP)
			.AddViewport(0.0f, 0.0f, static_cast<float>(PictureExtent2D.width), static_cast<float>(PictureExtent2D.height), 0.0f, 1.0f)
			.AddScissor(VkOffset2D{}, PictureExtent2D)
			.RasterizationSamples(VK_SAMPLE_COUNT_1_BIT)
			.AddAttachmentState(0b1111)
			.AddShaderStage(DefaultVertexModule.ShaderStageCi(VK_SHADER_STAGE_VERTEX_BIT))
			.AddShaderStage(EdgeTestFragmentModule.ShaderStageCi(VK_SHADER_STAGE_FRAGMENT_BIT))
			.UpdatePipelineCreateInfo();
		//configurator.PrintPipelineCreateInfo("DrawScreenNodePipeline CreateInfo:");
		EdgeTestPipeline.Create(configurator);
	}

	{
		PipelineConfigurator configurator;
		configurator.PipelineLayout(NMSPipelineLayout)
			.RenderPass(PassCommonRenderPass)
			.AddVertexInputBindings(0, sizeof(DefaultVertex), VK_VERTEX_INPUT_RATE_VERTEX)
			.AddVertexInputAttribute(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(DefaultVertex, position))
			.AddVertexInputAttribute(1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(DefaultVertex, texCoord))
			.PrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP)
			.AddViewport(0.0f, 0.0f, static_cast<float>(PictureExtent2D.width), static_cast<float>(PictureExtent2D.height), 0.0f, 1.0f)
			.AddScissor(VkOffset2D{}, PictureExtent2D)
			.RasterizationSamples(VK_SAMPLE_COUNT_1_BIT)
			.AddAttachmentState(0b1111)
			.AddShaderStage(DefaultVertexModule.ShaderStageCi(VK_SHADER_STAGE_VERTEX_BIT))
			.AddShaderStage(NMSFragmentModule.ShaderStageCi(VK_SHADER_STAGE_FRAGMENT_BIT))
			.UpdatePipelineCreateInfo();
		//configurator.PrintPipelineCreateInfo("DrawScreenNodePipeline CreateInfo:");
		NMSPipeline.Create(configurator);
	}

	{
		PipelineConfigurator configurator;
		configurator.PipelineLayout(LineWidthAdaptedPipelineLayout)
			.RenderPass(PassCommonRenderPass)
			.AddVertexInputBindings(0, sizeof(DefaultVertex), VK_VERTEX_INPUT_RATE_VERTEX)
			.AddVertexInputAttribute(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(DefaultVertex, position))
			.AddVertexInputAttribute(1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(DefaultVertex, texCoord))
			.PrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP)
			.AddViewport(0.0f, 0.0f, static_cast<float>(PictureExtent2D.width), static_cast<float>(PictureExtent2D.height), 0.0f, 1.0f)
			.AddScissor(VkOffset2D{}, PictureExtent2D)
			.RasterizationSamples(VK_SAMPLE_COUNT_1_BIT)
			.AddAttachmentState(0b1111)
			.AddShaderStage(DefaultVertexModule.ShaderStageCi(VK_SHADER_STAGE_VERTEX_BIT))
			.AddShaderStage(LineWidthAdaptedFragmentModule.ShaderStageCi(VK_SHADER_STAGE_FRAGMENT_BIT))
			.UpdatePipelineCreateInfo();
		//configurator.PrintPipelineCreateInfo("DrawScreenNodePipeline CreateInfo:");
		LineWidthAdaptedPipeline.Create(configurator);
	}

	{
		PipelineConfigurator configurator;
		configurator.PipelineLayout(StipplingPipelineLayout)
			.RenderPass(PassCommonRenderPass)
			.AddVertexInputBindings(0, sizeof(DefaultVertex), VK_VERTEX_INPUT_RATE_VERTEX)
			.AddVertexInputAttribute(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(DefaultVertex, position))
			.AddVertexInputAttribute(1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(DefaultVertex, texCoord))
			.PrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP)
			.AddViewport(0.0f, 0.0f, static_cast<float>(PictureExtent2D.width), static_cast<float>(PictureExtent2D.height), 0.0f, 1.0f)
			.AddScissor(VkOffset2D{}, PictureExtent2D)
			.RasterizationSamples(VK_SAMPLE_COUNT_1_BIT)
			.AddAttachmentState(0b1111)
			.AddShaderStage(DefaultVertexModule.ShaderStageCi(VK_SHADER_STAGE_VERTEX_BIT))
			.AddShaderStage(StipplingFragmentModule.ShaderStageCi(VK_SHADER_STAGE_FRAGMENT_BIT))
			.UpdatePipelineCreateInfo();
		//configurator.PrintPipelineCreateInfo("DrawScreenNodePipeline CreateInfo:");
		StipplingPipeline.Create(configurator);
	}

	{
		PipelineConfigurator configurator;
		configurator.PipelineLayout(BlendPipelineLayout)
			.RenderPass(VulkanPlus::Plus().SwapchainRenderPass())
			.AddVertexInputBindings(0, sizeof(DefaultVertex), VK_VERTEX_INPUT_RATE_VERTEX)
			.AddVertexInputAttribute(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(DefaultVertex, position))
			.AddVertexInputAttribute(1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(DefaultVertex, texCoord))
			.PrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP)
			.AddViewport(0.0f, 0.0f, static_cast<float>(VulkanBase::Base().SwapchainCi().imageExtent.width), static_cast<float>(VulkanBase::Base().SwapchainCi().imageExtent.height), 0.0f, 1.0f)
			.AddScissor(VkOffset2D{}, VulkanBase::Base().SwapchainCi().imageExtent)
			.RasterizationSamples(VK_SAMPLE_COUNT_1_BIT)
			.AddAttachmentState(0b1111)
			.AddShaderStage(DefaultVertexModule.ShaderStageCi(VK_SHADER_STAGE_VERTEX_BIT))
			.AddShaderStage(BlendFragmentModule.ShaderStageCi(VK_SHADER_STAGE_FRAGMENT_BIT))
			.UpdatePipelineCreateInfo();
		//configurator.PrintPipelineCreateInfo("DrawScreenNodePipeline CreateInfo:");
		BlendPipeline.Create(configurator);
	}

}

void Stippling::CreateFramebuffers()
{
	const uint32_t SwapchainImageCount = VulkanBase::Base().SwapchainImageCount();
	
	EdgeTestAttachments.resize(SwapchainImageCount);
	NMSAttachments.resize(SwapchainImageCount);
	StipplingAttachments.resize(SwapchainImageCount);
	LineWidthAdaptedAttachments.resize(SwapchainImageCount);

	for (size_t i = 0; i < SwapchainImageCount; i++)
	{
		EdgeTestAttachments[i].Create(VK_FORMAT_R32_SFLOAT, PictureExtent2D, false, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_SAMPLED_BIT);
		NMSAttachments[i].Create(VK_FORMAT_R32_SFLOAT, PictureExtent2D, false, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_SAMPLED_BIT);
		StipplingAttachments[i].Create(VK_FORMAT_R32_SFLOAT, PictureExtent2D, false, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_SAMPLED_BIT);
		LineWidthAdaptedAttachments[i].Create(VK_FORMAT_R32_SFLOAT, PictureExtent2D, false, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_SAMPLED_BIT);
	}
	
	EdgeTestFramebuffers.resize(SwapchainImageCount);
	NMSFramebuffers.resize(SwapchainImageCount);
	StipplingFramebuffers.resize(SwapchainImageCount);
	LineWidthAdaptedFramebuffers.resize(SwapchainImageCount);

	VkFramebufferCreateInfo FramebufferCreateInfo = {
		.renderPass = PassCommonRenderPass,
		.attachmentCount = 1,
		.width = PictureExtent2D.width,
		.height = PictureExtent2D.height,
		.layers = 1
	};

	for (uint32_t i = 0; i < SwapchainImageCount; i++) {
		FramebufferCreateInfo.pAttachments = EdgeTestAttachments[i].AddressOfImageView();
		EdgeTestFramebuffers[i].Create(FramebufferCreateInfo);
		FramebufferCreateInfo.pAttachments = NMSAttachments[i].AddressOfImageView();
		NMSFramebuffers[i].Create(FramebufferCreateInfo);
		FramebufferCreateInfo.pAttachments = StipplingAttachments[i].AddressOfImageView();
		StipplingFramebuffers[i].Create(FramebufferCreateInfo);
		FramebufferCreateInfo.pAttachments = LineWidthAdaptedAttachments[i].AddressOfImageView();
		LineWidthAdaptedFramebuffers[i].Create(FramebufferCreateInfo);
	}

}

void Stippling::OtherOperations()
{
	EdgeTestSet.Write(PictureTexture.DescriptorImageInfo(NearestSampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);
	StipplingSet.Write(PictureTexture.DescriptorImageInfo(LinearSampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);
	StipplingSet.Write(BlueNoiseTexture.DescriptorImageInfo(LinearRepeatSampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
	LineWidthAdaptedSet.Write(PictureTexture.DescriptorImageInfo(LinearSampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);

}

void Stippling::UpdateDescriptorSets()
{
	const uint32_t ImageIndex = VulkanBase::Base().CurrentImageIndex();
	NMSSet.Write(EdgeTestAttachments[ImageIndex].DescriptorImageInfo(NearestSampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);
	LineWidthAdaptedSet.Write(NMSAttachments[ImageIndex].DescriptorImageInfo(LinearSampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);

	switch (PassToPresent)
	{
	case 0:
		BlendSet.Write(EdgeTestAttachments[ImageIndex].DescriptorImageInfo(LinearSampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);
		BlendSet.Write(PureWhiteTexture.DescriptorImageInfo(LinearSampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
		BlendSet.Write(PureWhiteTexture.DescriptorImageInfo(LinearSampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2);
		BlendSet.Write(PureBlackTexture.DescriptorImageInfo(LinearSampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3);
		break;
	case 1:
		BlendSet.Write(NMSAttachments[ImageIndex].DescriptorImageInfo(LinearSampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);
		BlendSet.Write(PureWhiteTexture.DescriptorImageInfo(LinearSampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
		BlendSet.Write(PureWhiteTexture.DescriptorImageInfo(LinearSampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2);
		BlendSet.Write(PureBlackTexture.DescriptorImageInfo(LinearSampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3);
		break;
	case 2:
		BlendSet.Write(LineWidthAdaptedAttachments[ImageIndex].DescriptorImageInfo(LinearSampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);
		BlendSet.Write(PureWhiteTexture.DescriptorImageInfo(LinearSampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
		BlendSet.Write(PureWhiteTexture.DescriptorImageInfo(LinearSampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2);
		BlendSet.Write(PureBlackTexture.DescriptorImageInfo(LinearSampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3);
		break;
	case 3:
		BlendSet.Write(PureBlackTexture.DescriptorImageInfo(LinearSampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);
		BlendSet.Write(StipplingAttachments[ImageIndex].DescriptorImageInfo(LinearSampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
		BlendSet.Write(PureWhiteTexture.DescriptorImageInfo(LinearSampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2);
		BlendSet.Write(PureBlackTexture.DescriptorImageInfo(LinearSampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3);
		break;
	case 4:
		BlendSet.Write(LineWidthAdaptedAttachments[ImageIndex].DescriptorImageInfo(LinearSampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);
		BlendSet.Write(StipplingAttachments[ImageIndex].DescriptorImageInfo(LinearSampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
		BlendSet.Write(PaperTexture.DescriptorImageInfo(LinearSampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2);
		BlendSet.Write(BrushTexture.DescriptorImageInfo(LinearSampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3);
		break;
	default:
		break;
	}

}

void Stippling::RecordCommandBuffer()
{
	const CommandBuffer& GraphicCommandBuffer = VulkanPlus::Plus().CommandBuffer_Graphics();
	const uint32_t ImageIndex = VulkanBase::Base().CurrentImageIndex();
	const VkDeviceSize offset = 0;

	PassCommonRenderPass.Begin(GraphicCommandBuffer, EdgeTestFramebuffers[ImageIndex], {{},PictureExtent2D}, {{1.0f}});
	vkCmdBindVertexBuffers(GraphicCommandBuffer, 0, 1, VulkanPlus::Plus().DefaultVertexBuffer().Address(), &offset);
	vkCmdBindPipeline(GraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, EdgeTestPipeline);
	vkCmdBindDescriptorSets(GraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, EdgeTestPipelineLayout, 0, 1,
		EdgeTestSet.Address(), 0, nullptr);
	vkCmdDraw(GraphicCommandBuffer, 4, 1, 0, 0);
	PassCommonRenderPass.End(GraphicCommandBuffer);

	PassCommonRenderPass.Begin(GraphicCommandBuffer, NMSFramebuffers[ImageIndex], { {},PictureExtent2D }, { {1.0f} });
	vkCmdBindVertexBuffers(GraphicCommandBuffer, 0, 1, VulkanPlus::Plus().DefaultVertexBuffer().Address(), &offset);
	vkCmdBindPipeline(GraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, NMSPipeline);
	vkCmdBindDescriptorSets(GraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, NMSPipelineLayout, 0, 1,
		NMSSet.Address(), 0, nullptr);
	vkCmdPushConstants(GraphicCommandBuffer, NMSPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof NMS_PC, &NMS_PC);
	vkCmdDraw(GraphicCommandBuffer, 4, 1, 0, 0);
	PassCommonRenderPass.End(GraphicCommandBuffer);

	PassCommonRenderPass.Begin(GraphicCommandBuffer, LineWidthAdaptedFramebuffers[ImageIndex], { {},PictureExtent2D }, { {1.0f} });
	vkCmdBindVertexBuffers(GraphicCommandBuffer, 0, 1, VulkanPlus::Plus().DefaultVertexBuffer().Address(), &offset);
	vkCmdBindPipeline(GraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, LineWidthAdaptedPipeline);
	vkCmdBindDescriptorSets(GraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, LineWidthAdaptedPipelineLayout, 0, 1,
		LineWidthAdaptedSet.Address(), 0, nullptr);
	vkCmdPushConstants(GraphicCommandBuffer, LineWidthAdaptedPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof LineWidthAdapted_PC, &LineWidthAdapted_PC);
	vkCmdDraw(GraphicCommandBuffer, 4, 1, 0, 0);
	PassCommonRenderPass.End(GraphicCommandBuffer);


	PassCommonRenderPass.Begin(GraphicCommandBuffer, StipplingFramebuffers[ImageIndex], { {},PictureExtent2D }, { {1.0f} });
	vkCmdBindVertexBuffers(GraphicCommandBuffer, 0, 1, VulkanPlus::Plus().DefaultVertexBuffer().Address(), &offset);
	vkCmdBindPipeline(GraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, StipplingPipeline);
	vkCmdBindDescriptorSets(GraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, StipplingPipelineLayout, 0, 1,
		StipplingSet.Address(), 0, nullptr);
	vkCmdPushConstants(GraphicCommandBuffer, StipplingPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof Stippling_PC, &Stippling_PC);
	vkCmdDraw(GraphicCommandBuffer, 4, 1, 0, 0);
	PassCommonRenderPass.End(GraphicCommandBuffer);


	VulkanPlus::Plus().SwapchainRenderPass().Begin(GraphicCommandBuffer, VulkanPlus::Plus().CurrentSwapchainFramebuffer(), { {},VulkanBase::Base().SwapchainCi().imageExtent }, { {1.0f} });
	vkCmdBindVertexBuffers(GraphicCommandBuffer, 0, 1, VulkanPlus::Plus().DefaultVertexBuffer().Address(), &offset);
	vkCmdBindPipeline(GraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, BlendPipeline);
	vkCmdBindDescriptorSets(GraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, BlendPipelineLayout, 0, 1,
		BlendSet.Address(), 0, nullptr);
	vkCmdDraw(GraphicCommandBuffer, 4, 1, 0, 0);
	VulkanPlus::Plus().SwapchainRenderPass().End(GraphicCommandBuffer);
}

void Stippling::SendDataToNextNode()
{

}

void Stippling::ImguiRender()
{
	if (ImGui::TreeNode("NMS-Settings"))
	{
		ImGui::PushItemWidth(200);
		ImGui::SliderFloat("HighThreshold", &NMS_PC.HighThreshold, 0.0f, 1.0f);
		ImGui::SliderFloat("LowThreshold", &NMS_PC.LowThreshold, 0.0f, 1.0f);
		NMS_PC.HighThreshold = std::max(NMS_PC.HighThreshold, NMS_PC.LowThreshold);
		NMS_PC.LowThreshold = std::min(NMS_PC.HighThreshold, NMS_PC.LowThreshold);
		ImGui::PopItemWidth();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("LineWidthAdapted-Settings"))
	{
		ImGui::PushItemWidth(200);
		ImGui::SliderFloat("RadiusFactor", &LineWidthAdapted_PC.radius_factor, 1.0f, 5.0f);
		ImGui::PopItemWidth();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Stippling-Settings"))
	{
		ImGui::PushItemWidth(200);
		ImGui::SliderFloat("UV_Scale", &Stippling_PC.uv_scale, 0.0f, 8.0f);
		ImGui::SliderFloat("LuminanceScale", &Stippling_PC.luminance_scale, 0.0f, 2.0f);
		ImGui::PopItemWidth();
		ImGui::TreePop();
	}

	const char* items[] = { "EdgeDetect", "NMS", "LineWidth", "Stippling", "Blend" }; 
	if (ImGui::TreeNode("PassToPresent")){
		ImGui::PushItemWidth(200);
		ImGui::Combo("###PassToPresent", &PassToPresent, items, IM_ARRAYSIZE(items));
		ImGui::PopItemWidth();
		ImGui::TreePop();
	}
}
