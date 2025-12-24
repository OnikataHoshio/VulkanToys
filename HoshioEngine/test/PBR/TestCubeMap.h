#ifndef _TEST_CUBE_MAP_H_
#define _TEST_CUBE_MAP_H_

#include "Engine/ShaderEditor/RenderGraph/RenderNode.h"
#include "Plus/ImageManager.h"
using namespace HoshioEngine;

class TestCubeMap : public RenderNode {
public:
	int pipeline_id = M_INVALID_ID;
	int pipeline_layout_id = M_INVALID_ID;
	int renderpass_id = M_INVALID_ID;
	int sampler_id = M_INVALID_ID;
	int descriptor_set_layout_id = M_INVALID_ID;
	int dsAttachments_id = M_INVALID_ID;
	int framebuffers_id = M_INVALID_ID;

	int cubemap_id = M_INVALID_ID;
	int hdri_id = M_INVALID_ID;

	CubeAttachment cubeAttachment;
	CubeAttachment envAttachment;
	CubeAttachment envPrefilterAttachment;

	struct CubemapUniform {
		glm::mat4 model = {};
		glm::mat4 view = {};
		glm::mat4 proj = {};
	}cubemap_uniform;

	VertexBuffer vertex_buffer;
	UniformBuffer uniform_buffer;
	DescriptorSet descriptor_set;

	TestCubeMap() = default;

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

	void CmdCopyCubeMap();

	void CmdTransferHDRIToCubeMap();

	void CmdEnvPrefilter();

	//------------
};


#endif // !_TEST_CUBE_MAP_H_
