#ifndef KEY_MAPPER_H
#define KEY_MAPPER_H

#include <QHash>

class key_mapper
{
public:
    key_mapper();

    uint get_native_virtual_keycode(Qt::Key keyCode);

    private:
        QHash< Qt::Key, uint > _keymap;
};

#endif // KEY_MAPPER_H
