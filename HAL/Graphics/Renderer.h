#pragma once

#ifndef DISABLE_SDL

#include <vector>

#include <glm/fwd.hpp>

#include "EngineData/Geometry/Vector2D.h"

#include "HAL/Base/Types.h"

struct SDL_Renderer;

namespace HAL::Internal
{
	class Window;
} // namespace HAL::Internal

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
	} // namespace Render
} // namespace Graphics

#endif // !DISABLE_SDL
