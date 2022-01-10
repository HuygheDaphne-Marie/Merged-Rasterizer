#include "pch.h"
#include "Material.h"

#include <sstream>

#include "SceneManager.h"

Material::Material(ID3D11Device* pDevice, const std::wstring& path)
	: m_pEffect(nullptr)
{
	m_pEffect = LoadEffect(pDevice, path);
	if (m_pEffect == nullptr || !m_pEffect->IsValid())
	{
		std::cout << "Material is invalid";
		return;
	}

	m_pTechniques.push_back(LoadTechnique("DefaultTechnique"));
	m_pTechniques.push_back(LoadTechnique("LinearTechnique"));
	m_pTechniques.push_back(LoadTechnique("AnisotropicTechnique"));
}
Material::~Material()
{
	for (auto* pTechnique : m_pTechniques)
	{
		pTechnique->Release();
	}
	m_pEffect->Release();
}

ID3DX11Effect* Material::GetEffect() const
{
	return m_pEffect;
}
ID3DX11EffectTechnique* Material::GetTechnique() const
{
	return m_pTechniques[m_CurrentTechniqueIndex];
}

void Material::GotoNextTechnique()
{
	++m_CurrentTechniqueIndex;
	m_CurrentTechniqueIndex %= m_pTechniques.size();
}

// Base material update, will only set the WorldViewProjection
void Material::Update()
{
	const Camera* pCamera = SceneManager::GetInstance().GetActiveScene().GetCamera();
	SetEffectMatrix("gWorldViewProj", m_pWorldViewProjVariable, pCamera->GetLHProjection() * pCamera->GetLHWorldToView());

	// Rest has to be done by subclasses in their overloaded Update() function
}

ID3DX11Effect* Material::LoadEffect(ID3D11Device* pDevice, const std::wstring& path)
{
	HRESULT result = S_OK;
	ID3D10Blob* pErrorBlob = nullptr;
	ID3DX11Effect* pEffect;

	DWORD shaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DX11CompileEffectFromFile(path.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		pDevice,
		&pEffect,
		&pErrorBlob);

	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			auto* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());

			std::wstringstream ss;
			for (unsigned int i{ 0 }; i < pErrorBlob->GetBufferSize(); i++)
				ss << pErrors[i];

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			std::wcout << ss.str() << std::endl;
		}
		else
		{
			std::wstringstream ss;
			std::wcout << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << path << std::endl;
			return nullptr;
		}
	}

	std::cout << "Material loaded successfully\n";

	return pEffect;
}
ID3DX11EffectTechnique* Material::LoadTechnique(const std::string& techniqueName) const
{
	ID3DX11EffectTechnique* technique = m_pEffect->GetTechniqueByName(techniqueName.c_str());
	if (technique == nullptr || !technique->IsValid())
	{
		std::cout << "Technique: '" << techniqueName << "' is invalid";
		return nullptr;
	}

	return technique;
}

// Generalized functions for setting shader variables, don't make these const
void Material::SetEffectMatrix(const std::string& matrixVariableName, ID3DX11EffectMatrixVariable* pMeshMatrix,
	const Elite::FMatrix4& pMeshMatrixValue)
{
	// Get matrix
	pMeshMatrix = m_pEffect->GetVariableByName(matrixVariableName.c_str())->AsMatrix();
	if (!pMeshMatrix->IsValid())
	{
		std::cout << matrixVariableName << " is not valid\n";
		return;
	}

	// Set matrix if it's valid
	const HRESULT res = pMeshMatrix->SetMatrix(*pMeshMatrixValue.data);
	if (FAILED(res))
	{
		std::cout << "Issue setting " << matrixVariableName << std::endl;
		return;
	}
}
void Material::SetEffectShaderResource(const std::string& shaderVariableName, ID3DX11EffectShaderResourceVariable* pMeshShaderResource,
	Texture* pMeshShaderResourceValue)
{
	// Get the shader resource  variable
	pMeshShaderResource = m_pEffect->GetVariableByName(shaderVariableName.c_str())->AsShaderResource();
	if (!pMeshShaderResource->IsValid())
	{
		std::cout << shaderVariableName << " is not valid\n";
		return;
	}

	// Set the diffuse variable
	if (pMeshShaderResource->IsValid() && pMeshShaderResourceValue != nullptr)
	{
		pMeshShaderResource->SetResource(pMeshShaderResourceValue->GetTextureResourceView());
	}
}