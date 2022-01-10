#include "pch.h"
#include "SoftwareRenderer.h"

//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "ERGBColor.h"
#include "SceneManager.h"
#include "MathHelper.h"

SoftwareRenderer::SoftwareRenderer(SDL_Window* pWindow, Texture* pDiffuse, Texture* pNormal)
	: BaseRenderer(pWindow)
	, m_pTexture(pDiffuse)
	, m_pNormalMap(pNormal)
{
	//Initialize
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = static_cast<uint32_t*>(m_pBackBuffer->pixels);

	m_DepthBuffer.resize(m_Width * m_Height, 1.0f);
}

SoftwareRenderer::~SoftwareRenderer()
{
	delete m_pTexture;
	delete m_pNormalMap;
}

void SoftwareRenderer::Render()
{
	SDL_LockSurface(m_pBackBuffer);

	const SceneGraph& activeScene{ SceneManager::GetInstance().GetActiveScene() };

	// Clear buffer, could this be done better?
	for (uint32_t row = 0; row < m_Height; ++row)
	{
		for (uint32_t col = 0; col < m_Width; ++col)
		{
			m_pBackBufferPixels[PixelToBufferIndex(col, row, m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
				0,
				0,
				0);
		}
	}

	for (const Geometry* geometry : activeScene.GetGeometries())
	{
		std::vector<Vertex> geometryVertices{ geometry->GetModelVertices() };
		geometry->Project(geometryVertices);

		std::vector<Vertex> outVertices{};
		geometry->Rasterize(geometryVertices, m_DepthBuffer, outVertices);

		for (const Vertex& vertex : outVertices)
		{
			RGBColor finalPixelColor;
			if (m_RenderDepthBuffer)
			{
				finalPixelColor = RGBColor{ vertex.position.z, vertex.position.z , vertex.position.z };
			}
			else
			{
				// PixelShading is broken for now 
				// Todo: fix it could be tangent, normal, tangent space, or just the damn light, I do not know
				finalPixelColor = PixelShading(vertex);
				//finalPixelColor = m_pTexture.Sample(vertex.uv);
			}

			m_pBackBufferPixels
				[
					PixelToBufferIndex
					(
						static_cast<unsigned int>(roundf(vertex.position.x)),
						static_cast<unsigned int>(roundf(vertex.position.y)),
						m_Width
					)
				] = SDL_MapRGB(m_pBackBuffer->format,
					static_cast<Uint8>(m_ClearColor.r * 255.f),
					static_cast<Uint8>(m_ClearColor.g * 255.f),
					static_cast<Uint8>(m_ClearColor.b * 255.f));
		}
	}

	std::fill(m_DepthBuffer.begin(), m_DepthBuffer.end(), 1.0f);

	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

bool SoftwareRenderer::SaveBackbufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "BackbufferRender.bmp");
}

RGBColor SoftwareRenderer::PixelShading(const Vertex& outVertex) const
{
	// WIP, broken

	// Light constants
	const FVector3 lightDirection{ .577f, -.577f, -.577f };
	const RGBColor lightColor{ 1.f, 1.f, 1.f };
	constexpr float intensity{ 2.f };

	// Normal Mapping
	//const FVector3 binormal{ Cross(outVertex.tangent, outVertex.normal) };
	//const FMatrix3 tangentSpaceAxis{ outVertex.tangent, binormal, outVertex.normal };
	//
	//RGBColor sampledValue{ m_pNormalMap.Sample(outVertex.uv) };
	//sampledValue /= 255.f;
	//sampledValue.r = 2.f * sampledValue.r - 1.f;
	//sampledValue.g = 2.f * sampledValue.g - 1.f;
	//sampledValue.b = 2.f * sampledValue.b - 1.f;
	//FVector3 newNormal{ sampledValue.r, sampledValue.g, sampledValue.b };
	//newNormal = tangentSpaceAxis * newNormal;
	//
	//const float observedArea{ Dot(-newNormal, lightDirection) };

	// Normal mapping seems to have an issue *somewhere*, below is the old method as a temporary fix

	const float observedArea{ Dot(-outVertex.normal, lightDirection) };

	// Diffuse map color
	// Todo: lambert
	const RGBColor diffuseColor{ m_pTexture->Sample(outVertex.uv) };

	// Todo: Specular
	// Todo: Ambient

	return lightColor * intensity * diffuseColor * observedArea;
}

void SoftwareRenderer::ToggleRenderDepthBuffer()
{
	m_RenderDepthBuffer = !m_RenderDepthBuffer;
}
