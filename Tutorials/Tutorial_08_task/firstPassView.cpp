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
	// unbind the shader resource view, it is not necessary if the renderer is running in multithreading mode (default)
	//pPipelineManager->ClearPipelineState();
	ViewPerspective::ExecuteTask(pPipelineManager, pParamManager);
}

std::wstring FirstPassView::GetName()
{
	return (L"FirstPassView");
}