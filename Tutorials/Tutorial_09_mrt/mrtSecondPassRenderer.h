#pragma  once 
#include "SceneRenderTask.h"
#include "GeometryDX11.h"
#include "RenderEffectDX11.h"
#include "AppSettings.h"
#include "MatrixParameterDX11.h"
#include "VectorParameterDX11.h"
#include "ShaderResourceParameterDX11.h"

using namespace Glyph3;

class Glyph3::Entity3D;

class mrtSecondPassRenderer : public SceneRenderTask
{
public:
	mrtSecondPassRenderer(Glyph3::RendererDX11& Renderer);
	virtual ~mrtSecondPassRenderer(){};

	virtual void Update(float fTime);
	virtual void QueuePreTasks(Glyph3::RendererDX11* pRenderer);
	virtual void ExecuteTask(PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager);
	virtual void Resize(UINT width, UINT height);
	virtual void SetRenderParams(IParameterManager* pParamManager);
	virtual std::wstring GetName();

	void SetTargets(std::vector<ResourcePtr>& texTargets, ResourcePtr pRenderTarget,
		ResourcePtr DepthTarget, int Viewport, int vpWidth, int vpHeight);

protected:
	Glyph3::RendererDX11&			m_Renderer;
	ResourcePtr						m_RenderTarget;
	ResourcePtr						m_DepthTarget;
	ShaderResourceParameterDX11*	m_pColTexParams[3];
	std::vector<ResourcePtr>		m_colTex;
};