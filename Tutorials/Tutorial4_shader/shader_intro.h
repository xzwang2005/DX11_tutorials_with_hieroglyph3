#include "Application.h"
#include "Win32RenderWindow.h"
#include "RendererDX11.h"

using namespace Glyph3;

class shader_intro : public Application
{
public:
	shader_intro();

public:
	virtual void Initialize();
	virtual void Update();
	virtual void Shutdown();

	virtual bool ConfigureEngineComponents();
	virtual void ShutdownEngineComponents();
	virtual void TakeScreenShot();
	
	//virtual bool HandleEvent(EventPtr pEvent);
	virtual std::wstring GetName();

protected:
	RendererDX11*			m_pRenderer11;
	Win32RenderWindow *		m_pWindow;

	int						m_iSwapChain;
	ResourcePtr				m_RenderTarget;
	ResourcePtr				m_DepthTarget;

	ResourcePtr				m_pVertexBuffer;
	int						m_VertexLayout;
	ResourcePtr				m_pIndexBuffer;
	RenderEffectDX11		m_Effect;

	Matrix4f				m_WorldMat;
	Matrix4f				m_ViewMat;
	Matrix4f				m_ProjMat;
};