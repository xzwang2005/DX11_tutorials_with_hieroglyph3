#pragma once
#include "RenderApplication.h"

using namespace Glyph3;

class TaskApp :
	public RenderApplication
{
public:
	TaskApp();
	virtual ~TaskApp();

public:
	virtual bool ConfigureEngineComponents();
	virtual void ShutdownEngineComponents();

	virtual bool ConfigureRenderingSetup();

	virtual void Initialize();
	virtual void Update();
	virtual void Shutdown();

	//virtual bool HandleEvent(EventPtr pEvent);
	virtual std::wstring GetName();

protected:
	RenderEffectDX11*		m_pEffect;
	int						m_iSample;
	GeometryPtr				m_pFullScreen;

	ResourcePtr						m_Texture;
	ShaderResourceParameterDX11*	m_pInputParameter;
	RenderEffectDX11*				m_pEffect01;
	RenderEffectDX11*				m_pEffect02;
	Actor*							m_pActor;
};

