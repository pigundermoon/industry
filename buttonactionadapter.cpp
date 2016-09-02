#include "buttonactionadapter.h"

ButtonActionAdapter::ButtonActionAdapter(QObject *parent, QAction *a, QToolButton *b) : QObject(parent), action(a), button(b)
{

}

void ButtonActionAdapter::local_connect(){
    if(action != nullptr && button != nullptr){
        connect(action, SIGNAL(changed()), this, SLOT(receive_change_from_action()));
        connect(this, SIGNAL(send_enable_to_button(bool)), button, SLOT(setEnabled(bool)));
    }
}


void ButtonActionAdapter::receive_change_from_action(){
    if(action != nullptr && button != nullptr){
        emit send_enable_to_button(action->isEnabled());
    }
}
