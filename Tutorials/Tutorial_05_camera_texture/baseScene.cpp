#include "baseScene.h"
#include "Log.h"

#include <sstream>
#include <DirectXMath.h>

#include "EventManager.h"
#include "EvtFrameStart.h"
#include "EvtErrorMessage.h"

#include "PipelineManagerDX11.h"

#include "BufferConfigDX11.h"
#include "DepthStencilStateConfigDX11.h"
#include "RasterizerStateConfigDX11.h"
#include "BlendStateConfigDX11.h"
#include "SwapChainConfigDX11.h"
#include "Texture2dConfigDX11.h"

#include "GeometryLoaderDX11.h"
#include "MaterialGeneratorDX11.h"



using namespace Glyph3;

base_scene AppInstance;

base_scene::base_scene()
{}

bool base_scene::ConfigureEngineComponents()
{
	// window size
	int width = 640;
	int height = 480;

	// initialize render window
	m_pWindow = new Win32RenderWindow();
	m_pWindow->SetPosition(100, 100);
	m_pWindow->SetSize(width, height);
	m_pWindow->SetCaption(GetName());
	m_pWindow->Initialize(this);

	// initialize renderer
	m_pRenderer11 = new RendererDX11();

	// create renderer and initialize with only hardware level
	if (!m_pRenderer11->Initialize(D3D_DRIVER_TYPE_HARDWARE, D3D_FEATURE_LEVEL_11_0)) {
		Log::Get().Write(L"Failed to create hardware device");
		return false;
	}

	m_pRenderer11->MultiThreadingConfig.SetConfiguration(false);

	// create a swap chain
	SwapChainConfigDX11 Config;
	Config.SetWidth(m_pWindow->GetWidth());
	Config.SetHeight(m_pWindow->GetHeight());
	Config.SetOutputWindow(m_pWindow->GetHandle());
	m_iSwapChain = m_pRenderer11->CreateSwapChain(&Config);
	m_pWindow->SetSwapChain(m_iSwapChain);

	// keep a copy of the render target
	m_RenderTarget = m_pRenderer11->GetSwapChainResource(m_iSwapChain);

	// create a depth buffer
	Texture2dConfigDX11 depthConfig;
	depthConfig.SetDepthBuffer(width, height);
	m_DepthTarget = m_pRenderer11->CreateTexture2D(&depthConfig, 0);

	// bind the swap chain, render target and the depth buffer
	//PipelineManagerDX11 *pipelineMgr = m_pRenderer11->pImmPipeline;
	m_pRenderer11->pImmPipeline->ClearRenderTargets();
	m_pRenderer11->pImmPipeline->OutputMergerStage.DesiredState.RenderTargetViews.SetState(
		0, m_RenderTarget->m_iResourceRTV);
	m_pRenderer11->pImmPipeline->OutputMergerStage.DesiredState.DepthTargetViews.SetState(
		m_DepthTarget->m_iResourceDSV);
	m_pRenderer11->pImmPipeline->ApplyRenderTargets();

	// initialize shaders
	m_Effect = new RenderEffectDX11();
	m_Effect->SetVertexShader(m_pRenderer11->LoadShader(VERTEX_SHADER,
		std::wstring(L"Tutorial04.hlsl"),
		std::wstring(L"VSMAIN"),
		std::wstring(L"vs_4_0"),
		true));

	m_Effect->SetPixelShader(m_pRenderer11->LoadShader(PIXEL_SHADER,
		std::wstring(L"Tutorial04.hlsl"),
		std::wstring(L"PSMAIN"),
		std::wstring(L"ps_4_0"),
		true));

	//DepthStencilStateConfigDX11 dsConfig;
	//int iDepthStencilState = m_pRenderer11->CreateDepthStencilState(&dsConfig);
	//if (iDepthStencilState == -1) {
	//	Log::Get().Write(L"Failed to create light depth stencil state");
	//	assert(false);
	//}

	//BlendStateConfigDX11 blendConfig;
	//int iBlendState = m_pRenderer11->CreateBlendState(&blendConfig);
	//if (iBlendState == -1) {
	//	Log::Get().Write(L"Failed to create light blend state");
	//	assert(false);
	//}

	//RasterizerStateConfigDX11 rsConfig;
	//rsConfig.CullMode = D3D11_CULL_BACK;
	//int iRasterizerState = m_pRenderer11->CreateRasterizerState(&rsConfig);
	//if (iRasterizerState == -1) {
	//	Log::Get().Write(L"Failed to create rasterizer state");
	//	assert(false);
	//}

	//m_Effect->m_iBlendState = iBlendState;
	//m_Effect->m_iDepthStencilState = iDepthStencilState;
	//m_Effect->m_iRasterizerState = iRasterizerState;
	//m_Effect->m_uStencilRef = iDepthStencilState;
	return true;
}

void base_scene::ShutdownEngineComponents()
{
	if (m_pRenderer11)
	{
		m_pRenderer11->Shutdown();
		delete m_pRenderer11;
	}

	if (m_pWindow)
	{
		m_pWindow->Shutdown();
		delete m_pWindow;
	}
}

