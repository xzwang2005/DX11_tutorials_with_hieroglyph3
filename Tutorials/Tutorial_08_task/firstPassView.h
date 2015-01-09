#pragma once
//#include "SceneRenderTask.h"
#include "ViewPerspective.h"

using namespace Glyph3;

class Glyph3::Entity3D;

class firstPassView :
	public ViewPerspective
{
public:
	firstPassView(RendererDX11& Renderer, ResourcePtr RenderTarget, ResourcePtr DepthTarget = 0);
	virtual ~firstPassView();

	virtual void SetRenderParams(IParameterManager* pParamManager);

	virtual std::wstring GetName();


};

