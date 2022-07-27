#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sock = new QUdpSocket(this);
    preview_timer = new QTimer(this);
    connect(preview_timer, SIGNAL(timeout()), this, SLOT(send_preview_scr()));
    connect(sock, SIGNAL(readyRead()), this, SLOT(read_server_respond()));
    ui->dsc_button->setEnabled(false);
    control_sock = new QUdpSocket(this);
    connect(control_sock, SIGNAL(readyRead()), this, SLOT(recieve_control()));
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::send_preview_scr() {
    if (status == STATUS::CONNECTED) {
        take_preview_scr();
        auto before_comp = scr_data.size();
        QByteArray dg_data = qCompress(scr_data, settings.compression.toInt());
        auto after_comp = dg_data.size();
        protocol_msg_data ans;
        ans.data = dg_data;
        ans.msg = "SCR";
        QByteArray data;
        QDataStream answer(&data, QIODevice::WriteOnly);
        answer << ans;
        qDebug() << data.size() << ": data size";
        int sended_bytes = sock->writeDatagram(data, QHostAddress(ip), port.toUInt());
        auto compr_rate = 100 * (after_comp / (double) before_comp);
        qDebug() << before_comp << " -> " << after_comp << ", rate=" << 100 - compr_rate;
        if (-1 == sended_bytes) {
            qDebug() << "can not send datagram";
        }
    }
}

void MainWindow::recieve_settings(const QNetworkDatagram &dg)
{
    QByteArray data = dg.data();
    QDataStream ds(&data, QIODevice::ReadOnly);
    ds >> settings;
    ui->dsc_button->setEnabled(true);
    ui->connect_button->setEnabled(false);
    preview_timer->start(settings.preview_upd.toUInt()*1000);
    status = STATUS::CONNECTED;
    ui->status_info->setText("Подключен");
    qDebug() << "settings recieved, connected";
    qDebug() << settings.y_res << " " << settings.x_res << " " <<
                settings.img_format << " " << settings.compression;
}

void MainWindow::recieve_server_msg(const QNetworkDatagram &dg)
{
    QByteArray data = dg.data();
    QDataStream ds(&data, QIODevice::ReadOnly);
    protocol_msg_data msg;
    ds >> msg;
    if (msg.msg != "OK") {
        status = STATUS::ERROR;
    }
    else {
        if (status == STATUS::AWAITING_DISCONNECTION) {
            status = STATUS::DISCONNECTED;
            qDebug() << "DISCONNECTED";
            ui->status_info->setText("Не подключен");
            ui->dsc_button->setEnabled(false);
            ui->connect_button->setEnabled(true);
            preview_timer->stop();
            control_sock->close();
        }
        else if (status == STATUS::AWAITING_CONNECTION) {
            status = STATUS::AWAITING_SETTINGS;
            qDebug() << "Connected, waiting for settings";
            ui->status_info->setText("Ожидание настроек");
        }
    }
}


void MainWindow::read_server_respond() {
    if (sock->hasPendingDatagrams()) {
        auto dg = sock->receiveDatagram();

        if (status == STATUS::AWAITING_SETTINGS) {
            recieve_settings(dg);
        }
        else {
            recieve_server_msg(dg);
        }
    }
}

void MainWindow::recieve_control()
{

}

void MainWindow::take_scr() {

}

void MainWindow::take_preview_scr() {
    QScreen* screen = QGuiApplication::primaryScreen();
    if (const QWindow *window = windowHandle())
        screen = window->screen();
    if (!screen)
        return;
    QPixmap pxm = screen->grabWindow(0);

    pxm = pxm.scaled(QSize(settings.x_res.toInt(), settings.y_res.toInt()), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QBuffer buf(&scr_data);
    buf.open(QIODevice::WriteOnly);
    pxm.save(&buf, settings.img_format.toStdString().data());
}

void MainWindow::on_connect_button_clicked()
{
    protocol_msg_data msg {
        "CON", {}
    };
    port = ui->port_ed->text();
    ip = ui->ip_ed->text();
    QByteArray data;
    QDataStream ds(&data, QIODevice::WriteOnly);
    ds << msg;
    sock->writeDatagram(data, QHostAddress(ip), port.toUInt());
    qDebug() << "Connection request sended";
    status = STATUS::AWAITING_CONNECTION;
    ui->status_info->setText("Ожидание подключения");



    //
    //qDebug() << "compr lvl=" << compression << ", imgformat=" << img_format;


    //control_sock->bind(QHostAddress(ip), 1230);
}

void MainWindow::on_dsc_button_clicked()
{
    protocol_msg_data msg {
        "DSC", 0
    };
    QByteArray data;
    QDataStream ds(&data, QIODevice::WriteOnly);
    ds << msg;
    sock->writeDatagram(data, QHostAddress(ip), port.toUInt());
    qDebug() << "Disconnection request sended";
    status = STATUS::AWAITING_DISCONNECTION;
    ui->status_info->setText("Ожидание отключения");

}
