#include "Engine/ShaderEditor/RenderGraph/RenderGraph.h"

namespace HoshioEngine
{
	RenderGraph::RenderGraph()
	{
		preframeNode = std::make_unique<EmptyNode>();
		precomputeNode = std::make_unique<EmptyNode>();
	}

	RenderGraph* RenderGraph::InitRenderGraph()
	{
		if (!hasBeenInit)
		{
			RenderNode* renderNode = preframeNode.get();
			while (renderNode != nullptr)
			{
				renderNode->Init();
				renderNode = renderNode->NextNode();
			}

			renderNode = precomputeNode.get();
			while (renderNode != nullptr)
			{
				renderNode->Init();
				renderNode = renderNode->NextNode();
			}

			CreateTimestampQueries();

			hasBeenInit = true;
		}
		return this;
	}

	/*
	RenderGraph* RenderGraph::ConnectRenderNode(uint32_t originalNodeID, uint32_t connectedNodeID, RENDER_NODE_TYPE nodeType)
	{
		RenderNode* originalNode = CheckNodeIdRange(originalNodeID, nodeType);
		RenderNode* connectedNode = CheckNodeIdRange(connectedNodeID, nodeType);

		if (originalNode == nullptr || connectedNode == nullptr)
			return this;

		originalNode->LinkNextNode(connectedNode);

		return this;
	}

	RenderGraph* RenderGraph::DisconnectRenderNode(uint32_t originalNodeID, uint32_t connectedNodeID, RENDER_NODE_TYPE nodeType)
	{
		RenderNode* originalNode = CheckNodeIdRange(originalNodeID, nodeType);
		RenderNode* connectedNode = CheckNodeIdRange(connectedNodeID, nodeType);

		if (originalNode == nullptr || connectedNode == nullptr)
			return this;

		RenderNode* nextNode = connectedNode->NextNode();
		originalNode->LinkNextNode(nextNode);

		return this;
	}


	RenderNode* RenderGraph::CheckNodeIdRange(uint32_t nodeID, RENDER_NODE_TYPE nodeType) const
	{
		if (nodeType == RENDER_NODE_TYPE::PREFRAME_NODE)
		{
			const size_t RENDER_NODE_SIZE = preframeNodeBuffer.size();
			if (nodeID >= RENDER_NODE_SIZE)
			{
				std::cout << std::format("[RenderGraph]WARNNING::Node ID is larger than the max ID!");
				return nullptr;
			}
			if (nodeID == ENTER_NODE_ID)
				return preframeNode.get();
			return preframeNodeBuffer[nodeID].get();
		}
		else if (nodeType == RENDER_NODE_TYPE::PRECOMPUTE_NODE)
		{
			const size_t RENDER_NODE_SIZE = precomputeNodeBuffer.size();
			if (nodeID >= RENDER_NODE_SIZE)
			{
				std::cout << std::format("[RenderGraph]WARNNING::Node ID is larger than the max ID!");
				return nullptr;
			}
			if (nodeID == ENTER_NODE_ID)
				return precomputeNode.get();
			return precomputeNodeBuffer[nodeID].get();
		}
	}
	*/

	RenderGraph* RenderGraph::Render()
	{
		const CommandBuffer& commandBuffer = VulkanPlus::Plus().CommandBuffer_Graphics();
		preframeTimestampQueries.Reset(commandBuffer);
		preframeTimestampCounter = 0;

		Render_Internal();

		preframeTimestampQueries.WriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, preframeTimestampCounter++);

		return this;
	}

	RenderGraph* RenderGraph::ExecutePrecompute()
	{
		const CommandBuffer& commandBuffer = VulkanPlus::Plus().CommandBuffer_Graphics();
		//precomputeTimestampQueries.Reset(commandBuffer);
		//precomputeTimestampCounter = 0;

		ExecutePrecompute_Internal();

		//precomputeTimestampQueries.WriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, precomputeTimestampCounter++);
		return this;
	}

	void RenderGraph::Render_Internal()
	{
		const CommandBuffer& commandBuffer = VulkanPlus::Plus().CommandBuffer_Graphics();

		RenderNode* node = preframeNode->NextNode();

		while (node != nullptr)
		{
			preframeTimestampQueries.WriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, preframeTimestampCounter++);

			node->Render();

			node = node->NextNode();
		}
	}

	void RenderGraph::ExecutePrecompute_Internal()
	{
		const CommandBuffer& commandBuffer = VulkanPlus::Plus().CommandBuffer_Graphics();

		RenderNode* node = precomputeNode->NextNode();

		while (node != nullptr)
		{
		//  precomputeTimestampQueries.WriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, precomputeTimestampCounter++);

			node->Render();

			node = node->NextNode();
		}
	}

	void RenderGraph::CreateTimestampQueries()
	{
		preframeTimestampQueries.Create(16);
		precomputeTimestampQueries.Create(16);
	}

}