void base_scene::Initialize()
{
	// method 1: create a triangle from scratch
	// create the geometry (one triangle)
	m_pGeometry = GeometryPtr(new GeometryDX11());
	if (m_pGeometry == NULL) {
		EventManager::Get()->ProcessEvent(EvtErrorMessagePtr(new EvtErrorMessage(std::wstring(
			L"Attempted to create a triangle on null geometry object."))));
		return;
	}

	VertexElementDX11* vPos = new VertexElementDX11(3, 3);		// three-components and three vertex
	vPos->m_SemanticName = "POSITION";
	vPos->m_uiSemanticIndex = 0;
	vPos->m_Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vPos->m_uiInputSlot = 0;
	vPos->m_uiAlignedByteOffset = 0;
	vPos->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vPos->m_uiInstanceDataStepRate = 0;

	VertexElementDX11* vCol = new VertexElementDX11(4, 3);
	vCol->m_SemanticName = "COLOR";
	vCol->m_uiSemanticIndex = 0;
	vCol->m_Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	vCol->m_uiInputSlot = 0;
	vCol->m_uiAlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vCol->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vCol->m_uiInstanceDataStepRate = 0;

	Vector3f* pPos = vPos->Get3f(0);
	Vector4f* pCol = vCol->Get4f(0);

	pPos[0] = Vector3f(0.0f, 1.0f, 0.0f);
	pPos[1] = Vector3f(-1.0f, -1.0f, 0.0f);
	pPos[2] = Vector3f(1.0f, -1.0f, 0.0f);

	//Vector4f col_green = Vector4f(0.0f, 1.0f, 0.0f, 1.0f);
	pCol[0] = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
	pCol[1] = Vector4f(0.0f, 1.0f, 1.0f, 1.0f);
	pCol[2] = Vector4f(0.0f, 0.0f, 1.0f, 1.0f);

	TriangleIndices face;
	m_pGeometry->AddElement(vPos);
	m_pGeometry->AddElement(vCol);
	face = TriangleIndices(0, 2, 1);
	m_pGeometry->AddFace(face);

	m_pGeometry->GenerateInputLayout(m_Effect->GetVertexShader());
	m_pGeometry->LoadToBuffers();

	m_pMaterial = MaterialPtr(new MaterialDX11());
	m_pMaterial->Params[VT_PERSPECTIVE].bRender = true;
	m_pMaterial->Params[VT_PERSPECTIVE].pEffect = m_Effect;

	m_pEntity = new Entity3D();
	m_pEntity->Visual.SetGeometry(m_pGeometry);
	m_pEntity->Visual.SetMaterial(m_pMaterial);		// must use a pointer here, not a reference
	m_pEntity->Transform.Position() = Vector3f(0.0f, 0.0f, 0.0f);

	m_pActor = new Actor();
	m_pActor->GetNode()->AttachChild(m_pEntity);
	m_pScene->AddActor(m_pActor);

	//// method 2: load an object
	//m_pGeometry = GeometryLoaderDX11::loadStanfordPlyFile(std::wstring(L"spaceship.ply"));
	//m_pGeometry->LoadToBuffers();

	//m_TessParams = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	//m_pRenderer11->m_pParamMgr->SetVectorParameter(std::wstring(L"EdgeFactors"), &m_TessParams);

	//m_pMaterial = MaterialGeneratorDX11::GenerateWireFrame(*m_pRenderer11);


	//m_pEntity = new Entity3D();
	//m_pEntity->Visual.SetGeometry(m_pGeometry);
	//m_pEntity->Visual.SetMaterial(m_pMaterial);
	//m_pEntity->Transform.Position() = Vector3f(0.0f, 0.0f, 0.0f);

	//m_pActor = new Actor();
	//m_pActor->GetNode()->AttachChild(m_pEntity);

	////method 3: create a full screen rectangle
	//m_pActor = new FullscreenActor();
	//Vector4f green(0.0f, 1.0f, 0.0f, 1.0f);
	//m_pActor->SetColor(green);
	//m_pScene->AddActor(m_pActor);

	// create the camera
	m_pCamera = new Camera();
	m_pCamera->Spatial().SetTranslation(Vector3f(0.0f, 0.0f, -15.0f));
	m_pRenderView = new ViewPerspective(*m_pRenderer11, m_RenderTarget, m_DepthTarget);
	m_pRenderView->SetBackColor(Vector4f(0.2f, 0.2f, 0.2f, 1.0f));
	m_pCamera->SetCameraView(m_pRenderView);
	m_pCamera->SetProjectionParams(0.1f, 100.0f, 640.0f / 320.0f, static_cast<float>(GLYPH_PI) / 2.0f);

	m_pScene->AddCamera(m_pCamera);
}

void base_scene::Update()
{
	m_pTimer->Update();

	EvtManager.ProcessEvent(EvtFrameStartPtr(new EvtFrameStart(m_pTimer->Elapsed())));


	// Update the scene, and then render all cameras within the scene.
	m_pScene->Update(m_pTimer->Elapsed());
	m_pScene->Render(m_pRenderer11);

	// Present the results of the rendering to the output window.

	m_pRenderer11->Present(m_pWindow->GetHandle(), m_pWindow->GetSwapChain());
}

void base_scene::Shutdown()
{
	SAFE_DELETE(m_pEntity);

	std::wstringstream out;
	out << L"Max FPS: " << m_pTimer->MaxFramerate();
	Log::Get().Write(out.str());
}

void base_scene::TakeScreenShot()
{
	if (m_bSaveScreenshot)
	{
		m_bSaveScreenshot = false;
		m_pRenderer11->pImmPipeline->SaveTextureScreenShot(0, GetName());
	}
}

std::wstring base_scene::GetName()
{
	return(std::wstring(L"camera and scene"));
}