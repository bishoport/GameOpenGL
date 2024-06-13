#include "EngineOpenGL.h"
#include "../input/InputManager.h"

#include "GuiLayer.h"
#include "ViewportManager.hpp"


namespace libCore
{

	Scope<GuiLayer> guiLayer = nullptr; //esto se declara en el cpp porque si se pone en el h, hay errores de includes cíclicos
	Scope<ViewportManager> viewportManager = nullptr; //esto se declara en el cpp porque si se pone en el h, hay errores de includes cíclicos

	Scope<RoofGenerator> roofGenerator = nullptr;

	Ref<Skeleton> current_sk;
	RoofData current_roofData;

	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		EventManager::OnWindowResizeEvent().trigger(width, height);
	}


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

		// -- Gizmos
		shaderManager.setShaderDataLoad("Gizmos", shadersDirectory + "Gizmos.vert", shadersDirectory + "Gizmos.frag");


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
//		freeTypeManager = new libCore::FreeTypeManager();
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
		Scene::GetInstance().BeginPlay();

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
				Scene::GetInstance().Tick(m_deltaTime);
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
			for (auto& modelContainer : Scene::GetInstance().modelsInScene) {
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
			PaintRootDataLabels();
			//PaintSkeletonLabels();
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

		Scene::GetInstance().modelsInScene.push_back(modelContainer);
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

		Scene::GetInstance().modelsInScene.push_back(modelContainer);
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

		Scene::GetInstance().modelsInScene.push_back(modelContainer);
	}
	Ref<libCore::ModelContainer> EngineOpenGL::CreatePrefabCube()
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

		Scene::GetInstance().modelsInScene.push_back(modelContainer);

		return modelContainer;
	}



	//--ROOF´s
	void EngineOpenGL::CreateRoof(const std::vector<Vector2d>& points, const std::vector<Vector2d>& holes)
	{
		std::vector<Vector2d> unorderedPoints =
		{
			{ 31.238701, 37.603262 },
			{ 31.238701, 37.110594 },
			{ 30.443713, 37.110594 },
			{ 30.447452, 37.603262 },
			{ 29.253099, 37.606995 },
			{ 29.249366, 37.170311 },
			{ 27.592210, 37.174044 },
			{ 25.998504, 37.181509 },
			{ 25.998504, 37.636853 },
			{ 25.412527, 37.636853 },
			{ 25.412527, 37.883187 },
			{ 25.412527, 38.095930 },
			{ 25.248305, 38.099662 },
			{ 24.554090, 38.099662 },
			{ 23.882270, 38.099662 },
			{ 23.781497, 38.099662 },
			{ 23.781497, 37.655515 },
			{ 23.199253, 37.659247 },
			{ 23.195520, 36.128990 },
			{ 23.356011, 36.128990 },
			{ 23.356011, 34.236697 },
			{ 23.202986, 34.236697 },
			{ 23.202986, 33.803746 },
			{ 22.359478, 33.803746 },
			{ 22.359478, 33.359599 },
			{ 22.243776, 33.359599 },
			{ 22.247508, 32.489965 },
			{ 22.247508, 31.769625 },
			{ 22.344549, 31.769625 },
			{ 22.344549, 31.310548 },
			{ 23.191787, 31.314280 },
			{ 23.191787, 30.795485 },
			{ 23.292561, 30.795485 },
			{ 23.300003, 29.011430 },
			{ 23.154465, 29.011430 },
			{ 23.161930, 27.391597 },
			{ 23.699386, 27.395330 },
			{ 23.703118, 26.891464 },
			{ 23.878538, 26.891464 },
			{ 24.565287, 26.895196 },
			{ 25.308022, 26.898929 },
			{ 25.431189, 26.895196 },
			{ 25.431189, 26.943717 },
			{ 25.427456, 27.477441 },
			{ 26.013433, 27.481173 },
			{ 26.013433, 27.708846 },
			{ 27.581013, 27.712578 },
			{ 29.174719, 27.716310 },
			{ 29.174719, 27.440117 },
			{ 30.290687, 27.443850 },
			{ 30.290687, 28.205246 },
			{ 31.294685, 28.208978 },
			{ 31.294685, 27.544622 },
			{ 32.574875, 27.544622 },
			{ 33.791616, 27.548355 },
			{ 33.791616, 28.212710 },
			{ 34.773220, 28.216442 },
			{ 34.773220, 27.548355 },
			{ 35.721233, 27.548355 },
			{ 35.721233, 27.910392 },
			{ 37.423177, 27.910392 },
			{ 38.736959, 27.906659 },
			{ 38.736959, 27.197515 },
			{ 39.580466, 27.193783 },
			{ 39.580466, 26.846676 },
			{ 40.472494, 26.846676 },
			{ 41.192834, 26.846676 },
			{ 41.192834, 27.208713 },
			{ 41.999018, 27.204980 },
			{ 42.006482, 30.955976 },
			{ 42.010215, 31.269492 },
			{ 42.323731, 31.269492 },
			{ 42.327463, 32.531020 },
			{ 42.327463, 33.848534 },
			{ 41.950498, 33.852267 },
			{ 41.946766, 34.076207 },
			{ 41.950498, 37.577136 },
			{ 41.192834, 37.580868 },
			{ 41.200299, 37.950369 },
			{ 41.200299, 38.058607 },
			{ 40.446367, 38.058607 },
			{ 39.740956, 38.058607 },
			{ 39.737224, 37.569675 },
			{ 39.110199, 37.569675 },
			{ 39.110199, 37.035948 },
			{ 37.423177, 37.039680 },
			{ 37.008888, 37.043412 },
			{ 35.766021, 37.050876 },
			{ 35.769753, 37.513686 },
			{ 34.694841, 37.517419 },
			{ 34.694841, 37.114327 },
			{ 33.828940, 37.118059 },
			{ 33.828940, 37.588332 },
			{ 33.795349, 37.588332 },
			{ 33.784152, 37.812272 },
			{ 33.784152, 37.969031 },
			{ 33.224301, 37.969031 },
			{ 32.530087, 37.983960 },
			{ 31.705242, 37.987692 },
			{ 31.365599, 37.991425 },
			{ 31.365599, 37.752556 },
			{ 31.365599, 37.603262 }
			//{ 31.238701, 37.603262 }
		};


		std::vector<Vector2d> unorderedPoints2 =
		{
			{ 0.0, 0.0 },
			{ 4.0, 0.0 },
			{ 4.0, 2.0 },
			{ 2.0, 2.0 },
			{ 2.0, 6.0 },
			{ 0.0, 6.0 }
		};

		std::vector<Vector2d> unorderedPoints3 =
		{
			{0.000000, 9.000000  },
			{ 1.000000 ,8.000000 },
			{ 1.000000 ,7.000000 },
			{ 1.000000 ,6.000000 },
			{ 2.000000 ,5.000000 },
			{ 4.000000 ,5.000000 },
			{ 4.000000 ,3.000000 },
			{ 4.000000 ,2.000000 },
			{ 3.000000 ,0.000000 },
			{ 2.000000 ,0.000000 },
			{ 2.000000 ,-1.000000 },
			{ 5.000000 ,-1.000000 },
			{ 5.000000 ,0.000000 },
			{ 8.000000 ,0.000000 },
			{ 8.000000 ,5.000000 },
			{ 9.000000 ,6.000000 },
			{ 10.000000, 6.000000 },
			{ 10.000000, 8.000000 },
			{ 9.000000 ,8.000000 },
			{ 9.000000 ,9.000000 },
			{ 6.000000 ,9.000000 },
			{ 6.000000 ,7.000000 },
			{ 4.000000 ,7.000000 },
			{ 4.000000 ,9.000000 }
		};

		std::vector<Vector2d> testT = {
			{1.0, 7.0},
			{7.0, 7.0},
			{7.0, 3.0},
			{5.0, 3.0},
			{5.0, 1.0},
			{3.0, 1.0},
			{3.0, 3.0},
			{1.0, 3.0}
		};


		current_roofData.polygon = points;
		current_roofData.hole = holes;
		current_roofData.holes = { current_roofData.hole };



		current_roofData.expected = {
			//{1.0, 7.0},
			//{7.0, 7.0},
			//{7.0, 3.0},
			//{5.0, 3.0},
			//{5.0, 1.0},
			//{3.0, 1.0},
			//{3.0, 3.0},
			//{1.0, 3.0},
			//{4.0, 2.0},
			//{4.0, 4.0},
			//{5.0, 5.0},
			//{3.0, 5.0}
		};
		//PaintRoofDataPoints(current_roofData);

		// Construir el esqueleto
		current_sk = CreateRef<Skeleton>(roofGenerator->BuildSkeleton(current_roofData));
		PaintSkeletonPoints(*current_sk);

		Scene::GetInstance().modelsInScene.push_back(roofGenerator->CreateRoof(*current_sk, 102));
	}

	
	void EngineOpenGL::PaintRoofDataPoints(const RoofData& data) {

		// Colores definidos para cada conjunto de puntos
		glm::vec3 polygonColor(1.0f, 1.0f, 1.0f);  // Rojo
		glm::vec3 holeColor(0.0f, 1.0f, 0.0f);     // Verde
		glm::vec3 expectedColor(1.0f, 1.0f, 0.0f); // Amarillo

		// Pintar puntos del polígono
		int pointCounter = 0;
		for (const auto& point : data.polygon) {
			CreatePrefabDot(glm::vec3(point.X, 0.0f, point.Y), polygonColor);
			std::string pointInfo = std::to_string(pointCounter);
			freeTypeManager->RenderText(pointInfo, glm::vec3(point.X, 1.0f, point.Y), 1.009f, polygonColor);
		}

		// Crear líneas entre los puntos
		for (size_t i = 0; i < data.polygon.size(); ++i) {
			const auto& point1 = data.polygon[i];
			const auto& point2 = data.polygon[(i + 1) % data.polygon.size()]; // Conectar el último punto con el primero
			CreatePrefabLine(glm::vec3(point1.X, 0.0f, point1.Y), glm::vec3(point2.X, 0.0f, point2.Y));
		}

		//// Pintar puntos del agujero
		//for (const auto& point : data.hole) {
		//	CreatePrefabDot(glm::vec3(point.X, 0.0f, point.Y), holeColor);
		//}

		// Pintar puntos de los agujeros (si hay múltiples)
		for (const auto& hole : data.holes) {
			for (const auto& point : hole) {
				CreatePrefabDot(glm::vec3(point.X, 0.0f, point.Y), holeColor);
			}
		}
		//
		// Pintar puntos esperados
		for (const auto& point : data.expected) {
			CreatePrefabDot(glm::vec3(point.X, 0.0f, point.Y), expectedColor);
		}
	}
	void EngineOpenGL::PaintSkeletonPoints(const Skeleton& skeleton) {

		// Colores definidos para cada conjunto de puntos
		glm::vec3 edgeColor(1.0f, 0.0f, 0.0f);     // Rojo
		glm::vec3 distanceColor(0.0f, 1.0f, 0.0f); // Verde

		// Pintar puntos de los Edges
		if (skeleton.Edges) {
			for (const auto& edgeResult : *skeleton.Edges) {
				if (edgeResult->Polygon) {
					for (size_t i = 0; i < edgeResult->Polygon->size(); ++i) {
						const auto& point = (*edgeResult->Polygon)[i];
						double height = skeleton.Distances->at(*point);
						CreatePrefabDot(glm::vec3(point->X, height, point->Y), edgeColor);
		
						// Crear líneas entre los puntos
						const auto& nextPoint = (*edgeResult->Polygon)[(i + 1) % edgeResult->Polygon->size()];
						double nextHeight = skeleton.Distances->at(*nextPoint);
						CreatePrefabLine(glm::vec3(point->X, height, point->Y), glm::vec3(nextPoint->X, nextHeight, nextPoint->Y));
					}
				}
			}
		}

		// Pintar puntos de Distances
		//if (skeleton.Distances) {
		//	std::vector<glm::vec3> points;  // Almacenar los puntos en un vector para crear líneas más tarde

		//	for (const auto& distancePair : *skeleton.Distances) {
		//		const auto& point = distancePair.first;
		//		double height = distancePair.second;
		//		glm::vec3 point3D(point.X, height, point.Y);
		//		CreatePrefabDot(point3D, distanceColor);
		//		points.push_back(point3D);
		//	}

		//	// Crear líneas entre los puntos consecutivos
		//	for (size_t i = 0; i < points.size(); ++i) {
		//		const glm::vec3& point1 = points[i];
		//		const glm::vec3& point2 = points[(i + 1) % points.size()];  // Conectar el último punto con el primero
		//		CreatePrefabLine(point1, point2);
		//	}
		//}
	}
	void libCore::EngineOpenGL::PaintRootDataLabels()
	{
		// Colores definidos para cada conjunto de puntos
		glm::vec3 polygonColor(1.0f, 1.0f, 1.0f);  // Rojo
		glm::vec3 holeColor(0.0f, 1.0f, 0.0f);     // Verde
		glm::vec3 expectedColor(1.0f, 1.0f, 0.0f); // Amarillo

		// Pintar puntos del polígono
		int pointCounter = 0;
		for (const auto& point : current_roofData.polygon) 
		{
			std::string pointInfo = std::to_string(pointCounter);
			freeTypeManager->RenderText(pointInfo, glm::vec3(point.X, -0.5f, point.Y), 0.006f, polygonColor);
			pointCounter++;
		}

		pointCounter = 0;
		// Pintar puntos de los agujeros (si hay múltiples)
		for (const auto& hole : current_roofData.holes) {
			for (const auto& point : hole) 
			{
				pointCounter++;
			}
		}
		

		pointCounter = 0;
		// Pintar puntos esperados
		for (const auto& point : current_roofData.expected) 
		{
			pointCounter++;
		}
	}
	// Utilizamos una estructura personalizada para comparar los vectores 2D en el mapa
	struct Vector2dComparator {
		bool operator()(const Vector2d& a, const Vector2d& b) const {
			if (a.X != b.X)
				return a.X < b.X;
			return a.Y < b.Y;
		}
	};
	void libCore::EngineOpenGL::PaintSkeletonLabels()
	{
		// Colores definidos para cada conjunto de puntos
		glm::vec3 edgeColor(1.0f, 0.0f, 0.0f);     // Rojo
		glm::vec3 distanceColor(0.0f, 1.0f, 0.0f); // Verde

		// Mapa para agrupar puntos por posición
		std::map<Vector2d, std::vector<int>, Vector2dComparator> pointMap;

		// Agrupar los puntos por su posición y almacenar los índices
		int pointCounter = 0;
		if (current_sk->Edges) {
			for (const auto& edgeResult : *current_sk->Edges) {
				if (edgeResult->Polygon) {
					for (const auto& point : *edgeResult->Polygon) {
						pointMap[*point].push_back(pointCounter);
						pointCounter++;
					}
				}
			}
		}

		// Renderizar los textos agrupados
		for (const auto& entry : pointMap) {
			const Vector2d& point = entry.first;
			const std::vector<int>& indices = entry.second;
			double height = current_sk->Distances->at(point);

			// Crear el texto con todos los índices
			std::string pointInfo;
			for (size_t i = 0; i < indices.size(); ++i) {
				pointInfo += std::to_string(indices[i]);
				if (i < indices.size() - 1) {
					pointInfo += ", ";
				}
			}

			// Renderizar el texto en la posición del punto
			freeTypeManager->RenderText(pointInfo, glm::vec3(point.X, height, point.Y), 0.003f, edgeColor);
		}
		
		pointCounter = 0;
		// Pintar puntos de Distances
		if (current_sk->Distances) {
			for (const auto& distancePair : *current_sk->Distances) {
				const auto& point = distancePair.first;
				double height = distancePair.second;
				std::string pointInfo = std::to_string(pointCounter);
				freeTypeManager->RenderText(pointInfo, glm::vec3(point.X, height + 0.2, point.Y), 0.006f, distanceColor);
				pointCounter++;
			}
		}
	}

	

	void EngineOpenGL::AddBounding(Ref<libCore::ModelContainer>& modelLoader) {
		AttachBounding(modelLoader->models[0]);
	}
	void EngineOpenGL::AttachBounding(Ref<libCore::Model>& attachToModel)
	{
		Ref<libCore::Mesh> mesh = PrimitivesHelper::CreateBoundingBox();
		mesh->isBB = true;
		attachToModel->meshes.push_back(mesh);
		auto material = CreateRef<Material>();
		//Cambiar por un material con el shader debug
		material->materialName = "default_material";

		material->color.r = 1.0f;
		material->albedoMap = assetsManager.GetTexture("default_albedo");
		attachToModel->materials.push_back(material);
}

	// -------------------------------------------------
	// -------------------------------------------------




	//--PANELS
	void libCore::EngineOpenGL::DrawHierarchyPanel()
	{
		if (useImGUI)
		{
			guiLayer->DrawHierarchyPanel(Scene::GetInstance().modelsInScene);
			guiLayer->DrawLightsPanel(lights);
			guiLayer->RenderCheckerMatrix();
		}
	}
	// -------------------------------------------------
	// -------------------------------------------------
}



















