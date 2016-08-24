#ifndef RECDCMTKFILE_H
#define RECDCMTKFILE_H
#include "QString"

class recdcmtkfile
{
public:
    recdcmtkfile();

    unsigned short height;
    unsigned short width;
    QString name;
    QString id;
    QString date;
    QString time;


    bool flag;
};

#endif // RECDCMTKFILE_H
