#include "firstPassView.h"

FirstPassView::FirstPassView(RendererDX11& Renderer, ResourcePtr RenderTarget, ResourcePtr DepthTarget)
: ViewPerspective(Renderer, RenderTarget, DepthTarget)
{
	m_pTextureParam = Renderer.m_pParamMgr->GetShaderResourceParameterRef(std::wstring(L"ImageTexture"));
}


FirstPassView::~FirstPassView()
{
}

//void FirstPassView::SetScene(Scene* pScene)
//{
//	m_pScene = pScene;
//}
//
//void FirstPassView::SetEntity(Entity3D* pEntity)
//{
//	m_pEntity = pEntity;
//}
//
//void FirstPassView::SetRenderParams(IParameterManager* pParamManager)
//{
//	//pParamManager->SetShaderResourceParameter((RenderParameterDX11*)m_pTextureParam, m_RenderTarget);
//	
//}
//
//void FirstPassView::SetUsageParams(IParameterManager* pParamManager)
//{
//	pParamManager->SetShaderResourceParameter((RenderParameterDX11*)m_pTextureParam, m_RenderTarget);
//}

void FirstPassView::ExecuteTask(PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager)
{
	// unbind the shader resource view
	ID3D11ShaderResourceView* srvs[1] = { 0 };
	pPipelineManager->GetDeviceContext()->PSSetShaderResources(m_RenderTarget->m_iResourceSRV, 1, srvs);
	ViewPerspective::ExecuteTask(pPipelineManager, pParamManager);
	//SetUsageParams(pParamManager);
}

std::wstring FirstPassView::GetName()
{
	return (L"FirstPassView");
}