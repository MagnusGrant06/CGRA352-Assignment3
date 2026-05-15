#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "lightfield.hpp"

struct onMouseUserData {
    LightField* lf;
    Aperture* ap;
    cv::Mat out_image;
};

static void onMouse(int mouse_action, int x, int y, int flags, void* userdata) {
    if (mouse_action != cv::EVENT_LBUTTONDOWN) return;

    userdata = (onMouseUserData*)userdata;
    LightField* lf = ((onMouseUserData*)userdata)->lf;
    Aperture* ap = ((onMouseUserData*)userdata)->ap;
    cv::Mat& out_image = ((onMouseUserData*)userdata)->out_image;

    cv::Rect r(x-2, y-2, 5, 5);
    float min_cost = FLT_MAX;
    float min_dist = 0;
    for (float f = -0.5; f < 0.0; f += 0.005) {
        float cost = lf->calculate_focal_cost(r, *ap, f);
        if (cost < min_cost) {
            min_cost = cost;
            min_dist = f;
        }
    }

    out_image = lf->reconstruct(*ap, min_dist);
   
    cv::circle(out_image, cv::Point(x, y), 7, cv::Scalar(0, 0, 255), 2);
    cv::imshow("focus", out_image);
}

int main()
{   
    //core
    LightField lf = LightField("rectified\\*.png", 17, 17);
    lf.print_core_part1();

    Aperture ap40(cv::Vec2f( 533.057190 ,-776.880371), 40);
    Aperture ap75(cv::Vec2f(533.057190, -776.880371), 75);

    cv::Mat raw_data = lf.reconstruct_raw_data(ap75, cv::Range(205,305), cv::Range(770,870));

    cv::imshow("dsadsa", raw_data);
    cv::waitKey(0);

    //completion
    cv::Mat focal_reconstruction = lf.reconstruct(ap40, -0.5);
    cv::imshow("farthest", focal_reconstruction);
    cv::Mat focal_reconstruction2 = lf.reconstruct(ap40, -0.4);
    cv::imshow("2nd farthest", focal_reconstruction2);
    cv::Mat focal_reconstruction3 = lf.reconstruct(ap40, -0.3);
    cv::imshow("middle distance", focal_reconstruction3);
    cv::Mat focal_reconstruction4 = lf.reconstruct(ap40, -0.1);
    cv::imshow("2nd closest", focal_reconstruction4);
    cv::Mat focal_reconstruction5 = lf.reconstruct(ap40, 0.01);
    cv::imshow("closest", focal_reconstruction5);
    cv::waitKey(0);

    //challenge
    cv::Mat out_image = lf.reconstruct(ap40, 0.0);
    onMouseUserData userdata;
    userdata.lf = &lf;
    userdata.ap = &ap40;
    userdata.out_image = out_image;
    
    cv::namedWindow("focus", cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback("focus", onMouse, &userdata);
    while (true) {
        cv::imshow("focus", out_image);
        int key = cv::waitKey(1);
        if (key == 'q') break;
    }


}