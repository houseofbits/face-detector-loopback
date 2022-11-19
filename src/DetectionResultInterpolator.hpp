#include <stdio.h>
#include <fstream>
#include <opencv2/opencv.hpp>

using namespace std;

class DetectionResultInterpolator
{
protected:
    cv::Rect2d currentRect;
    cv::Rect2d targetRect;

private:
    float calculateInterpolatedValue(float current, float target, float step)
    {
        float diff = target - current;

        return current + diff * step;
    }

public:
    DetectionResultInterpolator()
    {
        targetRect.x = 0.4;
        targetRect.y = 0.4;
        targetRect.width = 0.2;
        targetRect.height = 0.2;

        currentRect = targetRect;
    }

    cv::Rect2d getInterpolatedResult(std::vector<cv::Rect2d> detectedRects, float frameTime)
    {
        if (!detectedRects.empty())
        {
            targetRect = getTargetRect(detectedRects);
        }

        float step = 1.0 * frameTime;

        currentRect.x = calculateInterpolatedValue(currentRect.x, targetRect.x, step);
        currentRect.y = calculateInterpolatedValue(currentRect.y, targetRect.y, step);
        currentRect.width = calculateInterpolatedValue(currentRect.width, targetRect.width, step);
        currentRect.height = calculateInterpolatedValue(currentRect.height, targetRect.height, step);

        return currentRect;
    }

    cv::Rect2d getTargetRect(std::vector<cv::Rect2d> detectedRects)
    {
        if (detectedRects.size() == 0)
        {
            throw new Exception("No detected objects");
        }
        else if(detectedRects.size() == 1)
        {
            return detectedRects[0];
        }

        float currentArea = currentRect.area();

        float prevIntersectionFactor = 0.0;
        int index = 0;

        for (int i = 0; i < detectedRects.size(); i++)
        {
            float area = detectedRects[i].area();

            cv::Rect2d inter = currentRect & detectedRects[i];

            if (inter.area() > 0)
            {
                float factor = inter.area() / currentArea;
                if (factor > prevIntersectionFactor)
                {
                    prevIntersectionFactor = factor;
                    index = i;
                }
            }
        }

        return detectedRects[index];
    }

    void printRect2d(std::string name, cv::Rect2d r)
    {
        printf("%s: %f,%f %f,%f\n", name.c_str(), r.x, r.y, r.width, r.height);
    }
};