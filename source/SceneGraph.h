#pragma once
#include <vector>
#include "Mesh.h"
#include "ECamera.h"

using namespace Elite;

class SceneGraph final
{
public:
	// Todo: rule of 5
	~SceneGraph();

	void AddGeometryToScene(Mesh* geometry);
	const std::vector<Mesh*>& GetGeometries() const;

	void SetCamera(Camera* pCamera);
	Camera* GetCamera() const;

private:
	std::vector<Mesh*> m_pGeometries{};
	Camera* m_pCamera{ nullptr };
};