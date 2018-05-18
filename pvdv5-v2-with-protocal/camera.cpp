#include "camera.h"
Camera::Camera(JsonValue jv)
{
  //  frame_rate=0;
    send_check_tick=0;
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
   // int no= pkt.get_int("modify_channel_no");
    vector< JsonValue> chs=pkt.array_value();
//    for(int i=0;i<chs.size();i++){
//         cam_cfg.chs[i]=chs[i];
//    }
    cam_cfg.chs.clear();
    cam_cfg.chs=chs;

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
    int sz=0;
    Mat frame;
    threadid=(int)QThread::currentThread();
    QByteArray rst;
    vector<JsonValue> channel_data;
    VideoProcessor *pro;
    while(!quit){
        //   prt(info,"runing %s",cam_cfg.url.toStdString().data());
        mtx.lock();
        if(src->get_frame(frame)&&frame.cols>0&&frame.rows>0){
          //  frame_rate++;
            // bool ret=process(frame,rst);
#if 0
            foreach (VideoProcessor *processor, processors) {
                bool ret=processor->process(frame);
                send_out(processor->get_rst());
            }
#else
            channel_data.clear();

            sz=processors.size();

            data_t da;
            for(i=0;i<sz;i++){
                cdata_t cd;
                pro= processors[i];
                pro->process(frame);
                JsonValue v=DataPacket((pro->get_rst())).value();
                DataPacket pkt;
                pkt.set_value("result",v);
                pkt.set_int("channel_id",pro->get_id());

                channel_data.push_back(pkt.value());

                cd.no=pro->get_id();
                cd.exist=DataPacket((pro->get_rst())).get_int("exist");
                da.channels.push_back(cd);
            }
          //  send_out(DataPacket(channel_data).data());
            DataPacket pkt1;
            pkt1.set_value("rt_data",DataPacket(channel_data).value());
            send_out(pkt1.data());
            if(1){

                da.direction=this->cam_cfg.direction;
                da.channel_count=this->cam_cfg.chs.size();
                vector <uint8_t> rst_stream=process_protocal(da);
                ProcessedDataSender *s=ProcessedDataSender::get_instance();
                QByteArray ba;
                for(int i=0;i<rst_stream.size();i++)
                    ba.append(rst_stream[i]);
                s->send_sig(ba);
             // send_data();
            }

#endif


        }else{
            //prt(info,"get no frame");
        }
        mtx.unlock();
        QThread::msleep(1);
    }
    // QThread::msleep(10);
}
