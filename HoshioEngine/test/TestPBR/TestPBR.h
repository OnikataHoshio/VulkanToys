#ifndef _TEST_PBR_H_
#define _TEST_PBR_H_

#include "Engine/ShaderEditor/RenderGraph/RenderNode.h"
#include "Engine/Actor/Model.h"
#include "Engine/Actor/Light.h"

using namespace HoshioEngine;

class TestPBR : public RenderNode {
private:
	Model sphere;
	Light light;

	ShaderInfo shader_info;
	int dsAttachments_id = M_INVALID_ID;
	int framebuffers_id = M_INVALID_ID;

	struct VertexUniform {
		glm::mat4 model = {};
		glm::mat4 view = {};
		glm::mat4 proj = {};
	}vertex_uniform;

	struct LightUniform {
		glm::vec4 lightDir = glm::vec4(-3.0f, -3.0f, 0.0f, 1.0f);
		glm::vec4 lightRadiance = glm::vec4(10.0f);

		glm::vec4 albedo = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	}light_uniform;

	struct SphereUniform {
		float roughness;
		float metallic;
	}sphere_uniform[5][5];

	UniformBuffer vertex_uniform_buffer;
	UniformBuffer light_uniform_buffer;
	UniformBuffer sphere_uniform_buffer;

	int uniform_set_layout_id = M_INVALID_ID;
	DescriptorSet uniform_set;

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

	void CreateSphere(uint32_t segments = 32, uint32_t rings = 16, float radius = 1);

public:
	TestPBR() = default;
	void ImguiRender() override;
};


#endif // !_PBR_H_

