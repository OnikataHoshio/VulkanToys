#ifndef _STIPPLING_H_
#define _STIPPLING_H_

#include "Engine/ShaderEditor/RenderGraph/RenderNode.h"

using namespace HoshioEngine;

class Stippling : public RenderNode {
private:
	Texture2D BlueNoiseTexture;
	Texture2D PictureTexture;
	Texture2D PureWhiteTexture;
	Texture2D PureBlackTexture;
	Texture2D PaperTexture;
	Texture2D BrushTexture;
	
	Sampler LinearSampler;
	Sampler NearestSampler;
	Sampler LinearRepeatSampler;

	Pipeline EdgeTestPipeline;
	Pipeline NMSPipeline;
	Pipeline StipplingPipeline;
	Pipeline LineWidthAdaptedPipeline;
	Pipeline BlendPipeline;
	PipelineLayout EdgeTestPipelineLayout;
	PipelineLayout NMSPipelineLayout;
	PipelineLayout LineWidthAdaptedPipelineLayout;
	PipelineLayout StipplingPipelineLayout;
	PipelineLayout BlendPipelineLayout;

	std::vector<ColorAttachment> EdgeTestAttachments;
	std::vector<ColorAttachment> NMSAttachments;
	std::vector<ColorAttachment> StipplingAttachments;
	std::vector<ColorAttachment> LineWidthAdaptedAttachments;

	std::vector<Framebuffer> EdgeTestFramebuffers;
	std::vector<Framebuffer> NMSFramebuffers;
	std::vector<Framebuffer> StipplingFramebuffers;
	std::vector<Framebuffer> LineWidthAdaptedFramebuffers;

	RenderPass PassCommonRenderPass;

	DescriptorSet EdgeTestSet;
	DescriptorSet NMSSet;
	DescriptorSet StipplingSet;
	DescriptorSet LineWidthAdaptedSet;
	DescriptorSet BlendSet;
	DescriptorSetLayout SingleSampler2DSetLayout;
	DescriptorSetLayout DoubleSampler2DSetLayout;
	DescriptorSetLayout BlendSetLayout;

	VkExtent2D PictureExtent2D;

	struct NMSPushConstant {
		float HighThreshold = 1.0;
		float LowThreshold = 0.8;
	}NMS_PC;

	struct LineWidthAdaptedPushConstant {
		float radius_factor = 4.0;
	}LineWidthAdapted_PC;

	struct StipplingPushConstant {
		float uv_scale = 3.0;
		float luminance_scale = 1.1;
	}Stippling_PC;

	int PassToPresent = 4;


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
	Stippling() = default;

	void ImguiRender() override;

};

#endif