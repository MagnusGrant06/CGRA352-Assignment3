#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "lightfield.hpp"

int main()
{
    LightField lf = LightField("rectified\\*.png", 17, 17);
    lf.print_core_part1();

    Aperture ap40(cv::Vec2f( 533.057190 ,-776.880371), 40);
    Aperture ap75(cv::Vec2f(533.057190, -776.880371), 75);

    cv::Mat raw_data = lf.reconstruct_raw_data(ap75, cv::Range(205,305), cv::Range(770,870));

    cv::imshow("dsadsa", raw_data);
    cv::waitKey(0);
}