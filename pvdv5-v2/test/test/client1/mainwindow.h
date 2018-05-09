#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QDebug>
#include "ui_mainwindow.h"
#include "videosource.h"
#include <QTimer>
#include "serverinfosearcher.h"
#include "client.h"
#include "datamanager.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    DataManager dm;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void open_config(bool ss,string cfg)
    {
        dm.set(cfg);
        dm.set(dm.get());
    }

    void timeup()
    {
        Mat rgb_frame;
        Mat bgr_frame;
        QImage img1;
        bool ret=src->get_frame(bgr_frame);
        if(ret){
            cvtColor(bgr_frame,rgb_frame,CV_BGR2RGB);
            img1=QImage((const uchar*)(rgb_frame.data),
                        rgb_frame.cols,rgb_frame.rows,
                        QImage::Format_RGB888);
            PlayerWidget *wgt=ui->widget_picture;
            if(wgt){
                img1.bits();
                wgt->set_image(img1);
                wgt->set_title(src->get_url().data());
                if(frame_rate%3==0)
                    wgt->update();
                frame_rate++;
            }


                    //   qDebug()<<"get frame";
        }
        // qDebug()<<"timeup";
    }

    void mouseMoveEvent(QMouseEvent *e)
    {
        //  qDebug()<<"move";
        if(picked){
            this->ui->groupBox_textbox->setFixedWidth(e->pos().x()-ui->groupBox_textbox->pos().x());
        }
    }
    void mousePressEvent(QMouseEvent *e)
    {
        picked=true;
        //   qDebug()<<"presss";
    }
    void mouseReleaseEvent(QMouseEvent *e)
    {
        picked=false;
    }

private slots:
    void on_pushButton_search_clicked();
    void ip_found(QString ip)
    {
        ui->comboBox->addItem(ip);
    }
    void server_msg(QByteArray ba)
    {
        ui->textEdit_recv->setPlainText(ba.data());
    }

    void on_comboBox_activated(const QString &arg1);

    void on_pushButton_clear_clicked();

    void on_pushButton_send_clicked();

private:
    Ui::MainWindow *ui;
    bool picked;
    VideoSource *src;
    QTimer tmr;
    int frame_rate;
    ServerInfoSearcher searcher;
    Client clt;
};

#endif // MAINWINDOW_H
