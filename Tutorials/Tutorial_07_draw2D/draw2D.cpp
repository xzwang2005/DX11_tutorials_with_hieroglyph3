#include "draw2D.h"
#include "Log.h"

#include <sstream>
#include "RotationController.h"
#include "EvtFrameStart.h"

#include "BufferConfigDX11.h"
#include "DepthStencilStateConfigDX11.h"
#include "RasterizerStateConfigDX11.h"
#include "BlendStateConfigDX11.h"
#include "SwapChainConfigDX11.h"
#include "Texture2dConfigDX11.h"

#include "GeometryGeneratorDX11.h"
#include "SamplerStateConfigDX11.h"

using namespace Glyph3;

draw2D AppInstance;

draw2D::draw2D(){}

bool draw2D::ConfigureEngineComponents()
{
	if (!ConfigureRenderingEngineComponents(640, 480, D3D_FEATURE_LEVEL_11_0)) {
		return(false);
	}

	if (!ConfigureRenderingSetup()) {
		return(false);
	}

	return(true);
}

void draw2D::ShutdownEngineComponents()
{
	ShutdownRenderingSetup();
	ShutdownRenderingEngineComponents();
}

std::wstring draw2D::GetName()
{
	return (std::wstring(L"Draw 2D image"));
}

void draw2D::Shutdown()
{
	std::wstringstream out;
	out << L"Max FPS: " << m_pTimer->MaxFramerate();
	Log::Get().Write(out.str());
}

void draw2D::Update()
{
	m_pTimer->Update();

	EvtManager.ProcessEvent(EvtFrameStartPtr(new EvtFrameStart(m_pTimer->Elapsed())));

	m_pScene->Update(m_pTimer->Elapsed());
	m_pScene->Render(m_pRenderer11);
	//m_pRenderer11->pImmPipeline->Draw(*m_pEffect, m_pFullScreen, m_pRenderer11->m_pParamMgr);

	// Perform the rendering and presentation for the window.
	m_pRenderer11->Present(m_pWindow->GetHandle(), m_pWindow->GetSwapChain());
}

void draw2D::Initialize()
{
	m_Texture = m_pRenderer11->LoadTexture(L"Outcrop.png");

	m_pEffect = new RenderEffectDX11();
	m_pEffect->SetVertexShader(m_pRenderer11->LoadShader(VERTEX_SHADER,
		std::wstring(L"Tutorial07.hlsl"),
		std::wstring(L"VSMAIN"),
		std::wstring(L"vs_5_0")));

	m_pEffect->SetPixelShader(m_pRenderer11->LoadShader(PIXEL_SHADER,
		std::wstring(L"Tutorial07.hlsl"),
		std::wstring(L"PSMAIN"),
		std::wstring(L"ps_5_0")));

	SamplerStateConfigDX11 SamplerConfig;
	m_iSample = RendererDX11::Get()->CreateSamplerState(&SamplerConfig);

	// disable depth stencil since we are rendering 2d images
	DepthStencilStateConfigDX11 ds;
	ds.DepthEnable = false;
	m_pEffect->m_iDepthStencilState = RendererDX11::Get()->CreateDepthStencilState(&ds);

	SamplerParameterDX11* pSamplerParameter =
		RendererDX11::Get()->m_pParamMgr->GetSamplerStateParameterRef(std::wstring(L"LinearSampler"));
	pSamplerParameter->InitializeParameterData(&m_iSample);

	// Create a full screen quad for rendering the texture to the backbuffer.
	m_pFullScreen = GeometryPtr(new GeometryDX11());
	GeometryGeneratorDX11::GenerateFullScreenQuad(m_pFullScreen);

	//m_pFullScreen->GenerateInputLayout(m_pEffect->GetVertexShader());
	m_pFullScreen->LoadToBuffers();

	MaterialPtr pMaterial = MaterialPtr(new MaterialDX11());

	pMaterial->Params[VT_PERSPECTIVE].bRender = true;
	pMaterial->Params[VT_PERSPECTIVE].pEffect = m_pEffect;

	m_pActor = new Actor();
	m_pActor->GetBody()->Visual.SetGeometry(m_pFullScreen);
	m_pActor->GetBody()->Visual.SetMaterial(pMaterial);
	m_pScene->AddActor(m_pActor);

	m_pCamera->Spatial().SetTranslation(Vector3f(0.0f, 0.0f, -5.0f));
	m_pCamera->SetOrthographicParams(0.1f, 10.0f, 1024.0f, 640.0f);
	m_pRenderView->SetBackColor(Vector4f(0.2f, 0.2f, 0.2f, 0.2f));

	//// method 1)  not working
	//m_pRenderer11->m_pParamMgr->SetShaderResourceParameter(L"SampleTexture", m_Texture);
	//m_pRenderer11->pImmPipeline->ApplyPipelineResources();		// when should I call this?

	// method 2)
	m_pInputParameter = m_pRenderer11->m_pParamMgr->GetShaderResourceParameterRef(std::wstring(L"SampleTexture"));
	m_pInputParameter->InitializeParameterData(&m_Texture->m_iResourceSRV);

	//InvalidateRect(m_pWindow->GetHandle(), NULL, TRUE);
}