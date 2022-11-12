#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <time.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include "ConfigReader.hpp"
#include "OutputVideoDevice.hpp"
#include "CaptureDevice.hpp"

using namespace cv;
using namespace std;

int main(int argc, char **argv)
{
    ConfigReader configReader;
    OutputVideoDevice outputVideoDevice;
    CaptureDevice captureDevice;
    
    clock_t start;
    clock_t end;
    double fps;

    configReader.read("config.conf");
    int cameraIndex = configReader.getIntValue("openCVCameraIndex", 0);
    string loopbackDevide = configReader.getStringValue("loopbackDevide", "");

    outputVideoDevice.open(loopbackDevide);

    captureDevice.open(cameraIndex);

    while (captureDevice.captureFrame())
    {
        end = clock();

        char c = (char)waitKey(10);
        // Press q to exit from window
        if (c == 27 || c == 'q' || c == 'Q')
        {
            break;
        }

        double seconds = (double(end) - double(start)) / double(CLOCKS_PER_SEC);
        fps = 1.0d / double(seconds);

        Mat frame = captureDevice.getFrame();

        putText(frame, "FPS: " + to_string(int(fps)), {10, 20}, FONT_HERSHEY_SIMPLEX, 0.6, {255, 0, 0}, 2);

        putText(frame, "Size: " + to_string(int(frame.size().width)) + ", " + to_string(int(frame.size().height)), {10, 37}, FONT_HERSHEY_SIMPLEX, 0.6, {255, 0, 0}, 2);

        imshow("Display Image", frame);

        outputVideoDevice.writeOpenCVImage(frame);

        start = clock();
    }

    outputVideoDevice.close();

    return 0;
}