//-------------------------------------ROOFS------------------------------------------------------------------------------------------

	//struct Edge {
	//	glm::vec3 start;
	//	glm::vec3 end;
	//};

	//struct VertexRoof {
	//	glm::vec3 position;
	//	glm::vec3 bisector;
	//};

	//std::vector<Edge> CalculateEdges(const std::vector<glm::vec3>& polygon) {
	//	std::vector<Edge> edges;
	//	for (size_t i = 0; i < polygon.size(); ++i) {
	//		edges.push_back({ polygon[i], polygon[(i + 1) % polygon.size()] });
	//	}
	//	return edges;
	//}

	//glm::vec3 CalculateBisector(const glm::vec3& prev, const glm::vec3& curr, const glm::vec3& next) {
	//	glm::vec3 prevDir = glm::normalize(curr - prev);
	//	glm::vec3 nextDir = glm::normalize(next - curr);
	//	glm::vec3 bisector = glm::normalize(prevDir + nextDir);
	//	return bisector;
	//}

	//bool EdgeEvent(const Edge& edge) {
	//	const float threshold = 0.1f;  // Ajustar el umbral para detectar eventos de borde
	//	bool event = glm::length(edge.end - edge.start) < threshold;
	//	if (event) {
	//		std::cout << "EdgeEvent detected between (" << edge.start.x << ", " << edge.start.y << ", " << edge.start.z
	//			<< ") and (" << edge.end.x << ", " << edge.end.y << ", " << edge.end.z << ")\n";
	//	}
	//	return event;
	//}

	//bool SplitEvent(const VertexRoof& vertex, const Edge& edge) {
	//	glm::vec3 edgeVector = edge.end - edge.start;
	//	glm::vec3 toStart = edge.start - vertex.position;

	//	glm::vec3 bisectorProjection = glm::dot(vertex.bisector, edgeVector) / glm::length2(edgeVector) * edgeVector;
	//	float lengthToProjection = glm::length(bisectorProjection);

	//	bool event = lengthToProjection > 0.0f && lengthToProjection < glm::length(edgeVector);
	//	if (event) {
	//		std::cout << "SplitEvent detected at vertex (" << vertex.position.x << ", " << vertex.position.y << ", " << vertex.position.z
	//			<< ") with edge (" << edge.start.x << ", " << edge.start.y << ", " << edge.start.z
	//			<< ") - (" << edge.end.x << ", " << edge.end.y << ", " << edge.end.z << ")\n";
	//	}
	//	return event;
	//}

	//glm::vec3 IntersectBisectorWithEdge(const VertexRoof& vertex, const Edge& edge) {
	//	glm::vec3 edgeVector = edge.end - edge.start;
	//	glm::vec3 toStart = edge.start - vertex.position;

	//	float t = glm::dot(toStart, edgeVector) / glm::dot(vertex.bisector, edgeVector);
	//	glm::vec3 intersection = vertex.position + t * vertex.bisector;

	//	if (t >= 0.0f && t <= 1.0f) {
	//		std::cout << "Intersection at (" << intersection.x << ", " << intersection.y << ", " << intersection.z << ")\n";
	//		return intersection;
	//	}
	//	else {
	//		std::cout << "Invalid intersection at (" << intersection.x << ", " << intersection.y << ", " << intersection.z << ")\n";
	//		return vertex.position;
	//	}
	//}

	//std::vector<glm::vec3> CalculateStraightSkeleton(const std::vector<glm::vec3>& polygon) {
	//	std::vector<glm::vec3> skeleton;

	//	std::vector<VertexRoof> vertices;
	//	for (size_t i = 0; i < polygon.size(); ++i) {
	//		glm::vec3 prev = polygon[(i + polygon.size() - 1) % polygon.size()];
	//		glm::vec3 curr = polygon[i];
	//		glm::vec3 next = polygon[(i + 1) % polygon.size()];
	//		vertices.push_back({ curr, CalculateBisector(prev, curr, next) });
	//	}

	//	std::vector<Edge> edges = CalculateEdges(polygon);

	//	bool eventFinished = false;
	//	int iteration = 0;
	//	while (!eventFinished && iteration < 100) {
	//		eventFinished = true;
	//		iteration++;

	//		std::vector<VertexRoof> newVertices;
	//		std::vector<Edge> newEdges;

	//		for (size_t i = 0; i < vertices.size(); ++i) {
	//			VertexRoof& vertex = vertices[i];
	//			Edge& edge = edges[i];

	//			if (EdgeEvent(edge)) {
	//				eventFinished = false;
	//				glm::vec3 midPoint = (edge.start + edge.end) / 2.0f;
	//				newVertices.push_back({ midPoint, CalculateBisector(edge.start, midPoint, edge.end) });
	//				continue;
	//			}

	//			if (SplitEvent(vertex, edge)) {
	//				eventFinished = false;
	//				glm::vec3 intersectionPoint = IntersectBisectorWithEdge(vertex, edge);
	//				if (intersectionPoint != vertex.position && intersectionPoint != edge.start && intersectionPoint != edge.end) {
	//					newVertices.push_back({ intersectionPoint, CalculateBisector(vertex.position, intersectionPoint, edge.end) });
	//					newEdges.push_back({ vertex.position, intersectionPoint });
	//					newEdges.push_back({ intersectionPoint, edge.end });
	//				}
	//				else {
	//					newEdges.push_back(edge);
	//				}
	//			}
	//			else {
	//				newEdges.push_back(edge);
	//			}
	//		}

	//		std::cout << "Iteration " << iteration << "\n";
	//		std::cout << "Vertices:\n";
	//		for (const auto& vertex : vertices) {
	//			std::cout << "(" << vertex.position.x << ", " << vertex.position.y << ", " << vertex.position.z << ")\n";
	//		}
	//		std::cout << "Edges:\n";
	//		for (const auto& edge : edges) {
	//			std::cout << "(" << edge.start.x << ", " << edge.start.y << ", " << edge.start.z << ") - (" << edge.end.x << ", " << edge.end.y << ", " << edge.end.z << ")\n";
	//		}

	//		if (newVertices.empty()) {
	//			glm::vec3 center(0.0f, 0.5f, 0.0f);
	//			skeleton.push_back(center);
	//			break;
	//		}

	//		vertices = newVertices;
	//		edges = newEdges;
	//	}

	//	if (iteration >= 100) {
	//		std::cout << "Reached iteration limit. Check for possible infinite loop.\n";
	//	}

	//	return skeleton;
	//}

	//void AddRoofVertices(std::vector<GLfloat>& vertexBuffer, std::vector<GLuint>& indices, const std::vector<glm::vec3>& basePolygon, const std::vector<glm::vec3>& skeleton) {
	//	GLuint baseIndex = 0;
	//	GLuint skeletonIndex = basePolygon.size();

	//	std::vector<glm::vec2> uvCoords = {
	//		{0.0f, 0.0f},
	//		{0.0f, 1.0f},
	//		{0.5f, 1.0f},
	//		{0.5f, 0.5f},
	//		{1.0f, 0.5f},
	//		{1.0f, 0.0f}
	//	};

	//	for (size_t i = 0; i < basePolygon.size(); ++i) {
	//		const auto& vertex = basePolygon[i];
	//		vertexBuffer.push_back(vertex.x);
	//		vertexBuffer.push_back(vertex.y);
	//		vertexBuffer.push_back(vertex.z);
	//		vertexBuffer.push_back(uvCoords[i].x);
	//		vertexBuffer.push_back(uvCoords[i].y);
	//		vertexBuffer.push_back(0.0f);
	//		vertexBuffer.push_back(1.0f);
	//		vertexBuffer.push_back(0.0f);
	//	}

	//	for (const auto& vertex : skeleton) {
	//		vertexBuffer.push_back(vertex.x);
	//		vertexBuffer.push_back(vertex.y);
	//		vertexBuffer.push_back(vertex.z);
	//		vertexBuffer.push_back(0.5f);
	//		vertexBuffer.push_back(0.5f);
	//		vertexBuffer.push_back(0.0f);
	//		vertexBuffer.push_back(1.0f);
	//		vertexBuffer.push_back(0.0f);
	//	}

	//	for (size_t i = 0; i < basePolygon.size(); ++i) {
	//		size_t nextIndex = (i + 1) % basePolygon.size();
	//		indices.push_back(baseIndex + i);
	//		indices.push_back(baseIndex + nextIndex);
	//		indices.push_back(skeletonIndex);
	//	}
	//}



