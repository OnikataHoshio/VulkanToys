#ifndef _RENDER_GRAPH_H_
#define _RENDER_GRAPH_H_

#include "EmptyNode.h"

namespace HoshioEngine
{
	enum class RENDER_NODE_TYPE {
		PREFRAME_NODE = 0,
		PRECOMPUTE_NODE = 1
	};

#define ENTER_NODE_ID -1

	class RenderGraph
	{
	private:
		bool hasBeenInit = false;
		std::unique_ptr<RenderNode> preframeNode;
		std::unique_ptr<RenderNode> precomputeNode;
		uint32_t preframeTimestampCounter = 0;
		uint32_t precomputeTimestampCounter = 0;
	public:
		std::vector<std::unique_ptr<RenderNode>> preframeNodeBuffer;
		std::vector<std::unique_ptr<RenderNode>> precomputeNodeBuffer;
		TimestampQueries preframeTimestampQueries;
		TimestampQueries precomputeTimestampQueries;
	public:
		RenderGraph();
		RenderGraph* InitRenderGraph();
		RenderGraph* ConnectRenderNode(uint32_t originalNodeID, uint32_t connectedNodeID, RENDER_NODE_TYPE nodeType);
		//void ConnectRenderNode(std::string originalNodeName, std::string connectedNodeName, RENDER_NODE_TYPE nodeType);
		RenderGraph* DisconnectRenderNode(uint32_t originalNodeID, uint32_t connectedNodeID, RENDER_NODE_TYPE nodeType);
		//void DisconnectRenderNode(std::string originalNodeName, std::string connectedNodeName, RENDER_NODE_TYPE nodeType);
		RenderGraph* Render();
		RenderGraph* ExecutePrecompute();

	private:
		RenderNode* CheckNodeIdRange(uint32_t nodeID, RENDER_NODE_TYPE nodeType) const;
		void Render_Internal();
		void ExecutePrecompute_Internal();
	};

}

#endif // !_RENDER_GRAPH_H_
