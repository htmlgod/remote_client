#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sock = new QTcpSocket(this);
    in.setDevice(sock);
    in.setVersion(QDataStream::Qt_5_10);
    preview_timer = new QTimer(this);
    connect(preview_timer, SIGNAL(timeout()), this, SLOT(send_preview_scr()));
    connect(sock, SIGNAL(readyRead()), this, SLOT(read_settings_and_connect()));
    ui->dsc_button->setEnabled(false);
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
        int sended_bytes = sock->write(dg_data);
        auto compr_rate = 100 * (after_comp / (double) before_comp);
        qDebug() << before_comp << " -> " << after_comp << ", rate=" << 100 - compr_rate;
        if (-1 == sended_bytes) {
            qDebug() << "can not send datagram";
        }
    }
}


void MainWindow::read_settings_and_connect() {
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


void MainWindow::take_preview_scr() {
    QScreen* screen = QGuiApplication::primaryScreen();
    if (const QWindow *window = windowHandle())
        screen = window->screen();
    if (!screen)
        return;
    QPixmap pxm = screen->grabWindow(0);

    pxm = pxm.scaled(QSize(settings.x_res.toInt(),
                           settings.y_res.toInt()),
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

    sock->disconnectFromHost();
    status = STATUS::DISCONNECTED;
    qDebug() << "DISCONNECTED";
    ui->status_info->setText("Не подключен");
    ui->dsc_button->setEnabled(false);
    ui->connect_button->setEnabled(true);
    preview_timer->stop();
    control_sock->close();
}