//Ref<ModelContainer> EngineOpenGL::CreateRoof() {
//
//	//auto modelContainer = CreateRef<ModelContainer>();
//	//modelContainer->skeletal = false;
//
//	//auto modelBuild = CreateRef<Model>();
//	//modelContainer->name = "ROOF";
//
//	//auto mesh = CreateRef<Mesh>();
//
//	//std::vector<glm::vec3> basePolygon = {
//	//	{-0.5f, 0.0f,  0.5f},
//	//	{-0.5f, 0.0f, -0.5f},
//	//	{ 0.0f, 0.0f, -0.5f},
//	//	{ 0.0f, 0.0f,  0.0f},
//	//	{ 0.5f, 0.0f,  0.0f},
//	//	{ 0.5f, 0.0f,  0.5f}
//	//};
//
//	//std::vector<glm::vec3> skeleton = CalculateStraightSkeleton(basePolygon);
//
//	//std::vector<GLfloat> vertexBuffer;
//	//std::vector<GLuint> indices;
//	//AddRoofVertices(vertexBuffer, indices, basePolygon, skeleton);
//
//	//mesh->vertices.reserve(vertexBuffer.size() / 8);
//
//	//for (size_t i = 0; i < vertexBuffer.size(); i += 8) {
//	//	Vertex vertex;
//	//	vertex.position = glm::vec3(vertexBuffer[i], vertexBuffer[i + 1], vertexBuffer[i + 2]);
//	//	vertex.texUV = glm::vec2(vertexBuffer[i + 3], vertexBuffer[i + 4]);
//	//	vertex.normal = glm::vec3(vertexBuffer[i + 5], vertexBuffer[i + 6], vertexBuffer[i + 7]);
//
//	//	vertex.tangent = glm::vec3(0.0f, 0.0f, 0.0f);
//	//	vertex.bitangent = glm::vec3(0.0f, 0.0f, 0.0f);
//
//	//	mesh->vertices.push_back(vertex);
//	//}
//
//	//mesh->indices = indices;
//	//mesh->SetupMesh();
//
//	//modelBuild->meshes.push_back(mesh);
//
//	////--DEFAULT_MATERIAL
//	//auto material = CreateRef<Material>();
//	//material->materialName = "roof_material";
//	//material->color.r = 1.0f;
//	//material->color.g = 1.0f;
//	//material->color.b = 1.0f;
//	//material->albedoMap = assetsManager.GetTexture("default_wall");
//	//material->normalMap = assetsManager.GetTexture("default_normal");
//	//material->metallicMap = assetsManager.GetTexture("default_metallic");
//	//material->roughnessMap = assetsManager.GetTexture("default_roughness");
//	//material->aOMap = assetsManager.GetTexture("default_ao");
//	//modelBuild->materials.push_back(material);
//	////----------------------------------------------------------------------------
//
//	//modelContainer->models.push_back(modelBuild);
//
//	//return modelContainer;
//}
