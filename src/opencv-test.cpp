#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <time.h>
#include "ConfigReader.hpp"
#include "CaptureDevice.hpp"

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    ConfigReader configReader;
    CaptureDevice captureDevice;

    clock_t start;
    clock_t end;
    double fps;

    configReader.read("config.conf");
    
    int cameraIndex = configReader.getIntValue("openCVCameraIndex", 0);

    captureDevice.open(cameraIndex);

    while (captureDevice.captureFrame())
    {
        end = clock();

        char c = (char)waitKey(10);
        // Press q to exit from window
        if( c == 27 || c == 'q' || c == 'Q' ) {
            break;            
        }

        double seconds =  (double(end) - double(start)) / double(CLOCKS_PER_SEC);
        fps = 1.0d / double(seconds);

        Mat frame = captureDevice.getFrame();

        putText(frame, "FPS: " + to_string(int(fps)), { 10, 20 }, FONT_HERSHEY_SIMPLEX, 0.7, {255, 0, 0},2);
        imshow("Display Image", frame);

        start = clock();
    }

    return 0;
}