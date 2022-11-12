#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <time.h>
#include "ConfigReader.hpp"
#include "CaptureDevice.hpp"
#include "FaceDetector.hpp"
#include "CaffeFaceDetector.hpp"
#include "OutputVideoDevice.hpp"

using namespace std;

void drawDetectedFaces(cv::Mat &frame, vector<cv::Rect2d> &faces);
cv::Mat getFirstFaceImage(cv::Mat &frame, vector<cv::Rect2d> &faces);

int main(int argc, char **argv)
{
    ConfigReader configReader;
    CaptureDevice captureDevice;
    CaffeFaceDetector caffeDetector;
    OutputVideoDevice outputVideoDevice;

    vector<cv::Rect2d> faces;
    cv::Mat faceImg;

    configReader.read("config.conf");

    captureDevice.open(configReader.getIntValue("openCVCameraIndex", 0));

    outputVideoDevice.setSize(320, 240);
    outputVideoDevice.open(configReader.getStringValue("loopbackDevide", ""));

    caffeDetector.load(
        configReader.getStringValue("caffePrototxtFilename"),
        configReader.getStringValue("caffeModelFilename"));

    while (captureDevice.captureFrame())
    {
        char c = (char)cv::waitKey(10);
        if (c == 27 || c == 'q' || c == 'Q')
        {
            break;
        }

        try
        {
            cv::Mat frame = captureDevice.getFrame();

            faces = caffeDetector.detect(frame);

            faceImg = getFirstFaceImage(frame, faces);

            cv::imshow("Display Image", faceImg);

            outputVideoDevice.writeOpenCVImage(faceImg);
        }
        catch (cv::Exception e)
        {

            cout << "Exception: " << e.msg << endl;

            continue;
        }
    }

    return 0;
}

cv::Mat getFirstFaceImage(cv::Mat &frame, vector<cv::Rect2d> &faces)
{
    cv::Mat faceImg, imgResized;
    int offset = 0;
    for (size_t i = 0; i < faces.size(); i++)
    {
        cv::Rect r;
        r.x = int((float)frame.size().width * faces[i].x);
        r.y = int((float)frame.size().height * faces[i].y);
        r.width = int((float)frame.size().width * faces[i].width);
        r.height = int((float)frame.size().height * faces[i].height);

        faceImg = frame(r);

        cv::resize(faceImg, imgResized, {320,240});

        return imgResized;
    }

    return frame;
}
