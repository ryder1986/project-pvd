#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include "tool.h"
#include "pvd.h"
using namespace std;
using namespace cv;
class VideoProcessor
{
public:
    string alg_rst;
    VideoProcessor()
    {
    }
    virtual  void set_config(JsonValue v)
    {
    }

    int get_id()
    {
        return channel_id;
    }
    virtual  bool process( Mat img)
    {
        return false;
    }
    virtual  string get_rst()
    {
        string ba;
        return ba ;
    }
    virtual void init()
    {
    }
protected:
    int channel_id;
private:

};
#endif // VIDEOPROCESSOR_H
