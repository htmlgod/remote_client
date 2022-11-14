#include "key_mapper.h"

#include <X11/keysymdef.h>
#include <X11/Intrinsic.h>

key_mapper::key_mapper()
{
    _keymap[ Qt::Key_Escape ] = XK_Escape;
    _keymap[ Qt::Key_Tab ] = XK_Tab;
    _keymap[ Qt::Key_Backtab ] = XK_ISO_Left_Tab;
    _keymap[ Qt::Key_Backspace ] = XK_BackSpace;
    _keymap[ Qt::Key_Return ] = XK_Return;
    _keymap[ Qt::Key_Insert ] = XK_Insert;
    _keymap[ Qt::Key_Delete ] = XK_Delete;
    _keymap[ Qt::Key_Pause ] = XK_Pause;
    _keymap[ Qt::Key_Print ] = XK_Print;
    _keymap[ Qt::Key_SysReq ] = 0x1005FF60;
    _keymap[ Qt::Key_Home ] = XK_Home;
    _keymap[ Qt::Key_End ] = XK_End;
    _keymap[ Qt::Key_Left ] = XK_Left;
    _keymap[ Qt::Key_Up ] = XK_Up;
    _keymap[ Qt::Key_Right ] = XK_Right;
    _keymap[ Qt::Key_Down ] = XK_Down;
    _keymap[ Qt::Key_PageUp ] = XK_Prior;
    _keymap[ Qt::Key_PageDown ] = XK_Next;
    _keymap[ Qt::Key_Shift ] = XK_Shift_L;
    _keymap[ Qt::Key_Control ] = XK_Control_L;
    _keymap[ Qt::Key_Meta ] = XK_Meta_L;
    _keymap[ Qt::Key_Alt ] = XK_Alt_L;
    _keymap[ Qt::Key_CapsLock ] = XK_Caps_Lock;
    _keymap[ Qt::Key_NumLock ] = XK_Num_Lock;
    _keymap[ Qt::Key_ScrollLock ] = XK_Scroll_Lock;
    _keymap[ Qt::Key_Super_L ] = XK_Super_L;
    _keymap[ Qt::Key_Super_R ] = XK_Super_R;
    _keymap[ Qt::Key_Menu ] = XK_Menu;
    _keymap[ Qt::Key_Hyper_L ] = XK_Hyper_L;
    _keymap[ Qt::Key_Hyper_R ] = XK_Hyper_R;
    _keymap[ Qt::Key_Help ] = XK_Help;
    _keymap[ Qt::Key_F11 ] = 0x1005FF10;
    _keymap[ Qt::Key_F12 ] = 0x1005FF11;
    _keymap[ Qt::Key_Space ] = XK_space;
    _keymap[ Qt::Key_Enter ] = XK_KP_Enter;
    _keymap[ Qt::Key_F1 ] = XK_KP_F1;
    _keymap[ Qt::Key_F2 ] = XK_KP_F2;
    _keymap[ Qt::Key_F3 ] = XK_KP_F3;
    _keymap[ Qt::Key_F4 ] = XK_KP_F4;
    _keymap[ Qt::Key_Clear ] = XK_KP_Begin;
    _keymap[ Qt::Key_0 ] = XK_0;
    _keymap[ Qt::Key_1 ] = XK_1;
    _keymap[ Qt::Key_2 ] = XK_2;
    _keymap[ Qt::Key_3 ] = XK_3;
    _keymap[ Qt::Key_4 ] = XK_4;
    _keymap[ Qt::Key_5 ] = XK_5;
    _keymap[ Qt::Key_6 ] = XK_6;
    _keymap[ Qt::Key_7 ] = XK_7;
    _keymap[ Qt::Key_8 ] = XK_8;
    _keymap[ Qt::Key_9 ] = XK_9;
    _keymap[ Qt::Key_BracketLeft ] = XK_bracketleft;
    _keymap[ Qt::Key_BracketRight ] = XK_bracketright;
    _keymap[ Qt::Key_A ] = XK_a;
    _keymap[ Qt::Key_B ] = XK_b;
    _keymap[ Qt::Key_C ] = XK_c;
    _keymap[ Qt::Key_D ] = XK_d;
    _keymap[ Qt::Key_E ] = XK_e;
    _keymap[ Qt::Key_F ] = XK_f;
    _keymap[ Qt::Key_G ] = XK_g;
    _keymap[ Qt::Key_H ] = XK_h;
    _keymap[ Qt::Key_I ] = XK_i;
    _keymap[ Qt::Key_J ] = XK_j;
    _keymap[ Qt::Key_K ] = XK_k;
    _keymap[ Qt::Key_L ] = XK_l;
    _keymap[ Qt::Key_M ] = XK_m;
    _keymap[ Qt::Key_N ] = XK_n;
    _keymap[ Qt::Key_O ] = XK_o;
    _keymap[ Qt::Key_P ] = XK_p;
    _keymap[ Qt::Key_Q ] = XK_q;
    _keymap[ Qt::Key_R ] = XK_r;
    _keymap[ Qt::Key_S ] = XK_s;
    _keymap[ Qt::Key_T ] = XK_t;
    _keymap[ Qt::Key_U ] = XK_u;
    _keymap[ Qt::Key_V ] = XK_v;
    _keymap[ Qt::Key_W ] = XK_w;
    _keymap[ Qt::Key_X ] = XK_x;
    _keymap[ Qt::Key_Y ] = XK_y;
    _keymap[ Qt::Key_Z ] = XK_z;
}

uint key_mapper::get_native_virtual_keycode(Qt::Key keyCode)
{
    uint code = 0;

    Display* display = XOpenDisplay( nullptr );
    if( display ) {
        if( _keymap.contains( keyCode ) ) {
            code = XKeysymToKeycode( display, _keymap[ keyCode ] );
        }
        XCloseDisplay( display );
    }

    return code;
}
