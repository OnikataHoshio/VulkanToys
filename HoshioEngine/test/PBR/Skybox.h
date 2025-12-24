#ifndef _SKY_BOX_H_
#define _SKY_BOX_H_

#include "Engine/ShaderEditor/RenderGraph/RenderNode.h"

namespace HoshioEngine
{
	class Skybox : public RenderNode
	{
	public:
		DescriptorSet descriptorSet;
	private:
		RenderPass renderPass;
		Pipeline pipeline;
		PipelineLayout pipelineLayout;
		Sampler sampler;
		DescriptorSetLayout descriptorSetLayout;
		int sampler_id = M_INVALID_ID;

		struct SkyboxUniform {
			glm::mat4 model = {};
			glm::mat4 view = {};
			glm::mat4 proj = {};
		}skybox_uniform;

		VertexBuffer vertex_buffer;
		UniformBuffer uniform_buffer;

	public:
		Skybox() = default;

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



#endif // !_SKY_BOX_H_
