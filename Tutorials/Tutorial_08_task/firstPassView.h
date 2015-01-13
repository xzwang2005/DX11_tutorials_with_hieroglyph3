#pragma once
//#include "SceneRenderTask.h"
#include "ViewPerspective.h"

using namespace Glyph3;

class Glyph3::Entity3D;

class FirstPassView :
	public ViewPerspective
{
public:
	FirstPassView(RendererDX11& Renderer, ResourcePtr RenderTarget, ResourcePtr DepthTarget = 0);
	virtual ~FirstPassView();

	virtual void ExecuteTask(PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager);

	virtual std::wstring GetName();
};

