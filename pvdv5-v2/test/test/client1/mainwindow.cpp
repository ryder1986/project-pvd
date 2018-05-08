#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    picked=false;
    ui->setupUi(this);
    src=new VideoSource("rtsp://192.168.1.97:554/av0_1");
    connect(&tmr,SIGNAL(timeout()),this,SLOT(timeup()));
    connect(&searcher,SIGNAL(find_ip(QString)),this,SLOT(ip_found(QString)));
    connect(&clt,SIGNAL(get_ret(QByteArray)),this,SLOT(server_msg(QByteArray)));
     tmr.start(1);

}

MainWindow::~MainWindow()
{
    delete src;
    delete ui;
}

void MainWindow::on_pushButton_search_clicked()
{
    ui->comboBox->clear();
    searcher.search_device();
}

void MainWindow::on_comboBox_activated(const QString &ip)
{
 //   qDebug()<<arg1;
    clt.connect_to_server(ip);

}

void MainWindow::on_pushButton_clear_clicked()
{
    ui->textEdit_recv->clear();
    ui->textEdit_send->clear();
}

void MainWindow::on_pushButton_send_clicked()
{
    clt.send_msg(ui->textEdit_send->toPlainText().toUtf8());
}
