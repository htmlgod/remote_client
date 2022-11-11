#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "xlib_utils.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sock = new QTcpSocket(this);
    in.setDevice(sock);
    in.setVersion(QDataStream::Qt_5_0);
    preview_timer = new QTimer(this);
    connect(preview_timer, SIGNAL(timeout()), this, SLOT(send_preview_scr()));
    connect(sock, SIGNAL(readyRead()), this, SLOT(read_settings_and_connect()));
    ui->dsc_button->setEnabled(false);

    control_socket = new QUdpSocket(this);
    if (!control_socket->bind(QHostAddress::AnyIPv4, CONTROL_PORT)) {
        qDebug() << "ERROR OPENING CONTROL SOCKET";
    }
    connect(control_socket, SIGNAL(readyRead()), this, SLOT(recieve_controls()));
    frame_timer = new QTimer(this);
    connect(frame_timer, SIGNAL(timeout()), this, SLOT(send_frame()));
    //display = XOpenDisplay (NULL);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::recieve_controls()
{
    if (control_socket->hasPendingDatagrams()) {
        control_data cd;
        auto dg = control_socket->receiveDatagram();
        QByteArray data = dg.data();
        QDataStream ds(&data, QIODevice::ReadOnly);
        ds >> cd;
        if (cd.type == "CLICK") {
            Display* display = XOpenDisplay (NULL);
            click(display, Button1);
        }
    }
}

void MainWindow::send_frame()
{
    take_preview_scr();
    QByteArray dg_data = qCompress(scr_data, settings.compression.toInt());
    //QDataStream answer(&dg_data, QIODevice::WriteOnly);
    //answer << qCompress(scr_data, settings.compression.toInt());
    int sended_bytes = control_socket->writeDatagram(dg_data, QHostAddress(ip), SERVER_XMIT_PORT);
    qDebug() << "ATTEMPT TO SEND FRAME";
    qDebug() << dg_data.size();
    if (-1 == sended_bytes) {
        qDebug() << "can not send datagram(frame)";
    }
}

void MainWindow::send_preview_scr() {
    if (status == STATUS::CONNECTED) {
        take_preview_scr();
        auto raw_size = scr_data.size();
        auto compressed = qCompress(scr_data, settings.compression.toInt());
        QByteArray dg_data;
        QDataStream out(&dg_data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_0);
        out << compressed;
        auto compressed_size = compressed.size();
        // int sended_bytes = sock->write(dg_data);
        sock->write(dg_data);
        qDebug() << "preview sended with size " << compressed_size;
        auto compr_rate = 100 * (compressed_size / (double) raw_size);
        qDebug() << raw_size << " -> " << compressed_size << ", rate=" << compr_rate;
    }
}


void MainWindow::read_settings_and_connect() {
    if (status == STATUS::AWAITING_CONNECTION) {
        in.startTransaction();
        in >> settings;
        if (!in.commitTransaction()) {
            return;
        }
        ui->dsc_button->setEnabled(true);
        ui->connect_button->setEnabled(false);
        preview_timer->start(settings.preview_upd.toUInt()*1000);
        status = STATUS::CONNECTED;
        ui->status_info->setText("Подключен");
        qDebug() << "settings recieved, connected";
        qDebug() << settings.y_res << " " << settings.x_res << " " <<
                    settings.img_format << " " << settings.compression;
    }
    else if(status == STATUS::CONNECTED or status == STATUS::CONTROL) {
        in.startTransaction();
        QString msg;
        in >> msg;
        if (!in.commitTransaction()) {
            return;
        }
        if (msg == QString("START")) {
            status = STATUS::CONTROL;
            ui->status_info->setText("Управление");
            frame_timer->start(settings.xmit_upd.toUInt()*1000);
            qDebug() << "server started control";
        }
        else if (msg == QString("STOP")) {
            status = STATUS::CONNECTED;
            ui->status_info->setText("Подключен");
            frame_timer->stop();
            qDebug() << "server stoped control";
        }
    }
}


void MainWindow::take_preview_scr() {
    QScreen* screen = QGuiApplication::primaryScreen();
    if (const QWindow *window = windowHandle())
        screen = window->screen();
    if (!screen)
        return;
    QPixmap pxm = screen->grabWindow(0);
    QSize new_size = QSize(settings.x_res.toInt(),settings.y_res.toInt());
    pxm = pxm.scaled(new_size,
                     Qt::IgnoreAspectRatio,
                     Qt::SmoothTransformation);

    QBuffer buf(&scr_data);
    buf.open(QIODevice::WriteOnly);
    pxm.save(&buf, settings.img_format.toStdString().data());
}

void MainWindow::on_connect_button_clicked()
{
    port = ui->port_ed->text();
    ip = ui->ip_ed->text();
    sock->connectToHost(ip, port.toInt());
    qDebug() << "Connection request sended";
    status = STATUS::AWAITING_CONNECTION;
    ui->status_info->setText("Ожидание подключения");
}

void MainWindow::on_dsc_button_clicked()
{
    sock->waitForBytesWritten();
    sock->disconnectFromHost();
    status = STATUS::DISCONNECTED;
    qDebug() << "DISCONNECTED";
    ui->status_info->setText("Не подключен");
    ui->dsc_button->setEnabled(false);
    ui->connect_button->setEnabled(true);
    preview_timer->stop();
}
