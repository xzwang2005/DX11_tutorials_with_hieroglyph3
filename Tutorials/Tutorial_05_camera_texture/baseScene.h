#pragma once
#include "Application.h"
#include "Win32RenderWindow.h"
#include "RendererDX11.h"

#include "Actor.h"
#include "FullscreenActor.h"
#include "ViewPerspective.h"

#include "GeometryActor.h"

#include "Camera.h"
#include "Scene.h"

using namespace Glyph3;

class base_scene : public Application
{
public:
	base_scene();

public:
	virtual void Initialize();
	virtual void Update();
	virtual void Shutdown();

	virtual bool ConfigureEngineComponents();
	virtual void ShutdownEngineComponents();
	virtual void TakeScreenShot();

	virtual std::wstring GetName();

	RendererDX11*			m_pRenderer11;
	Win32RenderWindow *		m_pWindow;

	int						m_iSwapChain;
	ResourcePtr				m_RenderTarget;
	ResourcePtr				m_DepthTarget;

	ResourcePtr				m_pVertexBuffer;
	int						m_VertexLayout;
	ResourcePtr				m_pIndexBuffer;
	RenderEffectDX11*		m_Effect;

	int						m_iViewPort;

	ViewPerspective*		m_pRenderView;
	Actor*					m_pActor;
	//FullscreenActor*		m_pActor;
	Entity3D*				m_pEntity;

	MaterialPtr				m_pMaterial;
	Vector4f				m_TessParams;

	GeometryPtr				m_pGeometry;
	Camera*					m_pCamera;

	GeometryActor*			m_pGeometryActor;
};