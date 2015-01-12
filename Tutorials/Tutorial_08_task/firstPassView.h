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

	//virtual void Update(float fTime);
	//virtual void QueuePreTasks(RendererDX11* pRenderer);
	//virtual void ExecuteTask(PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager);
	//virtual void Resize(UINT width, UINT height);

	virtual void SetEntity(Entity3D* pEntity);
	virtual void SetScene(Scene* pScene);

	virtual void SetRenderParams(IParameterManager* pParamManager);

	virtual std::wstring GetName();

protected:
	ShaderResourceParameterDX11*			m_pTextureParam;
};

