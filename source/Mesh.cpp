#include "pch.h"
#include "Mesh.h"
#include "SceneManager.h"

Mesh::Mesh(ID3D11Device* pDevice, const std::vector<IVertex>& vertices, const std::vector<unsigned int>& indices, Material* pMaterial,
	const Elite::FPoint3& position, const Elite::FVector3& forward)
	: m_pMaterial(pMaterial)
	, m_Position(position)
	, m_Forward(forward)
{
	RecalculateTransform();

	// Create vertex layout
	HRESULT result = S_OK;
	static constexpr uint32_t numElements{ 4 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "TEXCOORD";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "NORMAL";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 20;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "TANGENT";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[3].AlignedByteOffset = 32;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	// Create vertex buffer
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(IVertex) * static_cast<uint32_t>(vertices.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData{nullptr};
	initData.pSysMem = vertices.data();

	result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(result))
		return;

	// Create input layout
	D3DX11_PASS_DESC passDesc{};
	m_pMaterial->GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);
	result = pDevice->CreateInputLayout(vertexDesc, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_pVertexLayout);
	if (FAILED(result))
		return;

	// Create index buffer
	m_AmountIndices = static_cast<uint32_t>(indices.size());
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * m_AmountIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = indices.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	if (FAILED(result))
		return;

	std::cout << "Mesh loaded successfully\n";
}

Mesh::~Mesh()
{
	m_pIndexBuffer->Release();
	m_pVertexLayout->Release();
	m_pVertexBuffer->Release();
	delete m_pMaterial;
}

void Mesh::Render(ID3D11DeviceContext* pDeviceContext)
{
	if (!m_IsActive)
		return;

	// Set vertex buffer
	UINT stride = sizeof(IVertex);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	
	// Set index buffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set input layout
	pDeviceContext->IASetInputLayout(m_pVertexLayout);

	// Set primitive topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Update Effect (Sets shader variables)
	m_pMaterial->Update(this);

	// Render a triangle
	D3DX11_TECHNIQUE_DESC techDesc{}; // Todo: edited this
	m_pMaterial->GetTechnique()->GetDesc(&techDesc);
	for (UINT passIndex = 0; passIndex < techDesc.Passes; ++passIndex)
	{
		m_pMaterial->GetTechnique()->GetPassByIndex(0)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_AmountIndices, 0, 0);
	}
}

Material* Mesh::GetMaterial()
{
	return m_pMaterial;
}

void Mesh::SetIsActive(bool isActive)
{
	m_IsActive = isActive;
}

bool Mesh::GetIsActive() const
{
	return m_IsActive;
}

Elite::FPoint3 Mesh::GetPosition() const
{
	return m_Position;
}
void Mesh::SetPosition(const Elite::FPoint3& position)
{
	m_Position = position;
	RecalculateTransform();
}

Elite::FVector3 Mesh::GetForward() const
{
	return m_Forward;
}
void Mesh::SetForward(const Elite::FVector3& forward)
{
	m_Forward = forward;
	RecalculateTransform();
}

Elite::FMatrix4 Mesh::GetTransform() const
{
	return m_Transform;
}

void Mesh::RecalculateTransform()
{
	const FVector3 worldUp{ 0,1,0 };

	const FVector3 right{ Cross(worldUp, m_Forward) };
	const FVector3 up{ Cross(m_Forward, right) };

	m_Transform[0] = FVector4{ right, 0 };
	m_Transform[1] = FVector4{ up, 0 };
	m_Transform[2] = FVector4{ m_Forward, 0 };
	m_Transform[3] = FVector4{ m_Position.x, m_Position.y, m_Position.z, 1.f };
}
