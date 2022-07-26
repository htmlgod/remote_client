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
    take_preview_scr();
    auto before_comp = scr_data.size();
    scr_data = qComsockpress(scr_data, compression.toInt());
    auto after_comp = scr_data.size();
    int sended_bytes = sock->writeDatagram(scr_data, QHostAddress(ip), port.toUInt());
    auto compr_rate = 100 * (after_comp / (double) before_comp);
    qDebug() << before_comp << " -> " << after_comp << ", rate=" << compr_rate;
    if (-1 == sended_bytes) {
        qDebug() << "can not send datagram";
    }
}

void MainWindow::read_server_respond() {
    if (sock->hasPendingDatagrams()) {

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

    pxm = pxm.scaled(QSize(x_res.toInt(), y_res.toInt()), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QBuffer buf(&scr_data);
    buf.open(QIODevice::WriteOnly);
    pxm.save(&buf, img_format.toStdString().data());
}

void MainWindow::on_connect_button_clicked()
{
    ip = ui->ip_ed->text();
    port = ui->port_ed->text();
    y_res = ui->yres_preview->text();
    x_res = ui->xres_preview->text();
    img_format = ui->imgformat->currentText();
    compression = ui->imgcompr->currentText();
    preview_upd = ui->updtimer_preview->text();
    xmit_upd = ui->updtimer_transmit->text();

    ui->dsc_button->setEnabled(true);
    ui->connect_button->setEnabled(false);

    preview_timer->start(preview_upd.toUInt()*1000);
    qDebug() << "compr lvl=" << compression << ", imgformat=" << img_format;


    control_sock->bind(QHostAddress(ip), 1230);
}

void MainWindow::on_dsc_button_clicked()
{
    ui->dsc_button->setEnabled(false);
    ui->connect_button->setEnabled(true);
    preview_timer->stop();
    control_sock->close();
}
