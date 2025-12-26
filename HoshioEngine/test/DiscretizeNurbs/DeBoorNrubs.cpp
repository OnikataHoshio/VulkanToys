#include "DeBoorNrubs.h"

void DeBoorNurbs::InitResource()
{
}

void DeBoorNurbs::CreateSampler()
{
}

void DeBoorNurbs::CreateBuffer()
{
	input_sbuffer.Create(sizeof inputData)
		.TransferData(&inputData, sizeof inputData);
	output_sbuffer_size = sizeof(glm::vec4) * 20 * 20 * nurbs_num;
	output_sbuffer.Create(output_sbuffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	retriData.resize(20 * 20 * nurbs_num);
}

void DeBoorNurbs::CreateRenderPass()
{
}

void DeBoorNurbs::CreateDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding bindings[2] = {
		{
			.binding = 0,
			.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT
		},
		{
			.binding = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT
		}
	};

	VkDescriptorSetLayoutCreateInfo createInfo = {
		.bindingCount = 2,
		.pBindings = bindings
	};

	descriptor_set_layout.Create(createInfo);
	VulkanPlus::Plus().DescriptorPool().AllocateDescriptorSets(descriptor_set, descriptor_set_layout);
}

void DeBoorNurbs::CreatePipelineLayout()
{
	VkPushConstantRange pushConstantRange = {
		.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
		.offset = 0,
		.size = sizeof meta
	};

	VkPipelineLayoutCreateInfo createInfo = {
		.setLayoutCount = 1,
		.pSetLayouts = descriptor_set_layout.Address(),
		.pushConstantRangeCount = 1,
		.pPushConstantRanges = &pushConstantRange
	};

	pipeline_layout.Create(createInfo);
}

void DeBoorNurbs::CreatePipeline()
{
	static ShaderModule computeModule("test/DiscretizeNurbs/Resource/Shaders/SPIR-V/deboor-nurbs.comp.spv");

	VkPipelineShaderStageCreateInfo shaderStageInfo{};
	shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	shaderStageInfo.module = computeModule;
	shaderStageInfo.pName = "main";

	VkComputePipelineCreateInfo pipelineInfo{};
	pipelineInfo.stage = shaderStageInfo;
	pipelineInfo.layout = pipeline_layout;

	pipeline.Create(pipelineInfo);
}

void DeBoorNurbs::CreateFramebuffers()
{

}

void DeBoorNurbs::OtherOperations()
{
	VkDescriptorBufferInfo input_sbuffer_Info = {
		.buffer = input_sbuffer,
		.offset = 0,
		.range = sizeof inputData
	};

	descriptor_set.Write(input_sbuffer_Info, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0);

	VkDescriptorBufferInfo output_sbuffer_Info = {
		.buffer = output_sbuffer,
		.offset = 0,
		.range = output_sbuffer_size
	};

	descriptor_set.Write(output_sbuffer_Info, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1);
}

void DeBoorNurbs::UpdateDescriptorSets()
{
	if (update_u_knots || update_v_knots)
	{
		UpdateKnots();
		input_sbuffer.TransferData(&inputData, sizeof inputData);
	}
}

void DeBoorNurbs::RecordCommandBuffer()
{
	const CommandBuffer& commandBuffer = VulkanPlus::Plus().CommandBuffer_Graphics();
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline_layout, 0, 1, descriptor_set.Address(), 0, nullptr);
	vkCmdPushConstants(commandBuffer, pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof meta, &meta);
	vkCmdDispatch(commandBuffer, nurbs_num, 1, 1);

	
}

void DeBoorNurbs::SendDataToNextNode()
{

}

void DeBoorNurbs::PrintKnots(const int max_idx, bool isV)
{
	std::cout << std::endl << (isV ? "v_knots updated!" : "u_knots updated!") << std::endl;
	for (int i = 0; i <= max_idx; i++) {
		std::cout << (isV ? inputData.v_knots[i] : inputData.u_knots[i]) << " ";
	}
	std::cout << std::endl;
}

void DeBoorNurbs::UpdateKnots()
{
	if (update_u_knots) {
		const int knots_max_idx = meta.n + meta.p + 1;
		for (int i = 0; i <= meta.p; i++) {
			inputData.u_knots[i] = 0.0f;
		}
		for (int i = knots_max_idx; i >= knots_max_idx - meta.p; i--)
		{
			inputData.u_knots[i] = 1.0f;
		}
		float step = 1.0f / (knots_max_idx - (2 * (meta.p + 1)) + 2);
		for (int i = meta.p + 1; i < knots_max_idx - meta.p; i++) {
			inputData.u_knots[i] = (i - meta.p) * step;
		}
		PrintKnots(knots_max_idx, false);
	}

	if (update_v_knots) {
		const int knots_max_idx = meta.m + meta.q + 1;
		for (int i = 0; i <= meta.q; i++) {
			inputData.v_knots[i] = 0.0f;
		}
		for (int i = knots_max_idx; i >= knots_max_idx - meta.q; i--)
		{
			inputData.v_knots[i] = 1.0f;
		}
		float step = 1.0f / (knots_max_idx - (2 * (meta.q + 1)) + 2);
		for (int i = meta.q + 1; i < knots_max_idx - meta.q; i++) {
			inputData.v_knots[i] = (i - meta.q) * step;
		}
		PrintKnots(knots_max_idx, true);
	}

	update_u_knots = false;
	update_v_knots = false;
}

void DeBoorNurbs::ImguiRender()
{
}

void DeBoorNurbs::RetrieveData()
{
	output_sbuffer.RetrieveData(retriData.data(), output_sbuffer_size);
}

