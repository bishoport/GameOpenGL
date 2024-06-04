#pragma once

#include "../LibCoreHeaders.h"
#include "Viewport.hpp"
#include "FBO.hpp"
#include "../tools/Camera.h"

namespace libCore
{
    class ViewportManager {

    public:

        std::vector<Ref<Viewport>> viewports;

        void CreateViewport(std::string name, glm::vec3 cameraPosition, int viewportWidth, int viewportHeight)
        {
            auto viewport = CreateRef<Viewport>();
            viewport->viewportName = name;
            viewport->viewportSize.x = viewportWidth;
            viewport->viewportSize.y = viewportHeight;

            //G-Buffer
            auto gbo= CreateScope<GBO>();
            gbo->init(viewport->viewportSize.x, viewport->viewportSize.y);
            viewport->gBuffer = std::move(gbo);
            //----------------------------------------------------------

            //F-Buffer DEferred + lighting
            auto fbo0 = CreateScope<FBO>();
            fbo0->init(viewport->viewportSize.x, viewport->viewportSize.y, GL_RGB16);
            viewport->framebuffer_deferred = std::move(fbo0);
            //----------------------------------------------------------

            //F-Buffer Deferred + Forward 
            auto fbo1 = CreateScope<FBO>();
            fbo1->init(viewport->viewportSize.x, viewport->viewportSize.y, GL_RGB8);
            viewport->framebuffer_forward = std::move(fbo1);
            //----------------------------------------------------------



            //F-Buffer Final
            auto fbo2 = CreateScope<FBO>();
            fbo2->init(viewport->viewportSize.x, viewport->viewportSize.y, GL_RGB8);
            viewport->framebuffer_final = std::move(fbo2);
            //----------------------------------------------------------

            //F-Buffer HDR
            auto fbo3 = CreateScope<FBO>();
            fbo3->init(viewport->viewportSize.x, viewport->viewportSize.y, GL_RGB8); //Este buffer tienen una coma flotante más grande porque es para HDR
            viewport->framebuffer_HDR = std::move(fbo3);
            //----------------------------------------------------------

            //Camera
            viewport->camera = CreateScope <libCore::Camera>(viewport->viewportSize.x, viewport->viewportSize.y, cameraPosition);
            viewports.push_back(std::move(viewport));
            //----------------------------------------------------------
        }  
    };
}
