#pragma once

#include "Renderer.h"

namespace engine
{

class SceneWorld;

class SkyboxRenderer final : public Renderer
{
public:
	using Renderer::Renderer;
	virtual ~SkyboxRenderer();

	virtual void Init() override;
	virtual void UpdateView(const float* pViewMatrix, const float* pProjectionMatrix) override;
	virtual void Render(float deltaTime) override;
	virtual bool IsEnable() const override;

	void SetSceneWorld(SceneWorld* pSceneWorld) { m_pCurrentSceneWorld = pSceneWorld; }

private:
	SceneWorld* m_pCurrentSceneWorld = nullptr;
};

}