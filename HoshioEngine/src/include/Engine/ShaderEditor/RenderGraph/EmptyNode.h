#ifndef _EMPTY_NODE_H_
#define _EMPTY_NODE_H_

#include "RenderNode.h"

namespace HoshioEngine
{
	class EmptyNode : public RenderNode
	{
	public:
		EmptyNode() = default;

	private:
		// Í¨¹ý RenderNode ¼Ì³Ð
		void InitResource() override;

		void CreateSampler() override;

		void CreateBuffer() override;

		void CreateRenderPass() override;

		void CreateDescriptorSetLayout() override;

		void CreatePipelineLayout() override;

		void CreatePipeline() override;

		void CreateFramebuffers() override;

		void OtherOperations() override;

		void UpdateDescriptorSets() override;

		void RecordCommandBuffer() override;

		void SendDataToNextNode() override;

		void ImguiRender() override;

	};
}

#endif // !_EMPTY_NODE_H_
