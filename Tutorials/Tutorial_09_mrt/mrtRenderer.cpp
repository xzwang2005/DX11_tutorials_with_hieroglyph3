#include "PCH.h"
#include "mrtRenderer.h"
#include "Entity3D.h"
#include "Scene.h"
#include "Texture2dConfigDX11.h"
#include "Log.h"
#include "ActorGenerator.h"
#include "IParameterManager.h"
#include "DepthStencilViewConfigDX11.h"
#include "ShaderResourceViewConfigDX11.h"
#include "AppSettings.h"

using namespace Glyph3;

mrtRenderer::mrtRenderer(RendererDX11& Renderer, ResourcePtr RenderTarget)
{
	m_BackBuffer = RenderTarget;

	D3D11_TEXTURE2D_DESC desc = m_BackBuffer->m_pTexture2dConfig->GetTextureDesc();

	ResolutionX = desc.Width;
	ResolutionY = desc.Height;

	// create all render targets for the first pass
	Texture2dConfigDX11 RTConfig;
	RTConfig.SetColorBuffer(ResolutionX, ResolutionY);
	RTConfig.SetBindFlags(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
	RTConfig.SetFormat(DXGI_FORMAT_R32G32B32A32_FLOAT);
	for (int i = 0; i <  4; i++)
	{
		m_firstPassTargets.push_back(Renderer.CreateTexture2D(&RTConfig, NULL));
	}

	m_secondPassTarget = Renderer.CreateTexture2D(&RTConfig, NULL);

	// create depth target for the first pass, unlike render target, 
	// one depth target is enough
	Texture2dConfigDX11 DepthConfig;
	DepthConfig.SetDepthBuffer(ResolutionX, ResolutionY);
	m_DepthTarget = Renderer.CreateTexture2D(&DepthConfig, NULL);

	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<float>(ResolutionX);
	viewport.Height = static_cast<float>(ResolutionY);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	m_iViewport = Renderer.CreateViewPort(viewport);

	// create the first pass sub-view
	m_pFirstPassRenderer = new mrtFirstPassRenderer(Renderer);
	m_pSecondPassRenderer = new mrtSecondPassRenderer(Renderer);

	m_SpriteRenderer.Initialize();
}

mrtRenderer::~mrtRenderer() 
{
	SAFE_DELETE(m_pFirstPassRenderer);
}

void mrtRenderer::Update(float fTime){}

void mrtRenderer::QueuePreTasks(RendererDX11* pRenderer)
{
	// Call the super class's predraw in order to queue it in the renderer.  The
	// views are processed in a LIFO order, so this will be executed last in both
	// single- or multi-threaded mode.

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

	// Next we call the predraw method of each of the supporting views.

	SetupViews();
	m_pSecondPassRenderer->QueuePreTasks(pRenderer);
	m_pFirstPassRenderer->QueuePreTasks(pRenderer);
}

void mrtRenderer::ExecuteTask(PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager)
{
	// Bind the back buffer and clear it
	pPipelineManager->ClearRenderTargets();
	pPipelineManager->OutputMergerStage.DesiredState.RenderTargetViews.SetState(0, m_BackBuffer->m_iResourceRTV);
	pPipelineManager->ApplyRenderTargets();
	pPipelineManager->ClearBuffers(Vector4f(0.0f, 0.0f, 0.0f, 0.0f));

	// Configure the desired viewports in this pipeline
	ConfigureViewports(pPipelineManager);

	if (DisplayMode::Value == DisplayMode::FinalPass) {
		m_SpriteRenderer.Render(pPipelineManager, pParamManager, m_secondPassTarget, Matrix4f::ScaleMatrix(1.0f));
	}
	else {
		Matrix4f mat = Matrix4f::Identity();
		mat = Matrix4f::ScaleMatrix(0.5f)*Matrix4f::TranslationMatrix(0, 0, 0);
		m_SpriteRenderer.Render(pPipelineManager, pParamManager, m_firstPassTargets[0], mat);

		mat = Matrix4f::ScaleMatrix(0.5f)*Matrix4f::TranslationMatrix(ResolutionX/2, 0, 0);
		m_SpriteRenderer.Render(pPipelineManager, pParamManager, m_firstPassTargets[1], mat);

		mat = Matrix4f::ScaleMatrix(0.5f)*Matrix4f::TranslationMatrix(0, ResolutionY/2, 0);
		m_SpriteRenderer.Render(pPipelineManager, pParamManager, m_firstPassTargets[2], mat);

		mat = Matrix4f::ScaleMatrix(0.5f)*Matrix4f::TranslationMatrix(ResolutionX/2, ResolutionY/2, 0);
		m_SpriteRenderer.Render(pPipelineManager, pParamManager, m_firstPassTargets[3], mat);
	}
}

void mrtRenderer::SetScene(Scene* pScene)
{
	m_pFirstPassRenderer->SetScene(pScene);
	m_pSecondPassRenderer->SetScene(pScene);
}

void mrtRenderer::SetEntity(Entity3D* pEntity)
{
	m_pFirstPassRenderer->SetEntity(pEntity);
	m_pSecondPassRenderer->SetEntity(pEntity);
}

void mrtRenderer::SetupViews()
{
	SetViewPort(m_iViewport);
	m_pFirstPassRenderer->SetTargets(m_firstPassTargets, m_DepthTarget, m_iViewport);
	m_pSecondPassRenderer->SetTargets(m_firstPassTargets, m_secondPassTarget, m_DepthTarget, m_iViewport, ResolutionX, ResolutionY);
}

void mrtRenderer::Resize(UINT width, UINT height)
{
	RendererDX11* pRenderer = RendererDX11::Get();

	// Remember the new dimensions of the render view.
	ResolutionX = width;
	ResolutionY = height;

	for (UINT i = 0; i < m_firstPassTargets.size(); ++i)
	{
		pRenderer->ResizeTexture(m_firstPassTargets[i], ResolutionX, ResolutionY);
	}
	pRenderer->ResizeTexture(m_secondPassTarget, ResolutionX, ResolutionY);
	pRenderer->ResizeTexture(m_DepthTarget, ResolutionX, ResolutionY);

	pRenderer->ResizeViewport(m_iViewport, ResolutionX, ResolutionY);
	m_pFirstPassRenderer->Resize(width, height);
	m_pSecondPassRenderer->Resize(width, height);
}

std::wstring mrtRenderer::GetName()
{
	return(L"mrtRenderer");
}