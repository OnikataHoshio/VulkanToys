#ifndef _BLINN_PHONG_H_
#define _BLINN_PHONG_H_

#include "Engine//ShaderEditor/RenderGraph/RenderNode.h"
#include "Engine/Actor/Model.h"
#include "Engine/Actor/Light.h"

namespace HoshioEngine
{
	class BlinnPhong : public RenderNode
	{
	private:
		Model dashachun;
		Model floor;
		Light light[2];

		ShaderInfo shader_info;
		ShaderInfo shader_info_floor;

		struct VertexUniform {
			glm::mat4 model = {};
			glm::mat4 view = {};
			glm::mat4 proj = {};
		}vertex_uniform;

		struct FragmentUniform {
			glm::vec4 CameraPos = {};
			glm::vec4 PointLightPos = {};
			glm::vec4 I_p = {};
			glm::vec4 SunLightDir = {};
			glm::vec3 I_s = {};
			float Shininess = 0.0f;
		}fragment_uniform;

		UniformBuffer vertex_uniform_buffer;
		UniformBuffer fragment_uniform_buffer;

		DescriptorSetLayout uniform_set_layout;
		DescriptorSet uniform_set;

		const char* file_path = nullptr;

	public:
		BlinnPhong(const char* file_path);

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
	
		void CreateFloor(float length = 10.0f, float width = 10.0f);

	};
}


#endif // !1
