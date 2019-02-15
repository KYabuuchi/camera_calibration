#include "calibration/calibration.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char* argv[])
{
    cv::CommandLineParser parser(
        argc,
        argv,
        "{i input|../reference/pseye_000/files.txt| path to paths file for input images.}"
        "{o output|./|path to directory for captured images.}"
        "{x xml|calibration.xml| path to calibration file.}"
        "{d device v video|/dev/video0| path to camera device.}"
        "{m mode|calib|'calib' or 'photocalib' or 'rectify' or 'read'}"
        "{r row |10|row(行) size of chess board}"
        "{c col |7|col(列) size of chess board}"
        "{s size|19.5|size of each square side on chess board [mm]}"
        "{h help ?|false|Use like below.\n"
        "$./sample -m=calib -i=../reference/pseye/files.txt -x=calibration.xml -w=10 -c=7 -s=19.5 #calibration by using exsinting image\n"
        "$./sample -m=photocalib -d=/dev/video0 -o=./ -x=calibration.xml -w=10 -c=7 -s=19.5 #shot some pictures & calibration by using them\n"
        "$./sample -m=rectify -v=/dev/video0 -x=calibration.xml #rectify video stream by calibration data\n"
        "$./sample -m=read -x=calibration.xml #read & print calibration data}");
    if (parser.get<bool>("help")) {
        parser.printMessage();
        return 0;
    }

    int row = parser.get<int>("row");
    int col = parser.get<int>("col");
    float size = parser.get<float>("size");
    Camera::Calibration camera(row, col, size);

    std::string mode = parser.get<std::string>("mode");
    if (mode == "calib") {
        std::string paths_file_path = parser.get<std::string>("input");
        std::string xml_path = parser.get<std::string>("xml");
        camera.calcParameters(paths_file_path, xml_path);

    } else if (mode == "photocalib") {
        std::string device_path = parser.get<std::string>("device");
        std::string xml_path = parser.get<std::string>("xml");
        std::string output_dir = parser.get<std::string>("output");
        camera.calcParametersWithPhoto(output_dir, xml_path, device_path);

    } else if (mode == "rectify") {
        std::string device_path = parser.get<std::string>("device");
        std::string xml_path = parser.get<std::string>("xml");
        camera.readParameters(xml_path);
        cv::VideoCapture video(device_path);
        if (not video.isOpened()) {
            std::cout << "can not open " << device_path << std::endl;
            return -1;
        }
        cv::namedWindow("window", cv::WINDOW_NORMAL);
        while (true) {
            cv::Mat src, dst, show;
            video >> src;
            dst = camera.rectify(src);
            cv::hconcat(src, dst, show);
            cv::imshow("window", show);
            if (cv::waitKey(10) == 'q')
                break;
        }
    } else if (mode == "load") {
        std::string xml_path = parser.get<std::string>("xml");
        camera.readParameters(xml_path);
        camera.showParameters();

    } else {
        parser.printMessage();
        std::cout << "invlid arguments" << std::endl;
    }

    return 0;
}