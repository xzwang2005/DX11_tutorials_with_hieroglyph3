#include "firstPassView.h"


firstPassView::firstPassView(RendererDX11& Renderer, ResourcePtr RenderTarget, ResourcePtr DepthTarget = 0)
: ViewPerspective(Renderer, RenderTarget, DepthTarget)
{
}


firstPassView::~firstPassView()
{
}

std::wstring firstPassView::GetName()
{
	return (L"FirstPassView");
}

void firstPassView::SetRenderParams(IParameterManager* pParamManager)
{
	pParamManager->SetViewMatrixParameter(&ViewMatrix);
	pParamManager->SetProjMatrixParameter(&ProjMatrix);
}
