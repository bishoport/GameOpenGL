#include "EngineOpenGL.h"
#include "../input/InputManager.h"

#include "GuiLayer.h"
#include "ViewportManager.hpp"
#include "../tools/RoofGenerator.hpp"

namespace libCore
{
	//MANAGERS
	Scope<GuiLayer> guiLayer = nullptr; //esto se declara en el cpp porque si se pone en el h, hay errores de includes cíclicos
	Scope<ViewportManager> viewportManager = nullptr; //esto se declara en el cpp porque si se pone en el h, hay errores de includes cíclicos
	Scope<RoofGenerator> roofGenerator = nullptr;

	

	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		EventManager::OnWindowResizeEvent().trigger(width, height);
	}


	//--INIT & LIFE CYCLE
	bool EngineOpenGL::InitializeEngine(const std::string& windowTitle,
		int initialWindowWidth,
		int initialWindowHeight,
		MainLoopFnc mainLoodFnc,
		CloseGlFnc closeGlFnc,
		ImGUILoopFnc imGUILoopFnc)
	{
		windowWidth = initialWindowWidth;
		windowHeight = initialWindowHeight;

		g_mainLoodFnc = mainLoodFnc;
		g_closeGlFnc = closeGlFnc;

		/* Initialize the library */
		if (!glfwInit())
		{
			return false;
		}

		//--GLFW: initialize and configure 
		glfwInit();
		const char* glsl_version = "#version 460";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
		// ------------------------------


		//--GLFW window creation
		window = glfwCreateWindow(initialWindowWidth, initialWindowHeight, windowTitle.c_str(), NULL, NULL);
		if (window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return false;
		}
		glfwMakeContextCurrent(window);
		SetupInputCallbacks();
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		//-------------------------------------------------


		//--Glad: load all OpenGL function pointers
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return false;
		}
		//-------------------------------------------------


		// -- SHADERS
		std::string shadersDirectory = "assets/shaders/";
		shaderManager.setShaderDataLoad("basic", shadersDirectory + "basic.vert", shadersDirectory + "basic.frag");
		shaderManager.setShaderDataLoad("colorQuadFBO", shadersDirectory + "quad_fbo.vert", shadersDirectory + "color_quad_fbo.frag");
		shaderManager.setShaderDataLoad("debug", shadersDirectory + "debug.vert", shadersDirectory + "debug.frag");
		shaderManager.setShaderDataLoad("text", shadersDirectory + "text.vert", shadersDirectory + "text.frag");

		// -- G_Buffer
		shaderManager.setShaderDataLoad("shaderGeometryPass", shadersDirectory + "deferred/g_buffer.vert", shadersDirectory + "deferred/g_buffer.frag");
		shaderManager.setShaderDataLoad("shaderLightingPass", shadersDirectory + "deferred/deferred_lighting_pass.vert", shadersDirectory + "deferred/deferred_lighting_pass.frag");
		shaderManager.setShaderDataLoad("combinePass", shadersDirectory + "combine.vert", shadersDirectory + "combine.frag");

		shaderManager.LoadAllShaders();
		//-----------------------------------------------------------------



		// --IMGUI
		if (imGUILoopFnc && useImGUI == true)
		{
			guiLayer = CreateScope<GuiLayer>(window, windowWidth, windowHeight);
			guiLayer->SetCallBackFunc([this](const std::vector<Vector2d>& points, const std::vector<Vector2d>& holePoints) {
				this->CreateRoof(points, holePoints);
				});
			g_imGUILoopFnc = imGUILoopFnc;
		}
		// -------------------------------------------------

		// --InputManager
		//InputManager::Instance().subscribe();

		// --ViewportManager
		viewportManager = CreateScope<ViewportManager>();
		//---------------------------------------------------------------------------
		 
		
		//--FREETYPE
		freeTypeManager = new libCore::FreeTypeManager();
		//---------------------------------------------------------------------------

		

		// --ASSETS MANAGER (load Default Assets)
		assetsManager.LoadDefaultAssets();
		//---------------------------------------------------------------------------


		roofGenerator = CreateScope<RoofGenerator>();


		//--------------------------ESTO VA FUERA-----------------------------------------------------------
		setupQuad();//->está en el h, se moverá a Un Objeto Renderer
		
		//GENERACION DE LUCES DE PRUEBA -> ESTO TIENE QUE IR FUERA DE AQUI, ES SOLO PARA PROBAR
		srand(13);
		for (unsigned int i = 0; i < NR_LIGHTS; i++)
		{	

			auto light = CreateRef<Light>();
			light->id = i;

			// calculate slightly random offsets
			float minValueXZ = -2.1f;
			float maxValueXZ =  2.2f;

			float minValueY = 0.0f;
			float maxValueY = 2.2f;

			light->transform.position.x = static_cast<float>(minValueXZ + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxValueXZ - minValueXZ))));
			light->transform.position.y = static_cast<float>(minValueY + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxValueY - minValueY))));
			light->transform.position.z = static_cast<float>(minValueXZ + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxValueXZ - minValueXZ))));


			// Define los límites mínimo y máximo para los componentes de color
			float minColorValue = 0.5f;
			float maxColorValue = 1.0f;

			// Genera valores aleatorios para los componentes de color dentro del rango especificado
			light->color.r = static_cast<float>(minColorValue + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxColorValue - minColorValue))));
			light->color.g = static_cast<float>(minColorValue + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxColorValue - minColorValue))));
			light->color.b = static_cast<float>(minColorValue + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxColorValue - minColorValue))));


			float minRadius = 1.5f;
			float maxRadius = 3.0f;
			// Genera un radio aleatorio dentro del rango especificado
			light->Radius = static_cast<float>(minRadius + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxRadius - minRadius))));

			lights.push_back(light);
		}
		//--------------------------------------------------------------------------------------------------

		return true;
	}

	void EngineOpenGL::SetupInputCallbacks()
	{
		glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			if (action == GLFW_PRESS)
			{
				InputManager::Instance().SetKeyPressed(key, true);
			}
			else if (action == GLFW_RELEASE) {
				InputManager::Instance().SetKeyPressed(key, false);
			}
			});

		glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
			double x, y;
			glfwGetCursorPos(window, &x, &y);

			if (action == GLFW_PRESS) {
				InputManager::Instance().SetMousePressed(button, x, y, true);
			}
			else if (action == GLFW_RELEASE) {
				InputManager::Instance().SetMousePressed(button, x, y, false);
			}
			});

		glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
			InputManager::Instance().SetMousePosition(xpos, ypos);
			});
	}

	void EngineOpenGL::StopMainLoop()
	{
		running = false;
	}

	void EngineOpenGL::begin()
	{
		if (useImGUI)
		{
			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}
	}

	void EngineOpenGL::InitializeMainLoop()
	{
		Timestep lastFrameTime = glfwGetTime();

		running = true;


		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window) && running)
		{
			//DELTA TIME
			Timestep currentFrameTime = glfwGetTime();
			m_deltaTime = currentFrameTime - lastFrameTime;
			lastFrameTime = currentFrameTime;

			//WINDOW SIZE
			GLint windowWidth, windowHeight;
			glfwGetWindowSize(window, &windowWidth, &windowHeight);
			glViewport(0, 0, windowWidth, windowHeight);

			//START INPUT UPDATE
			InputManager::Instance().Update();

			//MAIN LOOP FUNCTION CALL
			if (g_mainLoodFnc)
			{
				g_mainLoodFnc(m_deltaTime);
			}
			// -------------------------------------------

			//-- ImGUI
			if (useImGUI)
			{
				guiLayer->begin();
				guiLayer->renderMainMenuBar();
				guiLayer->renderDockers();

				DrawViewports_ImGUI();

				if (g_imGUILoopFnc)
				{
					g_imGUILoopFnc();
				}

				guiLayer->end();
			}
			// -------------------------------------------

			//END INPUT UPDATE
			InputManager::Instance().EndFrame();

			// Actualiza la ventana
			glfwPollEvents();

			// Intercambia los buffers
			glfwSwapBuffers(window);
		}

		if (g_closeGlFnc)
		{
			g_closeGlFnc();
		}

		glfwTerminate();
	}
	// -------------------------------------------------
	// -------------------------------------------------



	//--RENDER PIPELINE
	void EngineOpenGL::RenderViewports()
	{
		//Esto está aqui porque lo estoy usando de Loop principal pero esto debe cambiar
		if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_P))
		{
			m_wireframe = !m_wireframe;
		}



		for (auto& viewport : viewportManager->viewports)
		{
			// En tu loop de renderizado
			glEnable(GL_DEPTH_TEST); // Habilitar el test de profundidad

			// Limpiar el buffer de color y profundidad del framebuffer por defecto
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// 1. Renderizado de geometría en el GBuffer
			viewport->gBuffer->bindGBuffer();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Actualizar la cámara
			viewport->camera->Inputs(m_deltaTime);
			viewport->camera->updateMatrix(45.0f, 0.1f, 1000.0f);

			// Usar el shader de geometría
			libCore::ShaderManager::Get("shaderGeometryPass")->use();
			libCore::ShaderManager::Get("shaderGeometryPass")->setMat4("camMatrix", viewport->camera->cameraMatrix);

			// Configurar el modo de polígono para wireframe o sólido
			if (m_wireframe) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}

			// Dibujar los modelos en la escena
			for (auto& modelContainer : modelsInScene) {
				modelContainer->Draw("shaderGeometryPass");
			}


			// Desligar el GBuffer
			viewport->gBuffer->unbindGBuffer();


			// 2. Renderizado de iluminación en el FBO deferred + iluminación
			viewport->framebuffer_deferred->bindFBO();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //forzamos el quitar el wireframe para mostrar el quad
			libCore::ShaderManager::Get("shaderLightingPass")->use();
			libCore::ShaderManager::Get("shaderLightingPass")->setInt("gPosition", 0);
			libCore::ShaderManager::Get("shaderLightingPass")->setInt("gNormal", 1);
			libCore::ShaderManager::Get("shaderLightingPass")->setInt("gAlbedoSpec", 2);

			viewport->gBuffer->bindTexture("position", 0);
			viewport->gBuffer->bindTexture("normal", 1);
			viewport->gBuffer->bindTexture("albedo", 2);

			// Enviar los uniformes de luz
			for (unsigned int i = 0; i < lights.size(); i++) {
				libCore::ShaderManager::Get("shaderLightingPass")->setVec3( "lights[" + std::to_string(i) +  "].Position", lights[i]->transform.position);
				libCore::ShaderManager::Get("shaderLightingPass")->setVec3( "lights[" + std::to_string(i) +  "].Color", lights[i]->color);
				libCore::ShaderManager::Get("shaderLightingPass")->setFloat("lights[" + std::to_string(i) + "].Linear", lights[i]->Linear);
				libCore::ShaderManager::Get("shaderLightingPass")->setFloat("lights[" + std::to_string(i) + "].Quadratic", lights[i]->Quadratic);
				libCore::ShaderManager::Get("shaderLightingPass")->setFloat("lights[" + std::to_string(i) + "].Radius", lights[i]->Radius);
			}

			libCore::ShaderManager::Get("shaderLightingPass")->setFloat("exposure", hdrExposure);
			libCore::ShaderManager::Get("shaderLightingPass")->setFloat("ambientLight", ambientLight);
			libCore::ShaderManager::Get("shaderLightingPass")->setVec3("viewPos", viewport->camera->Position);

			renderQuad();

			// Desligar el FBO diferido
			viewport->framebuffer_deferred->unbindFBO();

			// 3. Copiar el buffer de profundidad del GBuffer al FBO forward
			glBindFramebuffer(GL_READ_FRAMEBUFFER, viewport->gBuffer->gBuffer);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, viewport->framebuffer_forward->getFramebuffer());

			glBlitFramebuffer(0, 0, viewport->viewportSize.x, viewport->viewportSize.y,
				0, 0, viewport->viewportSize.x, viewport->viewportSize.y,
				GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_FRAMEBUFFER, 0); //->unbind all

			// 4. Renderizado de luces de depuración en el FBO forward
			viewport->framebuffer_forward->bindFBO();
			glClear(GL_COLOR_BUFFER_BIT); // No se limpia aquí el GL_DEPTH_BUFFER_BIT

			glDepthFunc(GL_LESS);
			//glEnable(GL_CULL_FACE);

			// Debug de Luces
			//libCore::ShaderManager::Get("debug")->use();
			//libCore::ShaderManager::Get("debug")->setMat4("view", viewport->camera->view);
			//libCore::ShaderManager::Get("debug")->setMat4("projection", viewport->camera->projection);

			//for (unsigned int i = 0; i < lights.size(); i++) {
			//  lights[i]->DrawDebug("debug");
			//}



			// TEXTOS
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glm::mat4 model = glm::mat4(1.0f);
			libCore::ShaderManager::Get("text")->use();
			libCore::ShaderManager::Get("text")->setMat4("projection", viewport->camera->projection);
			libCore::ShaderManager::Get("text")->setMat4("model", model);
			libCore::ShaderManager::Get("text")->setMat4("view", viewport->camera->view);
			
			//Render Textos
			roofGenerator->PaintRootDataLabels();
			//roofGenerator->PaintSkeletonLabels();
			
			glDisable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ZERO);

			// Desligar el FBO forward
			viewport->framebuffer_forward->unbindFBO();

			// 5. Combinar los resultados en el FBO final
			viewport->framebuffer_final->bindFBO();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			libCore::ShaderManager::Get("combinePass")->use();
			viewport->framebuffer_deferred->bindTexture("color", 0); // Textura de color del FBO deferred
			viewport->framebuffer_forward->bindTexture("color", 1);  // Textura de color del FBO forward
			libCore::ShaderManager::Get("combinePass")->setInt("deferredTexture", 0);
			libCore::ShaderManager::Get("combinePass")->setInt("forwardTexture", 1);

			renderQuad();

			// Desligar el FBO final
			viewport->framebuffer_final->unbindFBO();
		}

		// 6. Renderizar el resultado final en pantalla
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		libCore::ShaderManager::Get("colorQuadFBO")->use();
		viewportManager->viewports[0]->framebuffer_final->bindTexture("color", 0);
		libCore::ShaderManager::Get("colorQuadFBO")->setInt("screenTexture", 0);

		renderQuad();
	}
	// -------------------------------------------------
	// -------------------------------------------------




	//--VIEWPORTS
	void EngineOpenGL::CreateViewport(std::string name, glm::vec3 cameraPosition)
	{
		viewportManager->CreateViewport(name, cameraPosition, windowWidth, windowHeight);
	}
	void EngineOpenGL::DrawViewports_ImGUI()
	{
		if (useImGUI)
		{
			//guiLayer->DrawViewports(viewportManager->viewports);




			//HDR CONTROLS
			// Crear un panel de ImGui para los ajustes HDR
			ImGui::Begin("Global Light");


			ImGui::SliderFloat("Ambient Light", &ambientLight, 0.0f, 10.0f);

			//ImGui::Checkbox("HDR Enabled", &hdrEnabled);
			//ImGui::SliderFloat("HDR Exposure", &hdrExposure, 0.1f, 10.0f);



			ImGui::End();
			//---------------------------------------------------------------------------------------





			// Crear un panel de ImGui
			ImGui::Begin("Framebuffers");
			// Sección del GBuffer
			if (ImGui::CollapsingHeader("GBuffer Textures")) {
				GLuint gBufferPositionTexture = viewportManager->viewports[0]->gBuffer->getTexture("position");
				ImGui::Text("Position");
				ImGui::Image((void*)(intptr_t)gBufferPositionTexture, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));

				GLuint gBufferNormalTexture = viewportManager->viewports[0]->gBuffer->getTexture("normal");
				ImGui::Text("Normal");
				ImGui::Image((void*)(intptr_t)gBufferNormalTexture, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));

				GLuint gBufferAlbedoTexture = viewportManager->viewports[0]->gBuffer->getTexture("albedo");
				ImGui::Text("Albedo");
				ImGui::Image((void*)(intptr_t)gBufferAlbedoTexture, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));
			}

			// Sección de los Framebuffers
			if (ImGui::CollapsingHeader("Framebuffers")) {
				GLuint deferredTexture = viewportManager->viewports[0]->framebuffer_deferred->getTexture("color");
				ImGui::Text("Deferred FBO");
				ImGui::Image((void*)(intptr_t)deferredTexture, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));

				GLuint forwardTexture = viewportManager->viewports[0]->framebuffer_forward->getTexture("color");
				ImGui::Text("Forward FBO");
				ImGui::Image((void*)(intptr_t)forwardTexture, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));
			}

			// Finalizar el panel de ImGui
			ImGui::End();
		}
	}
	// -------------------------------------------------
	// -------------------------------------------------



	//--CREADOR DE PREFABS
	void libCore::EngineOpenGL::CreatePrefabExternalModel(ImportModelData importModelData)
	{
		modelsInScene.push_back(libCore::ModelLoader::LoadModel(importModelData));
	}
	void EngineOpenGL::CreatePrefabDot(const glm::vec3& pos, const glm::vec3& polygonColor)
	{
		auto modelContainer = CreateRef<ModelContainer>();
		modelContainer->skeletal = false;

		auto modelBuild = CreateRef<Model>();
		modelBuild->transform.position = pos;
		modelContainer->name = "PRIMITIVE_DOT";
		modelBuild->meshes.push_back(PrimitivesHelper::CreateDot());
		

		//--DEFAULT_MATERIAL
		auto material = CreateRef<Material>();
		material->materialName = "default_material";

		material->color = polygonColor;

		material->albedoMap = assetsManager.GetTexture("default_albedo");
		//material->normalMap = assetsManager.GetTexture("default_normal");
		//material->metallicMap = assetsManager.GetTexture("default_metallic");
		//material->roughnessMap = assetsManager.GetTexture("default_roughness");
		//material->aOMap = assetsManager.GetTexture("default_ao");

		modelBuild->materials.push_back(material);

		modelContainer->models.push_back(modelBuild);

		modelsInScene.push_back(modelContainer);
	}
	void EngineOpenGL::CreatePrefabLine(const glm::vec3& point1, const glm::vec3& point2)
	{
		auto modelContainer = CreateRef<ModelContainer>();
		modelContainer->skeletal = false;

		auto modelBuild = CreateRef<Model>();

		modelContainer->name = "PRIMITIVE_LINE";
		modelBuild->meshes.push_back(PrimitivesHelper::CreateLine(point1, point2));
		modelContainer->models.push_back(modelBuild);


		//--DEFAULT_MATERIAL
		auto material = CreateRef<Material>();
		material->materialName = "default_material";

		material->color.r = 1.0f;
		material->color.g = 1.0f;
		material->color.b = 1.0f;

		material->albedoMap = assetsManager.GetTexture("default_albedo");
		material->normalMap = assetsManager.GetTexture("default_normal");
		material->metallicMap = assetsManager.GetTexture("default_metallic");
		material->roughnessMap = assetsManager.GetTexture("default_roughness");
		material->aOMap = assetsManager.GetTexture("default_ao");

		modelBuild->materials.push_back(material);

		modelsInScene.push_back(modelContainer);
	}
	void EngineOpenGL::CreatePrefabSphere(float radius, unsigned int sectorCount, unsigned int stackCount)
	{
		auto modelContainer = CreateRef<ModelContainer>();
		modelContainer->skeletal = false;

		auto modelBuild = CreateRef<Model>();


		modelContainer->name = "PRIMIVITE_SPHERE";
		modelBuild->meshes.push_back(PrimitivesHelper::CreateSphere(0.01f, 6, 6));
		modelContainer->models.push_back(modelBuild);


		//--DEFAULT_MATERIAL
		auto material = CreateRef<Material>();
		material->materialName = "default_material";

		material->color.r = 1.0f;
		material->color.g = 1.0f;
		material->color.b = 1.0f;

		material->albedoMap = assetsManager.GetTexture("default_albedo");
		material->normalMap = assetsManager.GetTexture("default_normal");
		material->metallicMap = assetsManager.GetTexture("default_metallic");
		material->roughnessMap = assetsManager.GetTexture("default_roughness");
		material->aOMap = assetsManager.GetTexture("default_ao");

		modelBuild->materials.push_back(material);

		modelsInScene.push_back(modelContainer);
	}
	void EngineOpenGL::CreatePrefabCube()
	{
		auto modelContainer = CreateRef<ModelContainer>();
		modelContainer->skeletal = false;

		auto modelBuild = CreateRef<Model>();

		modelContainer->name = "PRIMIVITE_CUBE";
		modelBuild->meshes.push_back(PrimitivesHelper::CreateCube());
		

		//--DEFAULT_MATERIAL
		auto material = CreateRef<Material>();
		material->materialName = "default_material";

		material->color.r = 1.0f;
		material->color.g = 1.0f;
		material->color.b = 1.0f;

		material->albedoMap = assetsManager.GetTexture("default_albedo");
		material->normalMap = assetsManager.GetTexture("default_normal");
		material->metallicMap = assetsManager.GetTexture("default_metallic");
		material->roughnessMap = assetsManager.GetTexture("default_roughness");
		material->aOMap = assetsManager.GetTexture("default_ao");

		modelBuild->materials.push_back(material);

		modelContainer->models.push_back(modelBuild);

		modelsInScene.push_back(modelContainer);
	}
	void EngineOpenGL::CreateRoof(const std::vector<Vector2d>& points, const std::vector<Vector2d>& holes)
	{
		modelsInScene.push_back(roofGenerator->GenerateRoof(points, holes));
	}
	// -------------------------------------------------
	// -------------------------------------------------



	//--PANELS
	void libCore::EngineOpenGL::DrawHierarchyPanel()
	{
		if (useImGUI)
		{
			guiLayer->DrawHierarchyPanel(modelsInScene);
			guiLayer->DrawLightsPanel(lights);
			//guiLayer->RenderCheckerMatrix(); //Panel para el editor de roofs
		}
	}
	// -------------------------------------------------
	// -------------------------------------------------
}