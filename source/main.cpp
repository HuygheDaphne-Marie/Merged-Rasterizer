#include "pch.h"
//#undef main

//Standard includes
#include <iostream>

//Project includes
#include "ETimer.h"
#include "ERenderer.h"
#include "Vertex.h"
#include <vector>
#include "SceneManager.h"
#include "Texture.h"
#include "EOBJParser.h"

#include "VehicleMaterial.h"
#include "ExhaustMaterial.h"

using namespace Elite;

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;
	SDL_Window* pWindow = SDL_CreateWindow(
		"DirectX - Henri-Thibault Huyghe",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	auto pTimer{ std::make_unique<Elite::Timer>() };
	auto pRenderer{ std::make_unique<Elite::Renderer>(pWindow) };
	SceneManager& sceneManager{ SceneManager::GetInstance() };

	// Init Meshes
	SceneGraph& activeScene = sceneManager.GetActiveScene();
	activeScene.SetCamera(new Camera(width, height));
	{
		//const std::vector<IVertex> vertices // quad
		//{
		//	{FPoint3{-3.f, 3.f, 2.f}, FPoint2{0,0}, RGBColor{1.f, 1.f, 1.f}},
		//	{FPoint3{3.f, 3.f, 2.f}, FPoint2{1,0}, RGBColor{1.f, 1.f, 1.f}},
		//	{FPoint3{-3.f, -3.f, 2.f}, FPoint2{0,1}, RGBColor{1.f, 1.f, 1.f}},
		//	{FPoint3{3.f, -3.f, 2.f}, FPoint2{1,1}, RGBColor{1.f, 1.f, 1.f}}
		//};
		//const std::vector<uint32_t> indices{ 0, 1, 2, 2, 1, 3 };
		//
		//Mesh* pQuad = new Mesh(pRenderer->GetDevice(), vertices, indices, L"Resources/PosCol3D.fx");
		//pQuad->SetDiffuseTexture(new Texture("Resources/uv_grid_2.png", pRenderer->GetDevice()));
		//activeScene.AddGeometryToScene(pQuad);

		{
			std::vector<IVertex> verticesFromFile{};
			std::vector<uint32_t> indices{};
			ParseOBJ("Resources/vehicle.obj", verticesFromFile, indices);

			VehicleMaterial* pVehicleMaterial{ new VehicleMaterial(pRenderer->GetDevice(), L"Resources/PosCol3D.fx") };
			pVehicleMaterial->SetDiffuseTexture(new Texture("Resources/vehicle_diffuse.png", pRenderer->GetDevice()));
			pVehicleMaterial->SetNormalMap(new Texture("Resources/vehicle_normal.png", pRenderer->GetDevice()));
			pVehicleMaterial->SetSpecularMap(new Texture("Resources/vehicle_specular.png", pRenderer->GetDevice()));
			pVehicleMaterial->SetGlossinessMap(new Texture("Resources/vehicle_gloss.png", pRenderer->GetDevice()));

			Mesh* pMesh = new Mesh(pRenderer->GetDevice(), verticesFromFile, indices, pVehicleMaterial);
			activeScene.AddGeometryToScene(pMesh);
		}

		{
			std::vector<IVertex> verticesFromFile{};
			std::vector<uint32_t> indices{};
			ParseOBJ("Resources/fireFX.obj", verticesFromFile, indices);

			ExhaustMaterial* pExhaustMaterial
			{
				new ExhaustMaterial (pRenderer->GetDevice(), L"Resources/Exhaust.fx", 
					new Texture("Resources/fireFX_diffuse.png", pRenderer->GetDevice()))
			};

			Mesh* pMesh = new Mesh(pRenderer->GetDevice(), verticesFromFile, indices, pExhaustMaterial);
			activeScene.AddGeometryToScene(pMesh);
		}
	}
	
	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;

	while (isLooping)
	{
		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;
			case SDL_KEYUP:
				if (e.key.keysym.sym == SDLK_f)
				{
					for (auto* pMesh : activeScene.GetGeometries())
					{
						pMesh->GetMaterial()->GotoNextTechnique();
					}
				}
				break;
			}
		}

		// Update
		activeScene.GetCamera()->Update(pTimer->GetElapsed());

		//--------- Render ---------
		pRenderer->Render();

		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			std::cout << "FPS: " << pTimer->GetFPS() << std::endl;
		}

	}
	pTimer->Stop();

	//Shutdown "framework"
	ShutDown(pWindow);
	return 0;
}