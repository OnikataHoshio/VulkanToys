#ifndef _PBR_PRECOMPUTE_H_
#define _PBR_PRECOMPUTE_H_

#include "Engine/ShaderEditor/RenderGraph/RenderGraph.h"

namespace HoshioEngine {
	class PBRPrecomputeNode : public RenderNode
	{
	public:
		CubeAttachment envAttachment;
		CubeAttachment envPrefilterAttachment;
		ColorAttachment preBRDFTexture;
		Texture2D hdrImage;

	private:
		struct CubemapUniform {
			glm::mat4 model = {};
			glm::mat4 view = {};
			glm::mat4 proj = {};
		}vertex_uniform;

		Sampler sampler;

		VertexBuffer vertex_buffer;
		UniformBuffer uniform_buffer;

		DescriptorSetLayout descriptor_set_layout;
		DescriptorSet descriptor_set;

	public:
		PBRPrecomputeNode() = default;
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

		void CmdTransferHDRIToCubeMap();

		void CmdEnvPrefilter();

		void CmdPrecomputeBRDF();
	};
}



#endif // !_PBR_PRECOMPUTE_H_

