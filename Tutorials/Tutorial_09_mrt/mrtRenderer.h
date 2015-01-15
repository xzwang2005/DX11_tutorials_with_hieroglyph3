#pragma once
#include "SceneRenderTask.h"
#include "SpriteRendererDX11.h"
#include "SpriteFontLoaderDX11.h"
#include "mrtFirstPassRenderer.h"
#include "mrtSecondPassRenderer.h"

using namespace Glyph3;

class Glyph3::Entity3D;

class mrtRenderer : public SceneRenderTask
{
public:
	mrtRenderer(RendererDX11& Renderer, ResourcePtr RenderTarget);
	virtual ~mrtRenderer();

	virtual void Update(float fTime);
	virtual void QueuePreTasks(RendererDX11* pRenderer);
	virtual void ExecuteTask(PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager);
	virtual void Resize(UINT width, UINT height);

	//virtual void SetRenderParams(IParameterManager* pParamManager);
	//virtual void SetUsageParams(IParameterManager* pParamManager);

	virtual void SetEntity(Entity3D* pEntity);
	virtual void SetScene(Scene* pScene);

	//virtual void SetViewMatrix(const Matrix4f& matrix);
	//virtual void SetProjMatrix(const Matrix4f& matrix);

	//void SetClipPlanes(float NearClip, float FarClip);
	void SetupViews();

	virtual std::wstring GetName();

protected:
	int							ResolutionX;
	int							ResolutionY;

	ResourcePtr					m_BackBuffer;
	ResourcePtr					m_DepthTarget;
	std::vector<ResourcePtr>	m_firstPassTargets;
	int							m_iViewport;
	ResourcePtr					m_secondPassTarget;
	SpriteRendererDX11			m_SpriteRenderer;
	SpriteFontPtr				m_pFont;

	//ShaderResourceParameterDX11*	m_sr2SecondPass[3];
	mrtFirstPassRenderer*		m_pFirstPassRenderer;
	mrtSecondPassRenderer*		m_pSecondPassRenderer;
};