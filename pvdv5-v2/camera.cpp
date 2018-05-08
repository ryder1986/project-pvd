#include "camera.h"
Camera::Camera(JsonValue jv)
{
    frame_rate=0;
    quit=false;
    jv_2_cfg(jv);
    start_cam();
    tmr=new QTimer;
    connect (tmr,SIGNAL(timeout()),this,SLOT(handle_time_up()));
    tmr->start(1000);
}

bool Camera::modify_alg(JsonValue jv)
{

    DataPacket pkt(jv);
    int no= pkt.get_int("modify_channel_no");
    cam_cfg.chs[no-1]=pkt.get_value("chnanel_data");
    restart_processor();
    return true;
}

void Camera::modify_attr(JsonValue v)
{
    DataPacket pkt(v);
    int di= pkt.get_int("direction");
    int no= pkt.get_int("camera_id");
    cam_cfg.direction=di;
    cam_cfg.camera_id=no;
}

void Camera::add_watcher(QString ip)
{
    if(ip_list.contains(ip)){

    }else{
        prt(info,"client %s require output",ip.toStdString().data());
        ip_list.append(ip);
    }

}

void Camera::del_watcher(QString ip)
{
    if(ip_list.contains(ip)){
        prt(info,"client %s stop output",ip.toStdString().data());
        ip_list.removeOne(ip);
    }else{
    }
}

void Camera::run()
{
    int i=0;
    Mat frame;
    threadid=(int)QThread::currentThread();
    QByteArray rst;
    while(!quit){
        //   prt(info,"runing %s",cam_cfg.url.toStdString().data());
        mtx.lock();
        if(src->get_frame(frame)&&frame.cols>0&&frame.rows>0){
            frame_rate++;
            // bool ret=process(frame,rst);
            foreach (VideoProcessor *processor, processors) {
                bool ret=processor->process(frame);
                send_out(processor->get_rst());
            }

        }else{
            //prt(info,"get no frame");
        }
        mtx.unlock();
        QThread::msleep(1);
    }
    // QThread::msleep(10);
}
