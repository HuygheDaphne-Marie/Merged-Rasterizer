#pragma once
#include <vector>
#include "Mesh.h"
#include "ECamera.h"
#include "Geometry.h"

using namespace Elite;

class SceneGraph final
{
public:
	// Todo: rule of 5
	~SceneGraph();

	void AddGeometryToScene(Geometry* geometry);
	const std::vector<Geometry*>& GetGeometries() const;

	void AddMeshToScene(Mesh* geometry);
	const std::vector<Mesh*>& GetMeshes() const;

	void SetCamera(Camera* pCamera);
	Camera* GetCamera() const;

private:
	std::vector<Geometry*> m_pGeometries{};
	std::vector<Mesh*> m_pMeshes{};
	Camera* m_pCamera{ nullptr };
};