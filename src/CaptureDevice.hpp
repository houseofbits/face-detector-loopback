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
            throw new Exception("Could not open capture device" + cameraIndex);
        }
    }

    void captureFrame()
    {
        capture >> frame;

        if (frame.empty())
        {
            throw new Exception("Captured frame is empty");
        }
    }

    cv::Mat getFrame()
    {
        return frame;
    }
};