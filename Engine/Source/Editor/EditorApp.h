#pragma once

#include "Application/IApplication.h"

#include <memory>
#include <vector>

namespace engine
{

class CameraController;
class FlybyCamera;
class ImGuiBaseLayer;
class ImGuiContextInstance;
class Window;
class RenderContext;
class Renderer;
class RenderTarget;
class SceneWorld;

}

struct ImGuiContext;

namespace editor
{

class EditorImGuiViewport;
class SceneView;

class EditorApp final : public engine::IApplication
{
public:
	EditorApp();
	EditorApp(const EditorApp&) = delete;
	EditorApp& operator=(const EditorApp&) = delete;
	EditorApp(EditorApp&&) = default;
	EditorApp& operator=(EditorApp&&) = default;
	virtual ~EditorApp();

	virtual void Init(engine::EngineInitArgs initArgs) override;
	virtual bool Update(float deltaTime) override;
	virtual void Shutdown() override;

	engine::Window* GetWindow(size_t index) const;
	engine::Window* GetMainWindow() const { return GetWindow(0); }
	size_t AddWindow(std::unique_ptr<engine::Window> pWindow);
	void RemoveWindow(size_t index);

	void InitRenderContext(engine::GraphicsBackend backend, void* hwnd = nullptr);
	void InitEditorRenderers();
	void InitEngineRenderers();
	void InitShaderPrograms() const;
	void AddEditorRenderer(std::unique_ptr<engine::Renderer> pRenderer);
	void AddEngineRenderer(std::unique_ptr<engine::Renderer> pRenderer);

	void InitEditorImGuiContext(engine::Language language);
	void InitEditorUILayers();
	void InitEngineImGuiContext(engine::Language language);
	void InitEngineUILayers();
	void InitImGuiViewports(engine::RenderContext* pRenderContext);
	void RegisterImGuiUserData(engine::ImGuiContextInstance* pImGuiContext);

	void InitECWorld();
	void InitEditorController();

	bool IsAtmosphericScatteringEnable() const;

private:
	void InitEditorCameraEntity();
	void InitDDGIEntity();
	void InitSkyEntity();

	bool m_bInitEditor = false;
	engine::EngineInitArgs m_initArgs;

	// Windows
	std::vector<std::unique_ptr<engine::Window>> m_pAllWindows;

	// ImGui
	std::unique_ptr<engine::ImGuiContextInstance> m_pEditorImGuiContext;
	std::unique_ptr<engine::ImGuiContextInstance> m_pEngineImGuiContext;
	std::unique_ptr<EditorImGuiViewport> m_pEditorImGuiViewport;

	// Scene
	std::unique_ptr<engine::SceneWorld> m_pSceneWorld;
	editor::SceneView* m_pSceneView = nullptr;
	engine::Renderer* m_pSceneRenderer = nullptr;
	engine::Renderer* m_pDebugRenderer = nullptr;
	engine::Renderer* m_pPBRSkyRenderer = nullptr;
	engine::Renderer* m_pIBLSkyRenderer = nullptr;

	// Rendering
	std::unique_ptr<engine::RenderContext> m_pRenderContext;
	std::vector<std::unique_ptr<engine::Renderer>> m_pEditorRenderers;
	std::vector<std::unique_ptr<engine::Renderer>> m_pEngineRenderers;

	// Controllers for processing input events.
	std::unique_ptr<engine::CameraController> m_pViewportCameraController;
};

}