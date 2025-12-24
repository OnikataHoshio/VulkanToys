#ifndef _TEST_TESSELATION_H_
#define _TEST_TESSELATION_H_

#include "Engine/ShaderEditor/RenderGraph/RenderNode.h"

using namespace HoshioEngine;

class TestTesselation : public RenderNode
{
private:
	
	const float tessVertices[12] = {
		-0.5f, 0.0f,
		0.5f, 0.0f,
		0.0f, 0.5f,
		-0.5f,-0.1f,
		0.0f,-0.6f,
		0.5f,-0.1f
	};
	
	Sampler sampler;
	VertexBuffer vertex_buffer;
	Pipeline pipeline;
	PipelineLayout pipelineLayout;


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

	
public:
	TestTesselation() = default;
	void ImguiRender() override;


};




#endif
