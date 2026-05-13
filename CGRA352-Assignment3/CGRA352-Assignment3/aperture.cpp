#include "aperture.hpp"

bool Aperture::transparent(cv::Vec2f uv) {
	float dx = uv[0] - center[0];
	float dy = uv[1] - center[1];

	float dist = sqrt(dx * dx + dy * dy);
	return dist <= radius;
}