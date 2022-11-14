#ifndef COMMON_H
#define COMMON_H

#include <QMainWindow>
// THIS FILE DEPENDS ON common.h OF SERVER SIDE
const int CONTROL_PORT = 1235;

struct server_settings_data {
    QString y_res;
    QString x_res;
    QString img_format;
    QString compression;
    QString preview_upd;
    QString xmit_fps;
    friend QDataStream &operator<<(QDataStream& out, const server_settings_data& server_settings){
        out << server_settings.y_res <<  server_settings.x_res <<  server_settings.img_format <<  server_settings.compression <<  server_settings.preview_upd <<  server_settings.xmit_fps;
        return out;
    }
    friend QDataStream &operator>>(QDataStream& in, server_settings_data& server_settings){
        in >> server_settings.y_res >>  server_settings.x_res >>  server_settings.img_format >>  server_settings.compression >>  server_settings.preview_upd >>  server_settings.xmit_fps;
        return in;
    }
};
struct mouse_control_data {
    QString type;
    uint button;
    int xpos;
    int ypos;
    int delta;
    friend QDataStream &operator<<(QDataStream& out, const mouse_control_data& cd){
        out << cd.type << cd.button << cd.xpos << cd.ypos << cd.delta;
        return out;
    }
    friend QDataStream &operator>>(QDataStream& in, mouse_control_data& cd){
        in >> cd.type >> cd.button >> cd.xpos >>  cd.ypos >> cd.delta;
        return in;
    }
};
struct keyboard_control_data {
    QString type; // press, release
    int key;
    QString text;
    friend QDataStream &operator<<(QDataStream& out, const keyboard_control_data& cd){
        out << cd.type << cd.key << cd.text;
        return out;
    }
    friend QDataStream &operator>>(QDataStream& in, keyboard_control_data& cd){
        in >> cd.type >> cd.key >> cd.text;
        return in;
    }
};
struct control_data {
    QString type;
    mouse_control_data md;
    keyboard_control_data kd;
    friend QDataStream &operator<<(QDataStream& out, const control_data& cd){
        out << cd.type << cd.md << cd.kd;
        return out;
    }
    friend QDataStream &operator>>(QDataStream& in, control_data& cd){
        in >> cd.type >> cd.md >> cd.kd;
        return in;
    }
};
#endif // COMMON_H
