#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtNetwork>
#include <QTimer>
#include <QPixmap>
#include <QImage>
#include <QWindow>
#include <QScreen>
#include <QBuffer>
#include <QCursor>
#include <QPainter>
#include <QMessageBox>

#include <common.h>

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
    void recieve_controls();

    void read_data_from_server();
    void send_preview_scr();
    void on_connect_button_clicked();
    void on_dsc_button_clicked();
private:
    enum class STATUS {
        DISCONNECTED,
        CONNECTED,
        CONTROL,
        AWAITING_CONNECTION,
        ERROR
    };
    void take_preview_scr();
    int decode_mouse_btn(uint mb);

    struct cursor {
        QImage img;
        QPoint pos;
        // Linux-reserved
        QVarLengthArray< quint32 > buffer;
    };
    void press_key(bool is_pressed, int key, const QString& text);
    cursor capture_cursor() const;

    STATUS status = STATUS::DISCONNECTED;
    QTcpSocket* sock;
    QByteArray scr_data;
    QTimer* preview_timer;
    QString ip;
    QString port;
    QDataStream in;
    server_settings_data settings;

    QUdpSocket* control_socket; // listening
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
