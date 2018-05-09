#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include<iostream>
#include "pvd.h"
class DataManager
{
    typedef struct pnt{
        int x;
        int y;
    }pnt_t;
    typedef struct pvd_c4{
        vector<pnt_t> detect_area;
        string channel_id;
        int step;
        string ratio;
    }pvd_c4_t;
    typedef struct pvd_hog{
        vector<pnt_t> detect_area;
    }pvd_hog_t;
    typedef struct cha{
        int channel_id;
        string selected_alg;
        pvd_c4_t c4;
        pvd_hog_t hog;
    }cha_t;

    typedef struct cam{
        string     url ;
        int   direction ;
        int   camera_no ;
        string    user_name ;
        string    password ;
        string    camera_ip;
        int    camera_port ;
        vector<cha_t> channels;
    }cam_t;

    typedef struct config{
        string     device_name;
        int   deviceID ;
        string   signal_machine_ip ;
        int   signal_machine_port ;
        string    time_machine_ip ;
        int  time_machine_port ;
        vector <cam_t>  cams;
    }config_t;
    config_t cfg;
public:
    DataManager()
    {

    }
    void set(string data)
    {
        DataPacket pkt(data);
        cfg.deviceID =pkt.get_int("deviceID");
        cfg.signal_machine_port =pkt.get_int("signal_machine_port");
        cfg.time_machine_port =pkt.get_int("time_machine_port");
        cfg.device_name= pkt.get_string("device_name");
        cfg.signal_machine_ip= pkt.get_string("signal_machine_ip");
        cfg.time_machine_ip= pkt.get_string("time_machine_ip");
        vector <JsonValue>cameras=DataPacket(pkt.get_value("cameras")).array_value();
        int i=0;
        foreach (JsonValue v, cameras) {
            cfg.cams.push_back( decode_camera(v));
        }
    }

    string get()
    {
        DataPacket pkt;
        pkt.set_int("deviceID",cfg.deviceID);
        pkt.set_int("signal_machine_port",cfg.signal_machine_port);
        pkt.set_int("time_machine_port",cfg.time_machine_port);
        pkt.set_string("device_name",cfg.device_name);
        pkt.set_string("signal_machine_ip",cfg.signal_machine_ip);
        pkt.set_string("time_machine_ip",cfg.time_machine_ip);
        vector <JsonValue> cms;
        foreach (cam_t c, cfg.cams) {
          cms.push_back(encode_camera(c));
        }
        pkt.set_value("cameras",DataPacket(cms).value());
        return pkt.data();
    }


private:
    cam_t  decode_camera(JsonValue data)
    {
        cam_t camera;
        DataPacket pkt(data);
        camera.url=pkt.get_string("url") ;
        camera.direction=pkt.get_int("direction")  ;
        camera.camera_no=pkt.get_int("camera_no")  ;
        camera.user_name=pkt.get_string("user_name") ;
        camera.password=pkt.get_string("password") ;
        camera.camera_ip=pkt.get_string("camera_ip");
        camera.camera_port=pkt.get_int("camera_port") ;
        vector <JsonValue>channels=DataPacket(pkt.get_value("channel")).array_value();
        foreach (JsonValue v, channels) {
            camera.channels.push_back( decode_channel(v));
        }
        return camera;
    }
    JsonValue  encode_camera(    cam_t camera)
    {
        DataPacket pkt;
        pkt.set_string("url",camera.url) ;
        pkt.set_int("direction", camera.direction)  ;
        pkt.set_int("camera_no",camera.camera_no)  ;
        pkt.set_string("user_name",camera.user_name) ;
        pkt.set_string("password",camera.password) ;
        pkt.set_string("camera_ip",camera.camera_ip);
        pkt.set_int("camera_port",  camera.camera_port) ;
        vector< JsonValue> cns;
        foreach (cha_t channel, camera.channels) {
            cns.push_back(encode_channel(channel));
        }
        DataPacket pkt_cns(cns);
        pkt.set_value("channel", pkt_cns.value()) ;
        return pkt.value();
    }
    cha_t decode_channel(JsonValue data)
    {
        cha_t c;
        DataPacket pkt(data);
        c.selected_alg=pkt.get_string("selected_alg") ;
        c.channel_id=pkt.get_int("channel_id") ;
        if(c.selected_alg=="pvd_c4"){
            DataPacket c4(pkt.get_value("pvd_c4"));
            c.c4.step=c4.get_int("step");
            c.c4.ratio=c4.get_string("ratio");
            vector<JsonValue> ps=DataPacket(c4.get_value("detect_area")).array_value();
            foreach (JsonValue v, ps) {
                DataPacket p(v);
                pnt_t pt;
                pt.x=p.get_int("x");
                pt.y=p.get_int("y");
                c.c4.detect_area.push_back(pt);
            }
        }
        if(c.selected_alg=="pvd_hog"){
            DataPacket hog(pkt.get_value("pvd_hog"));
            vector<JsonValue> ps=DataPacket(hog.get_value("detect_area")).array_value();
            foreach (JsonValue v, ps) {
                DataPacket p(v);
                pnt_t pt;
                pt.x=p.get_int("x");
                pt.y=p.get_int("y");
                c.hog.detect_area.push_back(pt);
            }
        }
        return c;
    }
    JsonValue encode_channel(cha_t c)
    {
        //   cha_t c;
        DataPacket pkt;
        pkt.set_string("selected_alg",c.selected_alg) ;
        pkt.set_int("channel_id",c.channel_id) ;
        if(c.selected_alg=="pvd_c4"){
            DataPacket c4;
            c4.set_int("step", c.c4.step);
            c4.set_string("ratio", c.c4.ratio);
            vector <JsonValue> ps;
            foreach (pnt_t p,  c.c4.detect_area) {
                DataPacket pkt_pnt;
                pkt_pnt.set_int("x",p.x);
                pkt_pnt.set_int("y",p.y);
                ps.push_back(pkt_pnt.value());
            }
            c4.set_value("detect_area",DataPacket(ps).value());
            pkt.set_value("pvd_c4",c4.value());
        }
        if(c.selected_alg=="pvd_hog"){
            DataPacket hog;
            vector <JsonValue> ps;
            foreach (pnt_t p,  c.hog.detect_area) {
                DataPacket pkt_pnt;
                pkt_pnt.set_int("x",p.x);
                pkt_pnt.set_int("y",p.y);
                ps.push_back(pkt_pnt.value());
            }
            hog.set_value("detect_area",DataPacket(ps).value());
            pkt.set_value("pvd_c4",hog.value());
        }
        return pkt.value();
    }

};

#endif // DATAMANAGER_H
