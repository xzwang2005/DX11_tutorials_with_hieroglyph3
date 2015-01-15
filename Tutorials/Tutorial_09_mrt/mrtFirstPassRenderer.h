#pragma once
#include "SceneRenderTask.h"

using namespace Glyph3;

class mrtFirstPassRenderer : public SceneRenderTask
{
public:
	mrtFirstPassRenderer(RendererDX11& Renderer);
	virtual ~mrtFirstPassRenderer();

	virtual void Update(float fTime);
	virtual void QueuePreTasks(RendererDX11* pRenderer);
	virtual void ExecuteTask(PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager);
	virtual void Resize(UINT width, UINT height);

	virtual void SetRenderParams(IParameterManager* pParamManager);

	void SetTargets(std::vector<ResourcePtr>& renderTargets, ResourcePtr DepthTarget,
		int Viewport);

	virtual std::wstring GetName();

protected:
	RendererDX11&				m_Renderer;
	std::vector<ResourcePtr>	m_renderTargets;
	ResourcePtr					m_DepthTarget;
};