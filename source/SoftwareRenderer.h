#pragma once
#ifndef ELITE_RAYTRACING_RENDERER_SOFTWARE
#define	ELITE_RAYTRACING_RENDERER_SOFTWARE

#include "BaseRenderer.h"
#include <cstdint>
#include <vector>

#include "Texture.h"
#include "Vertex.h"

struct SDL_Window;
struct SDL_Surface;

namespace Elite
{
	class SoftwareRenderer final : public BaseRenderer
	{
	public:
		explicit SoftwareRenderer(SDL_Window* pWindow, Texture* pDiffuse, Texture* pNormal);
		~SoftwareRenderer() override;

		SoftwareRenderer(const SoftwareRenderer&) = delete;
		SoftwareRenderer(SoftwareRenderer&&) noexcept = delete;
		SoftwareRenderer& operator=(const SoftwareRenderer&) = delete;
		SoftwareRenderer& operator=(SoftwareRenderer&&) noexcept = delete;

		void Render() override;
		bool SaveBackbufferToImage() const;

		RGBColor PixelShading(const Vertex& outVertex) const;

		void ToggleRenderDepthBuffer();

	private:
		SDL_Surface* m_pFrontBuffer = nullptr;
		SDL_Surface* m_pBackBuffer = nullptr;
		uint32_t* m_pBackBufferPixels = nullptr;

		std::vector<float> m_DepthBuffer;

		Texture* m_pTexture;
		Texture* m_pNormalMap;

		bool m_RenderDepthBuffer = false;
	};
}

#endif