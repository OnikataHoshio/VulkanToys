#ifndef _BLINN_PHONG_H_
#define _BLINN_PHONG_H_

#include "Engine//ShaderEditor/RenderGraph/RenderNode.h"
#include "Engine/Actor/Model.h"
#include "Engine/Actor/Light.h"

namespace HoshioEngine
{
	class BlinnPhong : public RenderNode
	{
	public:

	private:
		Model model;
		Light light[2];

		RenderPass renderPass;
		Pipeline pipeline;
		PipelineLayout pipelineLayout;
		Sampler sampler;
		DescriptorSet descriptorSet;
		DescriptorSetLayout descriptorSetLayout;




	public:
		BlinnPhong() = default;

		void ImguiRender() override;

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
	
	};
}


#endif // !1
