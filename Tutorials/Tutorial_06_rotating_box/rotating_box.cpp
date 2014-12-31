#include "rotating_box.h"
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

using namespace Glyph3;

rotate_box AppInstance;

rotate_box::rotate_box(){}

bool rotate_box::ConfigureEngineComponents()
{
	if (!ConfigureRenderingEngineComponents(1024, 640, D3D_FEATURE_LEVEL_11_0)) {
		return(false);
	}

	if (!ConfigureRenderingSetup()) {
		return(false);
	}

	return(true);
}

void rotate_box::ShutdownEngineComponents()
{
	ShutdownRenderingSetup();
	ShutdownRenderingEngineComponents();
}

std::wstring rotate_box::GetName()
{
	return (std::wstring(L"rotating box"));
}

void rotate_box::Shutdown()
{
	std::wstringstream out;
	out << L"Max FPS: " << m_pTimer->MaxFramerate();
	Log::Get().Write(out.str());
}

void rotate_box::Initialize()
{
	SetMultiThreadedMode(false);
	m_pCamera->Spatial().SetTranslation(Vector3f(0.0f, 0.0f, -5.0f));
	m_pRenderView->SetBackColor(Vector4f(0.0f, 0.0f, 0.2f, 1.0f));

	m_pEffect = new RenderEffectDX11();
	m_pEffect->SetVertexShader(m_pRenderer11->LoadShader(VERTEX_SHADER,
		std::wstring(L"tutorial06.hlsl"),
		std::wstring(L"VSMAIN"),
		std::wstring(L"vs_5_0")));
	m_pEffect->SetPixelShader(m_pRenderer11->LoadShader(PIXEL_SHADER,
		std::wstring(L"tutorial06.hlsl"),
		std::wstring(L"PSMAIN"),
		std::wstring(L"ps_5_0")));

	DepthStencilStateConfigDX11 dsConfig;
	int iDepthStencilState = m_pRenderer11->CreateDepthStencilState(&dsConfig);
	if (iDepthStencilState == -1) {
		Log::Get().Write(L"Failed to create light depth stencil state");
		assert(false);
	}

	BlendStateConfigDX11 blendConfig;
	int iBlendState = m_pRenderer11->CreateBlendState(&blendConfig);
	if (iBlendState == -1) {
		Log::Get().Write(L"Failed to create light blend state");
		assert(false);
	}

	RasterizerStateConfigDX11 rsConfig;
	rsConfig.CullMode = D3D11_CULL_BACK;
	int iRasterizerState = m_pRenderer11->CreateRasterizerState(&rsConfig);
	if (iRasterizerState == -1) {
		Log::Get().Write(L"Failed to create rasterizer state");
		assert(false);
	}

	m_pEffect->m_iBlendState = iBlendState;
	m_pEffect->m_iDepthStencilState = iDepthStencilState;
	m_pEffect->m_iRasterizerState = iRasterizerState;
	m_pEffect->m_uStencilRef = iDepthStencilState;

	m_pMaterial = MaterialPtr(new MaterialDX11());
	m_pMaterial->Params[VT_PERSPECTIVE].bRender = true;
	m_pMaterial->Params[VT_PERSPECTIVE].pEffect = m_pEffect;

	m_pGeometryActor = new GeometryActor();
	// manually create the cube
	Vector3f positions[] =
	{
		Vector3f(-1.0f, 1.0f, -1.0f),
		Vector3f(1.0f, 1.0f, -1.0f),
		Vector3f(1.0f, 1.0f, 1.0f),
		Vector3f(-1.0f, 1.0f, 1.0f),
		Vector3f(-1.0f, -1.0f, -1.0f),
		Vector3f(1.0f, -1.0f, -1.0f),
		Vector3f(1.0f, -1.0f, 1.0f),
		Vector3f(-1.0f, -1.0f, 1.0f),
	};

	Vector4f colors[] =
	{
		Vector4f(0.0f, 0.0f, 1.0f, 1.0f),
		Vector4f(0.0f, 1.0f, 0.0f, 1.0f),
		Vector4f(0.0f, 1.0f, 1.0f, 1.0f),
		Vector4f(1.0f, 0.0f, 0.0f, 1.0f),
		Vector4f(1.0f, 0.0f, 1.0f, 1.0f),
		Vector4f(1.0f, 1.0f, 0.0f, 1.0f),
		Vector4f(1.0f, 1.0f, 1.0f, 1.0f),
		Vector4f(0.0f, 0.0f, 0.0f, 1.0f),
	};

	for (size_t i = 0; i < sizeof(positions) / sizeof(Vector3f); i++) {
		m_pGeometryActor->AddVertex(positions[i], colors[i]);
	}

	UINT indices[] =
	{
		3, 1, 0,
		2, 1, 3,

		0, 5, 4,
		1, 5, 0,

		3, 4, 7,
		0, 4, 3,

		1, 6, 5,
		2, 6, 1,

		2, 7, 6,
		3, 7, 2,

		6, 4, 5,
		7, 4, 6,
	};

	for (size_t i = 0; i < sizeof(indices) / sizeof(UINT); i=i+3) {
		m_pGeometryActor->AddIndices(indices[i], indices[i + 1], indices[i + 2]);
	}

	RotationController<Node3D>* pGeometryRotController = new RotationController<Node3D>(Vector3f(0.0f, 1.0f, 0.0f), 0.4f);
	m_pGeometryActor->GetNode()->Controllers.Attach(pGeometryRotController);
	m_pGeometryActor->GetBody()->Visual.SetMaterial(m_pMaterial);

	m_pScene->AddActor(m_pGeometryActor);
}

void rotate_box::Update()
{
	m_pTimer->Update();

	EvtManager.ProcessEvent(EvtFrameStartPtr(new EvtFrameStart(m_pTimer->Elapsed())));

	m_pScene->Update(m_pTimer->Elapsed());
	m_pScene->Render(m_pRenderer11);

	// Perform the rendering and presentation for the window.
	m_pRenderer11->Present(m_pWindow->GetHandle(), m_pWindow->GetSwapChain());
}