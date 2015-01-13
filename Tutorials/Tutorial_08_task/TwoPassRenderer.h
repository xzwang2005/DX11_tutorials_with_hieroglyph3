#pragma once
#include "SceneRenderTask.h"
#include "ViewPerspective.h"

using namespace Glyph3;

class Glyph3::Entity3D;

class TwoPassRenderer :
	public SceneRenderTask
{
public:
	TwoPassRenderer(RendererDX11 & Renderer, ResourcePtr RenderTarget);
	virtual ~TwoPassRenderer();

	virtual void Update(float fTime);
	virtual void QueuePreTasks(RendererDX11* pRenderer);
	virtual void ExecuteTask(PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager);
	virtual void Resize(UINT width, UINT height);
	virtual void SetRenderParams(IParameterManager* pParamManager);
	//virtual void SetUsageParams(IParameterManager* pParamManager);

	virtual void SetEntity(Glyph3::Entity3D* pEntity);
	virtual void SetScene(Scene* pScene);

	virtual std::wstring GetName();

protected:
	int						ResolutionX;
	int						ResolutionY;
	float					m_fNearClip;
	float					m_fFarClip;

	ResourcePtr				m_BackBuffer;
	ResourcePtr				m_DepthTarget;
	ResourcePtr				m_firstPassTarget;

	ViewPerspective*		m_pFirstPassView;
	ShaderResourceParameterDX11*	m_pTextureParam;
	ShaderResourceParameterDX11*	m_pOutputBuffer;
};

