#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTest>
#include <QNetworkDatagram>
#include <QDataStream>
#include <QUdpSocket>
#include <QTimer>
#include <QPixmap>
#include <QImage>
#include <QWindow>
#include <QScreen>
#include <QBuffer>
#include <QCursor>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void read_server_respond();
    void send_preview_scr();
    void recieve_control();

    void on_connect_button_clicked();

    void on_dsc_button_clicked();
private:
    enum class STATUS {
        NO_CONNECTION,
        REQUEST_SENDED,
        CONNECTED
    };
    struct protocol_msg_data {
        QString msg;
        // crypto
    };
    struct server_settings_data {
        QString msg;
        QString y_res;
        QString x_res;
        QString img_format;
        QString compression;
        QString preview_upd;
        QString xmit_upd;
    };
    // main socket - sock
    void init_connection();
    void disconnect();
    // main socket - control sock
    void start_control();
    void stop_control();

    void take_preview_scr();
    void take_scr();

    Ui::MainWindow *ui;
    QUdpSocket* sock;
    QTimer* preview_timer;
    QByteArray scr_data;

    QTimer* xmit_timer;
    QUdpSocket* contsend_preview_scrrol_sock;
    // vector for fullscreen img and etc
    // also add listen socket for remote control

    QString ip;
    QString port;
    QString y_res;
    QString x_res;
    QString img_format;
    QString compression;
    QString preview_upd;
    QString xmit_upd;
};
#endif // MAINWINDOW_H
