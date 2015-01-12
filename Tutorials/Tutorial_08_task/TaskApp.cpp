#include "TaskApp.h"
#include "Log.h"

#include "EventManager.h"
#include "EvtFrameStart.h"

#include "SwapChainConfigDX11.h"
#include "Texture2dConfigDX11.h"

#include "GeometryLoaderDX11.h"
#include "GeometryGeneratorDX11.h"
#include "MaterialGeneratorDX11.h"
#include "FirstPersonCamera.h"

#include "DepthStencilViewConfigDX11.h"
#include "ShaderResourceViewConfigDX11.h"
#include "SamplerStateConfigDX11.h"

#include "TwoPassRenderer.h"

using namespace Glyph3;

TaskApp AppInstance;

TaskApp::TaskApp()
{
}


TaskApp::~TaskApp()
{
}


bool TaskApp::ConfigureEngineComponents()
{
	if (!ConfigureRenderingEngineComponents(800, 480, D3D_FEATURE_LEVEL_11_0)) {
		return(false);
	}

	if (!ConfigureRenderingSetup()) {
		return(false);
	}

	SetMultiThreadedMode(false);

	return(true);
}

bool TaskApp::ConfigureRenderingSetup()
{
	// Create the camera, and the render view that will produce an image of the 
	// from the camera's point of view of the scene.

	TwoPassRenderer* pTwoPassView = new TwoPassRenderer(*m_pRenderer11, m_BackBuffer);
	m_pRenderView = pTwoPassView;

	m_pTextOverlayView = new ViewTextOverlay(*m_pRenderer11, m_BackBuffer);


	m_pCamera = new Camera();
	m_pCamera->Spatial().SetTranslation(Vector3f(0.0f, 0.0f, -4.0f));

	m_pCamera->SetCameraView(m_pRenderView);
	m_pCamera->SetOverlayView(m_pTextOverlayView);

	D3D11_TEXTURE2D_DESC desc = m_BackBuffer->m_pTexture2dConfig->GetTextureDesc();
	//m_pCamera->SetProjectionParams(nearClip, farClip, static_cast<float>(m_iWidth) / static_cast<float>(m_iHeight), static_cast<float>(GLYPH_PI) / 2.0f);
	m_pCamera->SetOrthographicParams(0.1f, 1000.0f, (float)desc.Width, (float)desc.Height);

	m_pScene->AddCamera(m_pCamera);

	return(true);
}

void TaskApp::ShutdownEngineComponents()
{
	ShutdownRenderingSetup();
	ShutdownRenderingEngineComponents();
}

void TaskApp::Shutdown()
{
	// Print the framerate out for the log before shutting down.

	std::wstringstream out;
	out << L"Max FPS: " << m_pTimer->MaxFramerate();
	Log::Get().Write(out.str());
}

std::wstring TaskApp::GetName()
{
	return(std::wstring(L"subclassTask"));
}

void TaskApp::Update()
{
	// Update the timer to determine the elapsed time since last frame.  This can
	// then used for animation during the frame.

	m_pTimer->Update();

	// Send an event to everyone that a new frame has started.  This will be used
	// in later examples for using the material system with render views.

	EvtManager.ProcessEvent(EvtFrameStartPtr(new EvtFrameStart(m_pTimer->Elapsed())));

	// Update the scene, and then render all cameras within the scene.
	m_pScene->Update(m_pTimer->Elapsed());
	m_pScene->Render(m_pRenderer11);


	// Perform the rendering and presentation for the window.
	m_pRenderer11->Present(m_pWindow->GetHandle(), m_pWindow->GetSwapChain());
}

void TaskApp::Initialize()
{
	m_Texture = m_pRenderer11->LoadTexture(L"Outcrop.png");

	m_pEffect01 = new RenderEffectDX11();
	m_pEffect01->SetVertexShader(m_pRenderer11->LoadShader(VERTEX_SHADER,
		std::wstring(L"Tutorial07.hlsl"),
		std::wstring(L"VSMAIN"),
		std::wstring(L"vs_5_0")));

	m_pEffect01->SetPixelShader(m_pRenderer11->LoadShader(PIXEL_SHADER,
		std::wstring(L"Tutorial07.hlsl"),
		std::wstring(L"PSMAIN"),
		std::wstring(L"ps_5_0")));

	m_pEffect02 = new RenderEffectDX11();
	m_pEffect02->SetVertexShader(m_pRenderer11->LoadShader(VERTEX_SHADER,
		std::wstring(L"Tutorial07.hlsl"),
		std::wstring(L"VSMAIN"),
		std::wstring(L"vs_5_0")));

	m_pEffect02->SetPixelShader(m_pRenderer11->LoadShader(PIXEL_SHADER,
		std::wstring(L"Tutorial07.hlsl"),
		std::wstring(L"PSFINAL"),
		std::wstring(L"ps_5_0")));

	SamplerStateConfigDX11 SamplerConfig;
	m_iSample = RendererDX11::Get()->CreateSamplerState(&SamplerConfig);
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
	pMaterial->Params[VT_PERSPECTIVE].pEffect = m_pEffect01;
	pMaterial->Params[VT_FINALPASS].bRender = true;
	pMaterial->Params[VT_FINALPASS].pEffect = m_pEffect02;

	// set the texture parameters
	//pMaterial->Parameters.SetShaderResourceParameter(L"InputMap", m_Texture);

	m_pActor = new Actor();
	m_pActor->GetBody()->Visual.SetGeometry(m_pFullScreen);
	m_pActor->GetBody()->Visual.SetMaterial(pMaterial);
	m_pScene->AddActor(m_pActor);

	m_pCamera->Spatial().SetTranslation(Vector3f(0.0f, 0.0f, -5.0f));
	m_pCamera->SetOrthographicParams(0.1f, 10.0f, 1024.0f, 640.0f);
	m_pRenderView->SetBackColor(Vector4f(0.2f, 0.2f, 0.2f, 0.2f));

	m_pInputParameter = m_pRenderer11->m_pParamMgr->GetShaderResourceParameterRef(std::wstring(L"SampleTexture"));
	m_pInputParameter->InitializeParameterData(&m_Texture->m_iResourceSRV);
}