#pragma once

#ifndef DISABLE_SDL

#include <array>
#include <vector>

#include <glm/fwd.hpp>

#include "EngineData/Geometry/Vector2D.h"

#include "HAL/Base/Types.h"

struct SDL_Renderer;

namespace HAL
{
	namespace Internal
	{
		class Window;
	}
}

namespace Graphics
{
	class Font;
	class Texture;
	class Surface;

	namespace Render
	{
		void BindSurface(const Surface& surface);
		void DrawQuad(const glm::mat4& transform, Vector2D size, QuadUV uv, float alpha = 1.0f);
		void DrawQuad(Vector2D pos, Vector2D size);
		void DrawQuad(Vector2D pos, Vector2D size, Vector2D anchor, float rotation, QuadUV uv, float alpha = 1.0f);
		void DrawFan(const std::vector<DrawPoint>& points, const glm::mat4& transform, float alpha);
		void DrawStrip(const std::vector<DrawPoint>& points, const glm::mat4& transform, float alpha);
		void DrawTiledQuad(Vector2D start, Vector2D size, const QuadUV& uv);
	}

	class Renderer
	{
	public:
		Renderer() = default;

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		Renderer& operator=(Renderer&&) = delete;

		void renderText(const Font& font, Vector2D pos, Color color, const char* text);
		std::array<int, 2> getTextSize(const Font& font, const char* text);
	};
}

#endif // !DISABLE_SDL
