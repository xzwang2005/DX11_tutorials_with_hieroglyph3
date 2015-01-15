#include "mrtFirstPassRenderer.h"
#include "Entity3D.h"
#include "Scene.h"
#include "Texture2dConfigDX11.h"
#include "Log.h"
#include "IParameterManager.h"
#include "PipelineManagerDX11.h"
#include "Texture2dDX11.h"

using namespace Glyph3;

mrtFirstPassRenderer::mrtFirstPassRenderer(RendererDX11& Renderer)
: m_Renderer(Renderer)
{
	ViewMatrix.MakeIdentity();
	ProjMatrix.MakeIdentity();
}

mrtFirstPassRenderer::~mrtFirstPassRenderer() {}

void mrtFirstPassRenderer::Update(float fTime){}

void mrtFirstPassRenderer::QueuePreTasks(RendererDX11* pRenderer)
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
		m_pScene->GetRoot()->PreRender(pRenderer, VT_PERSPECTIVE);
	}
}

void mrtFirstPassRenderer::ExecuteTask(PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager)
{
	if (m_pScene)
	{
		// Set the parameters for rendering this view
		pPipelineManager->ClearRenderTargets();
		for (unsigned int i = 0; i < m_renderTargets.size(); ++i)
			pPipelineManager->OutputMergerStage.DesiredState.RenderTargetViews.SetState(i, m_renderTargets[i]->m_iResourceRTV);
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
		m_pScene->GetRoot()->Render(pPipelineManager, pParamManager, VT_PERSPECTIVE);
	}
}

void mrtFirstPassRenderer::Resize(UINT width, UINT height)
{
	//
}

void mrtFirstPassRenderer::SetRenderParams(IParameterManager* pParamManager)
{
	pParamManager->SetViewMatrixParameter(&ViewMatrix);
	pParamManager->SetProjMatrixParameter(&ProjMatrix);
}

void mrtFirstPassRenderer::SetTargets(std::vector<ResourcePtr>& renderTargets, ResourcePtr DepthTarget, int Viewport)
{
	m_renderTargets = renderTargets;
	SetViewPort(Viewport);
	m_DepthTarget = DepthTarget;
}

std::wstring mrtFirstPassRenderer::GetName()
{
	return(L"mrtFirstPass");
}