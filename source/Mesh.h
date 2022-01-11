#pragma once
#include <vector>
#include "Vertex.h"
#include "Material.h"
#include "Texture.h"

class Mesh final
{
public:
	Mesh(ID3D11Device* pDevice, const std::vector<IVertex>& vertices, const std::vector<uint32_t>& indices, Material* pMaterial, 
		const Elite::FPoint3& position = {0,0,0}, const Elite::FVector3& forward = {0,0,1});
	Mesh(Mesh& other) = delete;
	Mesh(Mesh&& other) = delete;
	Mesh operator=(Mesh& other) = delete;
	Mesh operator=(Mesh&& other) = delete;
	~Mesh();

	void Render(ID3D11DeviceContext* pDeviceContext);

	Material* GetMaterial();

	void SetIsActive(bool isActive);
	bool GetIsActive() const;

	Elite::FPoint3 GetPosition() const;
	void SetPosition(const Elite::FPoint3& position);

	Elite::FVector3 GetForward() const;
	void SetForward(const Elite::FVector3& forward);

	Elite::FMatrix4 GetTransform() const;

private:
	bool m_IsActive{ true };
	Elite::FPoint3 m_Position;
	Elite::FVector3 m_Forward;
	Elite::FMatrix4 m_Transform{};

	Material* m_pMaterial;

	ID3D11Buffer* m_pVertexBuffer;
	ID3D11InputLayout* m_pVertexLayout;

	uint32_t m_AmountIndices;
	ID3D11Buffer* m_pIndexBuffer;

	void RecalculateTransform();
};

