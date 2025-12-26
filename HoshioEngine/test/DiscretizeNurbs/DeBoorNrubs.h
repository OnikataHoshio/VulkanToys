#ifndef _DE_BOOR_NURBS_H_
#define _DE_BOOR_NURBS_H_

#include "Engine/ShaderEditor/RenderGraph/RenderNode.h"

using namespace HoshioEngine;

class DeBoorNurbs : public RenderNode{
private:
	struct ControlPoint {
		glm::vec4 posw; // xyz = position, w = weight
	};

	struct InputData {
		ControlPoint baseControlPoints[16] = {
			// i = 0
			glm::vec4{-1.0f, -1.0f, 0.0f, 1.0f},
			glm::vec4{-1.0f, -0.33f, 0.5f, 1.0f},
			glm::vec4{-1.0f,  0.33f, 0.5f, 1.0f},
			glm::vec4{-1.0f,  1.0f, 0.0f, 1.0f},

			// i = 1
			glm::vec4{-0.33f, -1.0f, 0.5f, 1.0f},
			glm::vec4{-0.33f, -0.33f, 1.0f, 1.0f},
			glm::vec4{-0.33f,  0.33f, 1.0f, 1.0f},
			glm::vec4{-0.33f,  1.0f, 0.5f, 1.0f},

			// i = 2
			glm::vec4{ 0.33f, -1.0f, 0.5f, 1.0f},
			glm::vec4{ 0.33f, -0.33f, 1.0f, 1.0f},
			glm::vec4{ 0.33f,  0.33f, 1.0f, 1.0f},
			glm::vec4{ 0.33f,  1.0f, 0.5f, 1.0f},

			// i = 3
			glm::vec4{ 1.0f, -1.0f, 0.0f, 1.0f},
			glm::vec4{1.0f, -0.33f, 0.5f, 1.0f},
			glm::vec4{ 1.0f,  0.33f, 0.5f, 1.0f},
			glm::vec4{ 1.0f,  1.0f, 0.0f, 1.0f},
		};
		float u_knots[24];
		float v_knots[24];
	}inputData;

	struct Meta {
		int n = 3;
		int m = 3;
		int p = 3;
		int q = 3;
	}meta;

	bool update_u_knots = true;
	bool update_v_knots = true;

	uint32_t nurbs_num = 64;

	StorageBuffer output_sbuffer;
	StorageBuffer input_sbuffer;

	DescriptorSetLayout descriptor_set_layout;
	DescriptorSet descriptor_set;

	PipelineLayout pipeline_layout;
	Pipeline pipeline;

	VkDeviceSize output_sbuffer_size = 0;

	std::vector<glm::vec4> retriData;

private:
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

	void PrintKnots(const int max_idx, bool isV);
	void UpdateKnots();

public:
	DeBoorNurbs() = default;
	~DeBoorNurbs() = default;
	void ImguiRender() override;
	void RetrieveData();
};


#endif // !_DE_BOOR_NURBS_H_

