#include "Engine/ShaderEditor/RenderGraph/RenderNode.h"

namespace HoshioEngine {
	TimestampQueries* RenderNode::pTimestampQueries = nullptr;

	uint32_t RenderNode::timestampCounter = 0;

	RenderNode& RenderNode::Init(){
		InitResource();
		CreateSampler();
		CreateRenderPass();
		CreateDescriptorSetLayout();
		CreatePipelineLayout();
		CreatePipeline();
		CreateFramebuffers();
		CreateBuffer();
		OtherOperations();
		return *this;
	}

	RenderNode::RenderNode()
	{
	}

	RenderNode* RenderNode::NextNode()
	{
		return next;
	}

	RenderNode& RenderNode::LinkNextNode(RenderNode* node)
	{
		next = node;
		return *node;
	}

	void RenderNode::Render()
	{
		const CommandBuffer& commandBuffer = VulkanPlus::Plus().CommandBuffer_Graphics();

		UpdateDescriptorSets();
		RecordCommandBuffer();

		if (next) {
			SendDataToNextNode();
		}
	}
}