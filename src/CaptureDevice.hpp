#include <opencv2/opencv.hpp>
#include <stdio.h>
#include "Exception.hpp"

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

    cv::Mat getFrame(int rotate = 0, int flip = 0)
    {   
        if (rotate == 90) {
            cv::rotate(frame, frame, cv::ROTATE_90_CLOCKWISE);
        }
        if (rotate == -90) {
            cv::rotate(frame, frame, cv::ROTATE_90_COUNTERCLOCKWISE);
        }
        if (rotate == 180) {
            cv::rotate(frame, frame, cv::ROTATE_180);
        }        
        if (flip == 1) {
            cv::flip(frame, frame, 0);
        }                
        if (flip == 2) {
            cv::flip(frame, frame, 1);
        }                
        if (flip == 3) {
            cv::flip(frame, frame, -1);
        } 

        return frame;
    }
};