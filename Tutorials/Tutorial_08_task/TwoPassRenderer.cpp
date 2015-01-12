//#include "PCH.h"
#include "TwoPassRenderer.h"
#include "Texture2dConfigDX11.h"
#include "Entity3D.h"
#include "Scene.h"
#include "firstPassView.h"

using namespace Glyph3;

TwoPassRenderer::TwoPassRenderer(RendererDX11 & Renderer, ResourcePtr RenderTarget)
{
	m_BackBuffer = RenderTarget;
	D3D11_TEXTURE2D_DESC desc = m_BackBuffer->m_pTexture2dConfig->GetTextureDesc();

	ResolutionX = desc.Width;
	ResolutionY = desc.Height;

	// Create render targets
	Texture2dConfigDX11 RTConfig;
	RTConfig.SetColorBuffer(ResolutionX, ResolutionY);
	RTConfig.SetBindFlags(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
	RTConfig.SetFormat(DXGI_FORMAT_R32G32B32A32_FLOAT);
	m_firstPassTarget = Renderer.CreateTexture2D(&RTConfig, NULL);

	Texture2dConfigDX11 DepthConfig;
	DepthConfig.SetDepthBuffer(ResolutionX, ResolutionY);
	m_DepthTarget = Renderer.CreateTexture2D(&DepthConfig, NULL);

	//m_pFirstPassView = new ViewPerspective(Renderer, m_firstPassTarget, m_DepthTarget);
	m_pFirstPassView = new FirstPassView(Renderer, m_firstPassTarget, m_DepthTarget);
	m_pFirstPassView->SetBackColor(Vector4f(0.6f, 0.6f, 0.9f, 1.0f));

	m_pTextureParam = Renderer.m_pParamMgr->GetShaderResourceParameterRef(std::wstring(L"ImageTexture"));
	//m_pTextureParam->SetParameterData(&m_firstPassTarget->m_iResourceSRV);
}


TwoPassRenderer::~TwoPassRenderer()
{
	SAFE_DELETE(m_pFirstPassView);
}

void TwoPassRenderer::Update(float fTime)
{}

void TwoPassRenderer::QueuePreTasks(RendererDX11* pRenderer)
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

	m_pFirstPassView->QueuePreTasks(pRenderer);
}

void TwoPassRenderer::ExecuteTask(PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager)
{
	if (m_pScene)
	{
		// Set the render target for the final pass, and clear it
		pPipelineManager->ClearRenderTargets();
		pPipelineManager->OutputMergerStage.DesiredState.RenderTargetViews.SetState(0, m_BackBuffer->m_iResourceRTV);
		pPipelineManager->ApplyRenderTargets();
		pPipelineManager->ClearBuffers(Vector4f(0.0f, 0.0f, 0.0f, 0.0f));

		// Also bind the depth buffer
		pPipelineManager->OutputMergerStage.DesiredState.DepthTargetViews.SetState(m_DepthTarget->m_iResourceDSV);
		pPipelineManager->ApplyRenderTargets();

		// Configure the desired viewports in this pipeline
		ConfigureViewports(pPipelineManager);

		// Set this view's render parameters
		SetRenderParams(pParamManager);

		// Run through the graph and render each of the entities
		m_pScene->GetRoot()->Render(pPipelineManager, pParamManager, VT_FINALPASS);
	}
}

void TwoPassRenderer::SetRenderParams(IParameterManager* pParamManager)
{
	SceneRenderTask::SetRenderParams(pParamManager);

	pParamManager->SetShaderResourceParameter(m_pTextureParam, m_firstPassTarget);
}

void TwoPassRenderer::Resize(UINT width, UINT height)
{
	ResolutionX = width;
	ResolutionY = height;
	RendererDX11::Get()->ResizeTexture(m_DepthTarget, width, height);
	RendererDX11::Get()->ResizeTexture(m_firstPassTarget, width, height);

	// resize the viewport
	RendererDX11::Get()->ResizeViewport(m_iViewports[0], width, height);

}

void TwoPassRenderer::SetEntity(Glyph3::Entity3D* pEntity)
{
	m_pEntity = pEntity;
	m_pFirstPassView->SetEntity(pEntity);
}

void TwoPassRenderer::SetScene(Scene* pScene)
{
	m_pScene = pScene;
	m_pFirstPassView->SetScene(pScene);
}

std::wstring TwoPassRenderer::GetName()
{
	return(L"TwoPassRenderer");
}