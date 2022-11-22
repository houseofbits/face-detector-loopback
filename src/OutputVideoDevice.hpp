#include <stdio.h>
#include <linux/videodev2.h>
#include <opencv2/opencv.hpp>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>

#define ROUND_UP_2(num) (((num) + 1) & ~1)
#define ROUND_UP_4(num) (((num) + 3) & ~3)
#define ROUND_UP_8(num) (((num) + 7) & ~7)
#define ROUND_UP_16(num) (((num) + 15) & ~15)
#define ROUND_UP_32(num) (((num) + 31) & ~31)
#define ROUND_UP_64(num) (((num) + 63) & ~63)

class OutputVideoDevice
{
protected:
    unsigned int frameWidth;
    unsigned int frameHeight;
    unsigned int frameFormat;
    size_t frameSize;
    size_t bytesPerLine;
    __u8 *buffer;
    int fdwr;

public:
    OutputVideoDevice()
    {
        frameWidth = 640;
        frameHeight = 480;
        frameFormat = V4L2_PIX_FMT_YUV420;
    }

    void setSize(int width, int height)
    {
        frameWidth = width;
        frameHeight = height;
    }

    void open(string deviceName)
    {
        struct v4l2_capability vid_caps;
        struct v4l2_format vid_format;

        try
        {
            fdwr = ::open(deviceName.c_str(), O_RDWR);
            if (fdwr < 0)
            {
                throw new Exception("Could not open output device:" + deviceName);
            }

            int ret_code = ioctl(fdwr, VIDIOC_QUERYCAP, &vid_caps);
            if (ret_code == -1)
            {
                throw new Exception("Capability query failed for output device:" + deviceName);
            }

            memset(&vid_format, 0, sizeof(vid_format));

            ret_code = ioctl(fdwr, VIDIOC_G_FMT, &vid_format);

            vid_format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
            vid_format.fmt.pix.width = frameWidth;
            vid_format.fmt.pix.height = frameHeight;
            vid_format.fmt.pix.pixelformat = frameFormat;
            vid_format.fmt.pix.sizeimage = frameSize;
            vid_format.fmt.pix.field = V4L2_FIELD_NONE;
            vid_format.fmt.pix.bytesperline = bytesPerLine;
            vid_format.fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;

            ret_code = ioctl(fdwr, VIDIOC_S_FMT, &vid_format);
            if (ret_code == -1)
            {
                throw new Exception("Setting format failed for output device:" + deviceName);
            }

            if (!createFormatProperties(vid_format))
            {
                throw new Exception("unable to guess correct settings for format " + frameFormat);
            }

            buffer = (__u8 *)malloc(sizeof(__u8) * frameSize);

            memset(buffer, 0, frameSize);
            for (int i = 0; i < frameSize; ++i)
            {
                buffer[i] = rand() % 255;
            }

            write(fdwr, buffer, frameSize);
        }
        catch (Exception *e)
        {
            cout << e->getMessage() << endl;
        }
    }

    void close()
    {
        ::close(fdwr);
        free(buffer);
    }

    void writeOpenCVImage(cv::Mat image)
    {
        cv::Mat converted;

        cv::cvtColor(image, converted, cv::COLOR_BGRA2YUV_I420);

        if (image.size().width > frameWidth || image.size().height > frameHeight)
        {
            cv::Rect r;
            r.x = 0;
            r.y = 0;
            r.width = min<unsigned int>(frameWidth, image.size().width);
            r.height = min<unsigned int>(frameHeight, image.size().height);

            converted = converted(r);
        }

        ::write(fdwr, converted.data, frameSize);
    }

private:
    bool createFormatProperties(v4l2_format vidFormat)
    {
        unsigned int format = vidFormat.fmt.pix.pixelformat;
        unsigned int width = vidFormat.fmt.pix.width;
        unsigned int height = vidFormat.fmt.pix.height;

        size_t lw, fw;
        switch (format)
        {
        case V4L2_PIX_FMT_YUV420:
        case V4L2_PIX_FMT_YVU420:
            lw = width; /* ??? */
            fw = ROUND_UP_4(width) * ROUND_UP_2(height);
            fw += 2 * ((ROUND_UP_8(width) / 2) * (ROUND_UP_2(height) / 2));
            break;
        case V4L2_PIX_FMT_UYVY:
        case V4L2_PIX_FMT_Y41P:
        case V4L2_PIX_FMT_YUYV:
        case V4L2_PIX_FMT_YVYU:
            lw = (ROUND_UP_2(width) * 2);
            fw = lw * height;
            break;
        default:
            return false;
        }

        bytesPerLine = lw;
        frameSize = fw;

        return true;
    }
};