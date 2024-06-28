#pragma once

#include "../LibCoreHeaders.h"

#include "../Core/FBO.hpp"
#include "../tools/EventManager.h"
#include "../input/InputManager.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_stdlib.h"
#include "imgui/ImGuizmo.h"
#include "../Timestep.h"
#include "../tools/Camera.h"
#include "Viewport.hpp"
#include "../StraightSkeleton/Vector2d.h"
namespace libCore
{
    class GuiLayer {

    public:

        using CallbackFromGuiLayer = std::function<void(const std::vector<Vector2d>&, const std::vector<Vector2d>&)>;

        GuiLayer(GLFWwindow* window, float window_W, float window_H);
        void SetCallBackFunc(CallbackFromGuiLayer callbackFromGuiLayerFunc);
        ~GuiLayer();

        void begin();
        void end();
        void renderDockers();
        void renderMainMenuBar();

        //Viewports
        void DrawViewports(std::vector<Ref<Viewport>> viewports);

		//Panels
		void DrawHierarchyPanel(const std::vector<Ref<libCore::ModelContainer>>& modelsInScene);
        void GizmosBasicButtons(float width, float height);
        void SelectCurrentGizmoObject(const std::vector<Ref<libCore::ModelContainer>>& modelsInScene, libCore::Camera camera);
        void DrawGizmos(entt::entity& entity, libCore::Camera camera, Ref<libCore::ModelContainer> modelContainer);
        void DrawLightsPanel(const std::vector<Ref<libCore::Light>>& lightsInScene);

        void RenderCheckerMatrix();

    private:
        ImGuizmo::OPERATION currentOperation = ImGuizmo::OPERATION::TRANSLATE;
      /*  Ref<IGizmo> gizmo;
        Ref<IGizmo> gizmoMove, gizmoRotate, gizmoScale;*/
        CallbackFromGuiLayer m_callbackFromGuiLayerFunc;
        std::vector<std::pair<int, int>> selectedOrder;
        bool ini_file_exists;
        float m_window_W = 0.0f;
        float m_window_H = 0.0f;
        float snappingGizmo;
        struct Point {
            float x, y;
        };
    };	
}