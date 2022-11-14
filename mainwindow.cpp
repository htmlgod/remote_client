#include "mainwindow.h"
#include "ui_mainwindow.h"

//#include "xlib_utils.h"
#include <X11/extensions/XTest.h>
#include <X11/extensions/Xfixes.h>
#include <QX11Info>


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
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::decode_mouse_btn(uint mb) {
    uint btn = 0;
    switch (mb) {
    case Qt::LeftButton:
        btn = Button1;
        break;
    case Qt::RightButton:
        btn = Button3;
        break;
    case Qt::MiddleButton:
        btn = Button2;
        break;
    }
    return btn;
}



void MainWindow::recieve_controls()
{
    if (control_socket->hasPendingDatagrams()) {
        Display* display = XOpenDisplay (NULL);
        if (display == NULL)
        {
            qDebug() << "Can't open display!";
            return;
        }

        control_data cd;
        auto dg = control_socket->receiveDatagram();
        QByteArray data = dg.data();
        QDataStream ds(&data, QIODevice::ReadOnly);
        ds >> cd;
        if (cd.type == "MOUSE") {
            auto md = cd.md;
            if (md.type == "MOVE") {
                QCursor::setPos(md.xpos, md.ypos);
            }
            else if (md.type == "SCROLL") {
                auto button = (md.delta <= 0) ? Button4 : Button5;
                XTestFakeButtonEvent(display, button, True, CurrentTime);
                XTestFakeButtonEvent(display, button, False, CurrentTime);
            }
            else {
                bool is_pressed;
                if (md.type == "PRESS") {
                    is_pressed = true;
                }
                else {
                    is_pressed = false;
                }
                auto button = decode_mouse_btn(md.button);
                XTestFakeButtonEvent(display, button, is_pressed, CurrentTime);
            }
        }
        else if (cd.type == "KEYBOARD") {
            auto kd = cd.kd;
            qDebug() << kd.type << kd.key << kd.text;
        }
        XFlush (display);
        XCloseDisplay(display);
    }
}

void MainWindow::send_preview_scr() {
    if (status == STATUS::CONNECTED or status == STATUS::CONTROL) {
        take_preview_scr();
        //auto raw_size = scr_data.size();
        auto compressed = qCompress(scr_data, settings.compression.toInt());
        QByteArray dg_data;
        QDataStream out(&dg_data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_0);
        out << compressed;
        //auto compressed_size = compressed.size();
        // int sended_bytes = sock->write(dg_data);
        sock->write(dg_data);
        //qDebug() << "preview sended with size " << compressed_size;
        //auto compr_rate = 100 * (compressed_size / (double) raw_size);
        //qDebug() << raw_size << " -> " << compressed_size << ", rate=" << compr_rate;
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

            if (!control_socket->bind(QHostAddress::AnyIPv4, CONTROL_PORT)) {
                qDebug() << "ERROR OPENING CONTROL SOCKET";
            }
            connect(control_socket, SIGNAL(readyRead()), this, SLOT(recieve_controls()));
            ui->status_info->setText("Управление");
            preview_timer->setInterval(settings.xmit_upd.toUInt()*1000);
            qDebug() << "server started control";
        }
        else if (msg == QString("STOP")) {
            control_socket->close();
            status = STATUS::CONNECTED;
            ui->status_info->setText("Подключен");
            preview_timer->setInterval(settings.preview_upd.toUInt()*1000);
            qDebug() << "server stoped control";
        }
    }
}

MainWindow::cursor MainWindow::capture_cursor() const
{
    cursor cursor;

    if( auto curImage = XFixesGetCursorImage( QX11Info::display() ) ) {
        cursor.buffer.resize( curImage->width * curImage->height );
        for( int i = 0; i < cursor.buffer.size(); ++i ) {
            cursor.buffer[ i ] = curImage->pixels[ i ] & 0xffffffff;
        }
        cursor.img = QImage(
            reinterpret_cast< const uchar* >( cursor.buffer.data() ),
            curImage->width,
            curImage->height,
            QImage::Format_ARGB32_Premultiplied
        );
        cursor.pos = QCursor::pos() - QPoint( curImage->xhot, curImage->yhot );
        XFree( curImage );
    }

    return cursor;
}

void MainWindow::take_preview_scr() {
    QScreen* screen = QGuiApplication::primaryScreen();
    if (const QWindow *window = windowHandle())
        screen = window->screen();
    if (!screen)
        return;
    QPixmap pxm = screen->grabWindow(0);

    if (status == STATUS::CONNECTED) {
        auto cursor = capture_cursor();
        if (!cursor.img.isNull()) {
            QPainter p;
            p.begin(&pxm);
            p.drawImage(cursor.pos, cursor.img);
            p.end();
        }
    }

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
