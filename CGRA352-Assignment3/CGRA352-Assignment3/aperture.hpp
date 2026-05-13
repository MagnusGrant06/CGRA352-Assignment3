#include <opencv2/opencv.hpp>

class Aperture {
	
private:
	int radius;
	cv::Vec2f center;

public:
	Aperture(cv::Vec2f center, int radius) : center(center), radius(radius) {}

	bool transparent(cv::Vec2f uv);

};