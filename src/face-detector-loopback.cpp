#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <time.h>
#include "ConfigReader.hpp"
#include "CaptureDevice.hpp"
#include "FaceDetector.hpp"
#include "CaffeFaceDetector.hpp"
#include "OutputVideoDevice.hpp"
#include "DetectionResultInterpolator.hpp"

using namespace std;

cv::Mat getImageFromRect(cv::Mat &frame, cv::Rect2d rect);

int main(int argc, char **argv)
{
    ConfigReader configReader;
    CaptureDevice captureDevice;
    CaffeFaceDetector caffeDetector;
    OutputVideoDevice outputVideoDevice;
    DetectionResultInterpolator detectionResultInterpolator;
    vector<cv::Rect2d> faces;
    cv::Mat faceImg;
    clock_t start;
    clock_t end;

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
            end = clock();

            double frameTime = (double(end) - double(start)) / double(CLOCKS_PER_SEC);

            frameTime = fmax(0.1, frameTime);

            cv::Mat frame = captureDevice.getFrame();

            faces = caffeDetector.detect(frame);

            cv::Rect2d rect = detectionResultInterpolator.getInterpolatedResult(faces, frameTime);

            faceImg = getImageFromRect(frame, rect);
            
            cv::imshow("Display Image", faceImg);

            outputVideoDevice.writeOpenCVImage(faceImg);

            start = clock();
        }
        catch (cv::Exception e)
        {

            cout << "Exception: " << e.msg << endl;

            continue;
        }
    }

    return 0;
}

cv::Mat getImageFromRect(cv::Mat &frame, cv::Rect2d rect)
{
    cv::Mat faceImg, imgResized;
    cv::Rect r;
    int offset = 0;
    float expandBy = 0.2;
    float fixedAspectRatio = 1.4;
    float frameWidth = frame.size().width;
    float frameHeight = frame.size().height;

    r.x = int(frameWidth * rect.x);
    r.y = int(frameHeight * rect.y);
    r.width = int(frameWidth * rect.width);
    r.height = int(frameHeight * rect.height);
    float expand = (float)r.width * expandBy;

    r.x = r.x - expand;
    r.y = r.y - expand;
    r.width = r.width + expand * 2;
    r.height = r.height + expand * 2;

    float newWidth = r.height / fixedAspectRatio;

    r.x = r.x + ((r.width - newWidth) / 2);
    r.width = newWidth;

    float offsetex = 0;
    if (r.x + r.width > frameWidth)
    {
        offsetex = fabs(frameWidth - (r.x + r.width));
        r.width = frameWidth - r.x;
    }
    float offsetey = 0;
    if (r.y + r.height > frameHeight)
    {
        offsetey = fabs(frameHeight - (r.y + r.height));
        r.height = frameHeight - r.y;
    }
    float offsetsx = 0;
    if (r.x < 0)
    {
        offsetsx = fabs(r.x);
        r.x = 0;
    }
    float offsetsy = 0;
    if (r.y < 0)
    {
        offsetsy = fabs(r.y);
        r.y = 0;
    }

    faceImg = frame(r);

    cv::Mat largerImage = cv::Mat::zeros(cv::Size(offsetsx + r.width + offsetex, offsetsy + r.height + offsetey), frame.type());
    cv::Rect rFace({(int)offsetsx, (int)offsetsy}, faceImg.size());

    faceImg.copyTo(largerImage(rFace));

    cv::resize(largerImage, imgResized, {320, 240});

    return imgResized;
}