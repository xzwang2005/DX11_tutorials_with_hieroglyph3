#include "firstPassView.h"

FirstPassView::FirstPassView(RendererDX11& Renderer, ResourcePtr RenderTarget, ResourcePtr DepthTarget)
: ViewPerspective(Renderer, RenderTarget, DepthTarget)
{
}


FirstPassView::~FirstPassView()
{
}

void FirstPassView::ExecuteTask(PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager)
{
	// unbind the shader resource view
	pPipelineManager->ClearPipelineState();
	ViewPerspective::ExecuteTask(pPipelineManager, pParamManager);
}

std::wstring FirstPassView::GetName()
{
	return (L"FirstPassView");
}