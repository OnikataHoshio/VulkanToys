#include "PBRRenderGraph.h"

HoshioEngine::PBRRenderGraph::PBRRenderGraph()
{
	InitialSetting();
}

void HoshioEngine::PBRRenderGraph::InitialSetting()
{
	precomputeNode->LinkNextNode(&pbrPrecomputeNode);
	preframeNode->LinkNextNode(&pbrNode);
	pbrNode.LinkNextNode(&skybox);

	InitRenderGraph();

	{
		Sampler& sampler = VulkanPlus::Plus().GetSampler("trilinear-sampler").second[0];
		pbrNode.sampler_set.Write(pbrPrecomputeNode.preBRDFTexture.DescriptorImageInfo(sampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0);
		pbrNode.sampler_set.Write(pbrPrecomputeNode.kullaContyTexture.DescriptorImageInfo(sampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
		VkImageViewCreateInfo imageViewCreateInfo = {
			.image = pbrPrecomputeNode.envPrefilterAttachment.Image(),
			.viewType = VK_IMAGE_VIEW_TYPE_CUBE,
			.format = VK_FORMAT_R16G16B16A16_SFLOAT,
			.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 5, 0 , 6},
		};

		static ImageView imageView(imageViewCreateInfo);

		VkDescriptorImageInfo descriptorImageInfo =
		{
			.sampler = sampler,
			.imageView = imageView,
			.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		};
		pbrNode.sampler_set.Write(descriptorImageInfo, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2);

		skybox.descriptorSet.Write(pbrPrecomputeNode.envAttachment.DescriptorImageInfo(sampler), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
	}
}
