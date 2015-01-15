#pragma once
#include "RenderApplication.h"
#include "mrtRenderer.h"

using namespace Glyph3;

class mrtApp :
	public RenderApplication
{
public:
	mrtApp();
	virtual ~mrtApp();

public:
	virtual bool ConfigureEngineComponents();
	virtual void ShutdownEngineComponents();

	virtual bool ConfigureRenderingSetup();

	virtual void Initialize();
	virtual void Update();
	virtual void Shutdown();

	virtual bool HandleEvent(EventPtr pEvent);

	//virtual bool HandleEvent(EventPtr pEvent);
	virtual std::wstring GetName();

	void DrawHUD();

protected:
	RenderEffectDX11*		m_pEffect;
	RenderEffectDX11*		m_pFinalEffect;
	int						m_iSample;
	GeometryPtr				m_pFullScreen;

	ResourcePtr						m_Texture;
	Actor*							m_pActor;

	mrtRenderer*			m_pMainRenderer;
};