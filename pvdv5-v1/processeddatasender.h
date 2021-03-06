#ifndef PROCESSEDDATASENDER_H
#define PROCESSEDDATASENDER_H

#include <QTimer>
#include <QtNetwork>
#include <QNetworkInterface>
#include "tool.h"
#include "pvd.h"
class ProcessedDataSender : public QObject{
    Q_OBJECT

public:
    static ProcessedDataSender *get_instance()
    {
        static ProcessedDataSender sender;
        return &sender;
    }
    void send(QByteArray datagram,const QHostAddress addr)
    {
        udp_skt->writeDatagram(datagram.data(), datagram.size(),
                               addr, Pvd::get_instance().client_data_port);
    }
private:
    ProcessedDataSender(){
        udp_skt = new QUdpSocket();
    }
    ~ProcessedDataSender()
    {
        delete udp_skt;
    }

public  slots:
    void check_client()
    {
        QByteArray client_msg;
        char *msg;
        if(udp_skt->hasPendingDatagrams())
        {
            client_msg.resize((udp_skt->pendingDatagramSize()));
            udp_skt->readDatagram(client_msg.data(),client_msg.size());
            prt(info,"msg :%s",msg=client_msg.data());
            if(!strcmp(msg,"pedestrian"))
                send_buffer_to_client();
        }else{
            //prt(debug,"searching client on port %d",Protocol::SERVER_REPORTER_PORT)
        }
    }

    void send_buffer_to_client()
    {
        QByteArray datagram;
        datagram.clear();
        QList <QNetworkInterface>list_interface=QNetworkInterface::allInterfaces();
        foreach (QNetworkInterface i, list_interface) {
            if(i.name()!="lo"){
                QList<QNetworkAddressEntry> list_entry=i.addressEntries();
                foreach (QNetworkAddressEntry e, list_entry) {
                    if(e.ip().protocol()==QAbstractSocket::IPv4Protocol)
                    {
                        datagram.append(QString(e.ip().toString())).append(QString(",")).\
                                append(QString(e.netmask().toString())).append(QString(",")).append(QString(e.broadcast().toString()));
                    }

                }
            }
        }
        udp_skt->writeDatagram(datagram.data(), datagram.size(),
                               QHostAddress::Broadcast, Pvd::CLIENT_REPORTER_PORT);
    }
private:
    QTimer *timer;
    QUdpSocket *udp_skt;
};


#endif // PROCESSEDDATASENDER_H
