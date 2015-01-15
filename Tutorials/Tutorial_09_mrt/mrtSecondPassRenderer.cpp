#include "mrtSecondPassRenderer.h"
#include "Entity3D.h"
#include "Scene.h"
#include "Texture2dConfigDX11.h"
#include "Log.h"
#include "IParameterManager.h"
#include "PipelineManagerDX11.h"
#include "Texture2dDX11.h"

mrtSecondPassRenderer::mrtSecondPassRenderer(RendererDX11& Renderer)
: m_Renderer(Renderer)
{
	ViewMatrix.MakeIdentity();
	ProjMatrix.MakeIdentity();

	// get ref to textures to be used in the shader
	m_pColTexParams[0] = Renderer.m_pParamMgr->GetShaderResourceParameterRef(std::wstring(L"RedTexture"));
	m_pColTexParams[1] = Renderer.m_pParamMgr->GetShaderResourceParameterRef(std::wstring(L"GreenTexture"));
	m_pColTexParams[2] = Renderer.m_pParamMgr->GetShaderResourceParameterRef(std::wstring(L"BlueTexture"));
}

void mrtSecondPassRenderer::Update(float fTime){};

void mrtSecondPassRenderer::QueuePreTasks(RendererDX11* pRenderer)
{
	if (m_pEntity != NULL)
	{
		Matrix4f view = m_pEntity->Transform.GetView();
		SetViewMatrix(view);
	}

	// Queue this view into the renderer for processing.
	pRenderer->QueueTask(this);

	if (m_pScene)
	{
		// Run through the graph and pre-render the entities
		m_pScene->GetRoot()->PreRender(pRenderer, VT_FINALPASS);
	}
}

void mrtSecondPassRenderer::ExecuteTask(PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager)
{
	if (m_pScene)
	{
		// Set the parameters for rendering this view
		pPipelineManager->ClearRenderTargets();
		pPipelineManager->OutputMergerStage.DesiredState.RenderTargetViews.SetState(0, m_RenderTarget->m_iResourceRTV);
		pPipelineManager->OutputMergerStage.DesiredState.DepthTargetViews.SetState(m_DepthTarget->m_iResourceDSV);
		pPipelineManager->ApplyRenderTargets();

		// Configure the desired viewports in this pipeline
		ConfigureViewports(pPipelineManager);

		// Clear the G-Buffer targets
		Vector4f color(0.0f, 0.0f, 0.0f, 0.0f);
		pPipelineManager->ClearBuffers(color, 1.0f, 0);

		// Set this view's render parameters
		SetRenderParams(pParamManager);

		// Run through the graph and render each of the entities
		m_pScene->GetRoot()->Render(pPipelineManager, pParamManager, VT_FINALPASS);
	}
}

void mrtSecondPassRenderer::Resize(UINT width, UINT height)
{

}

void mrtSecondPassRenderer::SetRenderParams(IParameterManager* pParamManager)
{
	pParamManager->SetViewMatrixParameter(&ViewMatrix);
	pParamManager->SetProjMatrixParameter(&ProjMatrix);

	for (UINT i = 0; i < m_colTex.size() - 1; ++i)		// don't need to bind the last buffer
		pParamManager->SetShaderResourceParameter(m_pColTexParams[i], m_colTex[i]);
}

void mrtSecondPassRenderer::SetTargets(std::vector<ResourcePtr>& texTargets, 
	ResourcePtr pRenderTarget, ResourcePtr DepthTarget, 
	int Viewport, int vpWidth, int vpHeight)
{
	m_colTex = texTargets;
	m_RenderTarget = pRenderTarget;
	m_DepthTarget = DepthTarget;
	SetViewPort(Viewport);

}

std::wstring mrtSecondPassRenderer::GetName()
{
	return (L"mrtSecondPass");
}