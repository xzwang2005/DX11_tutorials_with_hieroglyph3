#include "shader_intro.h"
#include "Log.h"

#include <sstream>
#include <DirectXMath.h>

#include "EventManager.h"
#include "EvtFrameStart.h"

#include "PipelineManagerDX11.h"

#include "BufferConfigDX11.h"
#include "DepthStencilStateConfigDX11.h"
#include "RasterizerStateConfigDX11.h"
#include "BlendStateConfigDX11.h"
#include "SwapChainConfigDX11.h"
#include "Texture2dConfigDX11.h"

using namespace Glyph3;
/*
 *	Structure for vertex buffer
 */
struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT4 Color;
};

shader_intro AppInstance;

shader_intro::shader_intro()
{
}

bool shader_intro::ConfigureEngineComponents()
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
	m_Effect.SetVertexShader(m_pRenderer11->LoadShader(VERTEX_SHADER,
		std::wstring(L"tutorial04.hlsl"),
		std::wstring(L"VSMAIN"),
		std::wstring(L"vs_4_0"),
		true));

	m_Effect.SetPixelShader(m_pRenderer11->LoadShader(PIXEL_SHADER,
		std::wstring(L"tutorial04.hlsl"),
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

	//m_Effect.m_iBlendState = iBlendState;
	//m_Effect.m_iDepthStencilState = iDepthStencilState;
	//m_Effect.m_iRasterizerState = iRasterizerState;
	//m_Effect.m_uStencilRef = iDepthStencilState;

	// create vertex buffer layout
	D3D11_INPUT_ELEMENT_DESC desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	std::vector<D3D11_INPUT_ELEMENT_DESC> layout;
	layout.push_back(desc[0]);
	layout.push_back(desc[1]);
	m_VertexLayout = m_pRenderer11->CreateInputLayout(layout, m_Effect.GetVertexShader());
	if (m_VertexLayout == -1) {
		Log::Get().Write(L"Failed to create vertex layout");
		assert(false);
	}

	// create viewport
	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	int ViewPort = m_pRenderer11->CreateViewPort(viewport);
	m_pRenderer11->pImmPipeline->RasterizerStage.DesiredState.ViewportCount.SetState(1);
	m_pRenderer11->pImmPipeline->RasterizerStage.DesiredState.Viewports.SetState(0, ViewPort);

	return(true);
}

void shader_intro::ShutdownEngineComponents()
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

void shader_intro::Initialize()
{
	{
		Vertex vertices[] =
		{
			{ DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
			{ DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
			{ DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
		};

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = vertices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		BufferConfigDX11 vbConfig;
		vbConfig.SetDefaultVertexBuffer(3 * sizeof(Vertex), false);
		m_pVertexBuffer = m_pRenderer11->CreateVertexBuffer(&vbConfig, &data);
		if (m_pVertexBuffer->m_iResource == -1) {
			Log::Get().Write(L"Failed to create vertex buffer");
			assert(false);
		}
	}

	{
		// create the index buffer resource
		UINT indices[] = { 0, 1, 2 };
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = indices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		BufferConfigDX11 vbConfig;
		vbConfig.SetDefaultIndexBuffer(sizeof(UINT)* 3, false);
		m_pIndexBuffer = m_pRenderer11->CreateIndexBuffer(&vbConfig, &data);
		if (m_pIndexBuffer->m_iResource == -1) {
			Log::Get().Write(L"Failed to create index buffer");
			assert(false);
		}
	}

	// set the viewpoint (camera)
	m_WorldMat = Matrix4f::Identity();
	DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.0f, 1.0f, -100.0f, 0.0f);
	DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, 0.5f, 0.0f, 0.0f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_ViewMat = *((Matrix4f*)&DirectX::XMMatrixLookAtLH(eye, at, up));

	m_ProjMat = *((Matrix4f*)&DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XM_PIDIV2, 
		(FLOAT)m_pWindow->GetWidth() / (FLOAT)m_pWindow->GetHeight(), 
		0.01f, 100.0f));

	m_pRenderer11->m_pParamMgr->SetWorldMatrixParameter(&m_WorldMat);
	m_pRenderer11->m_pParamMgr->SetViewMatrixParameter(&m_ViewMat);
	m_pRenderer11->m_pParamMgr->SetProjMatrixParameter(&m_ProjMat);
}

void shader_intro::Update()
{
	// Update the timer to determine the elapsed time since last frame.  This can 
	// then used for animation during the frame.

	m_pTimer->Update();

	// Send an event to everyone that a new frame has started.  This will be used
	// in later examples for using the material system with render views.

	EvtManager.ProcessEvent(EvtFrameStartPtr(new EvtFrameStart(m_pTimer->Elapsed())));

	// Clear the window
	m_pRenderer11->pImmPipeline->ClearBuffers(Vector4f(0.0f, 0.0f, 0.0f, 0.0f), 1.0f);

	// Perform the actual drawing

	UINT stride = sizeof(Vertex);
	m_pRenderer11->pImmPipeline->Draw(
		m_Effect, m_pVertexBuffer, 
		m_pIndexBuffer, m_VertexLayout, 
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 
		stride, 3, m_pRenderer11->m_pParamMgr);

	// Present the results
	m_pRenderer11->Present(m_pWindow->GetHandle(), m_pWindow->GetSwapChain());
}

void shader_intro::Shutdown()
{
	// Print the framerate out for the log before shutting down.

	std::wstringstream out;
	out << L"Max FPS: " << m_pTimer->MaxFramerate();
	Log::Get().Write(out.str());
}

void shader_intro::TakeScreenShot()
{
	if (m_bSaveScreenshot)
	{
		m_bSaveScreenshot = false;
		m_pRenderer11->pImmPipeline->SaveTextureScreenShot(0, GetName());
	}
}

std::wstring shader_intro::GetName()
{
	return(std::wstring(L"shader and HLSL"));
}