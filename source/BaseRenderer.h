#pragma once
#ifndef ELITE_RAYTRACING_RENDERER_BASE
#define	ELITE_RAYTRACING_RENDERER_BASE

#include <cstdint>

struct SDL_Window;
struct SDL_Surface;

namespace Elite
{
	class BaseRenderer
	{
	public:
		explicit BaseRenderer(SDL_Window* pWindow);
		virtual ~BaseRenderer() = default;

		BaseRenderer(const BaseRenderer&) = delete;
		BaseRenderer(BaseRenderer&&) noexcept = delete;
		BaseRenderer& operator=(const BaseRenderer&) = delete;
		BaseRenderer& operator=(BaseRenderer&&) noexcept = delete;

		virtual void Render() = 0;

	protected:
		SDL_Window* m_pWindow;
		uint32_t m_Width;
		uint32_t m_Height;
		RGBColor m_ClearColor{ 0.1f, 0.1f, 0.1f };
	};
}
#endif