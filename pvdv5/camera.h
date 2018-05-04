#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include <QTimer>
#include "filedatabase.h"
#include "videosource.h"
#include "processeddatasender.h"
#include "pvdprocessor.h"
#include "pvdhogprocessor.h"
#include <mutex>
using namespace cv;
class Camera : public QThread
{
    Q_OBJECT
    typedef struct alg{
        JsonValue pvd_c4;
        JsonValue pvd_hog;
        string selected_alg;
    }alg_t;

    typedef struct Camera_config{
        string url;
        int direction;
        int camera_id;
        string user_name;
        string password;
        string camera_ip;
        int camera_port;
        alg_t alg;
    }Config_t;
    Rect detect_rect;
public:
    Camera(JsonValue jv);
    ~Camera()
    {
        stop_cam();
    }
    bool modify_alg(JsonValue jv);
    void modify_attr(JsonValue v);
    void add_watcher(QString ip);
    void del_watcher(QString ip);
    Rect area_2_rect(QJsonValue area)
    {
        int x_min=10000;
        int y_min=10000;
        int x_max=0;
        int y_max=0;
        foreach (QJsonValue v, area.toArray()) {
            int x= v.toObject()["x"].toInt();
            int y= v.toObject()["y"].toInt();
            if(x<x_min)
                x_min=x;
            if(x>x_max)
                x_max=x;
            if(y<y_min)
                y_min=y;
            if(y>y_max)
                y_max=y;
        }
        return Rect(x_min,y_min,x_max-x_min,y_max-y_min);
    }

    inline JsonValue config()
    {
        return cfg_2_jv();
    }
private:
    void restart_processor()
    {
        mtx.lock();
        string str=cam_cfg.alg.selected_alg;
        if(processor)
            delete processor;
        if(str=="pvd_c4"){
            processor=new PvdC4Processor(cam_cfg.alg.pvd_c4);
        }else if(str=="pvd_hog"){
            processor=new PvdHogProcessor(cam_cfg.alg.pvd_hog);
        }
        mtx.unlock();
    }

    void start_cam()
    {
        src=new VideoSource(cam_cfg.url);
        restart_processor();
        start();
    }

    void stop_cam()
    {
        delete tmr;
        quit=true;
        prt(info,"stoping camera..");
        this->wait();//TODO, maybe we dont need wait?
        prt(info," camera %s stoped",this->src->get_url().data());
        delete src;
        delete processor;
        src=NULL;
        processor=NULL;
    }

    virtual JsonValue cfg_2_jv()
    {
        DataPacket pkt;
        pkt.set_string("url",cam_cfg.url);
        pkt.set_int("direction",cam_cfg.direction);
        pkt.set_int("camera_id",cam_cfg.camera_id);
        pkt.set_string("user_name",cam_cfg.user_name);
        pkt.set_string("password",cam_cfg.password);
        pkt.set_string("camera_ip",cam_cfg.camera_ip);
        pkt.set_int("camera_port",cam_cfg.camera_port);

        DataPacket pkt_alg;
        pkt_alg.set_string("selected_alg",cam_cfg.alg.selected_alg);
        pkt_alg.set_value("pvd_c4",cam_cfg.alg.pvd_c4);
        pkt_alg.set_value("pvd_hog",cam_cfg.alg.pvd_hog);
        pkt.set_value("alg",pkt_alg.value());

        return pkt.value();
    }

    virtual void jv_2_cfg(JsonValue cfg)
    {
        DataPacket pkt(cfg);
        cam_cfg.url = pkt.get_string("url");
        cam_cfg.direction=pkt.get_int("direction");
        cam_cfg.camera_id=pkt.get_int("camera_id");
        cam_cfg.user_name=pkt.get_string("user_name");
        cam_cfg.password=pkt.get_string("password");
        cam_cfg.camera_ip=pkt.get_string("camera_ip");
        cam_cfg.camera_port=pkt.get_int("camera_port");
        JsonValue alg=pkt.get_value("alg");
        DataPacket pkt_alg(alg);
        cam_cfg.alg.selected_alg=pkt_alg.get_string("selected_alg");
        cam_cfg.alg.pvd_c4=pkt_alg.get_value("pvd_c4");
        cam_cfg.alg.pvd_hog=pkt_alg.get_value("pvd_hog");
    }

    void send_out(string ba)
    {
        //  emit output(ba);
        ProcessedDataSender *s=ProcessedDataSender::get_instance();
        foreach (QString ip, ip_list) {
            s->send(ba.data(),QHostAddress(ip));
        }
    }

protected:
    void run();
signals:
public slots:
    inline void handle_time_up()
    {
        frame_rate=0;
    }
private:
    QList <QString> ip_list;
    int frame_rate;
    int threadid;
    QTimer *tmr;
    VideoSource *src;
    VideoProcessor *processor;
    Config_t cam_cfg;
    bool quit;
    mutex mtx;
};

#endif // CAMERA_H
