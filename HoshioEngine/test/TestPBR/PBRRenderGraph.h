#ifndef _PBR_RENDER_GRAPH_H_
#define _PBR_RENDER_GRAPH_H_

#include "Engine/ShaderEditor/RenderGraph/RenderGraph.h"
#include "PBR.h"
#include "PBRPrecompute.h"
#include "Skybox.h"

namespace HoshioEngine
{
	class PBRRenderGraph : public RenderGraph
	{
	public:
		PBRPrecomputeNode pbrPrecomputeNode;
		PBR pbrNode;
		Skybox skybox;
	public:
		PBRRenderGraph();

	private:
		void InitialSetting();
	};
}

#endif // !_PBR_RENDER_GRAPH_H_

