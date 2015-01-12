#include "firstPassView.h"

FirstPassView::FirstPassView(RendererDX11& Renderer, ResourcePtr RenderTarget, ResourcePtr DepthTarget)
: ViewPerspective(Renderer, RenderTarget, DepthTarget)
{
	m_pTextureParam = Renderer.m_pParamMgr->GetShaderResourceParameterRef(std::wstring(L"SampleTexture"));
}


FirstPassView::~FirstPassView()
{
}

void FirstPassView::SetScene(Scene* pScene)
{
	m_pScene = pScene;
}

void FirstPassView::SetEntity(Entity3D* pEntity)
{
	m_pEntity = pEntity;
}

void FirstPassView::SetRenderParams(IParameterManager* pParamManager)
{
	pParamManager->SetShaderResourceParameter((RenderParameterDX11*)m_pTextureParam, m_RenderTarget);
}

std::wstring FirstPassView::GetName()
{
	return (L"FirstPassView");
}