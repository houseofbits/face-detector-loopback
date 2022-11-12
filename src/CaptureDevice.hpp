#include <opencv2/opencv.hpp>
#include <stdio.h>

class CaptureDevice
{
protected:
    cv::VideoCapture capture;
    cv::Mat frame;

public:
    void open(unsigned int cameraIndex)
    {
        capture.open(cameraIndex);
        if (!capture.isOpened())
        {
            printf("Could not open capture device: %d\n", cameraIndex);
        }
    }

    bool captureFrame()
    {
        capture >> frame;

        if (frame.empty())
        {
            return false;
        }

        return true;
    }

    cv::Mat getFrame()
    {
        return frame;
    }
};