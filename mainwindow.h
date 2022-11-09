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
    void read_settings_and_connect();
    void send_preview_scr();
    void on_connect_button_clicked();
    void on_dsc_button_clicked();
private:
    enum class STATUS {
        DISCONNECTED,
        CONNECTED,
        AWAITING_CONNECTION,
        ERROR
    };

    struct server_settings_data {
        QString y_res;
        QString x_res;
        QString img_format;
        QString compression;
        QString preview_upd;
        QString xmit_upd;
        //bool operator==(const server_settings_data& o) const = default;
        friend QDataStream &operator<<(QDataStream& out, const server_settings_data& server_settings){
            out << server_settings.y_res <<  server_settings.x_res <<  server_settings.img_format <<  server_settings.compression <<  server_settings.preview_upd <<  server_settings.xmit_upd;
            return out;
        }
        friend QDataStream &operator>>(QDataStream& in, server_settings_data& server_settings){
            in >> server_settings.y_res >>  server_settings.x_res >>  server_settings.img_format >>  server_settings.compression >>  server_settings.preview_upd >>  server_settings.xmit_upd;
            return in;
        }
    };
    void take_preview_scr();

    STATUS status = STATUS::DISCONNECTED;
    QTcpSocket* sock;
    QByteArray scr_data;
    QTimer* preview_timer;
    QString ip;
    QString port;
    QDataStream in;
    server_settings_data settings;

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
