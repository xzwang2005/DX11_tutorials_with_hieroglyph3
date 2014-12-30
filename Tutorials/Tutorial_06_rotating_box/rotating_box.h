#include "RenderApplication.h"

#include "GeometryActor.h"
#include "GeometryDX11.h"
#include "MaterialDX11.h"
#include "Camera.h"
#include "Scene.h"

#include "ShaderResourceParameterDX11.h"
#include "UnorderedAccessParameterDX11.h"
#include "VectorParameterDX11.h"
#include "SamplerParameterDX11.h"
#include "Vector4f.h"

using namespace Glyph3;

class rotate_box : public RenderApplication
{
public:
	rotate_box();

public:
	virtual bool ConfigureEngineComponents();
	virtual void ShutdownEngineComponents();

	virtual void Initialize();
	virtual void Update();
	virtual void Shutdown();

	virtual std::wstring GetName();

protected:
	RenderEffectDX11*		m_pEffect;
	MaterialPtr				m_pMaterial;
	GeometryActor*			m_pGeometryActor;
};