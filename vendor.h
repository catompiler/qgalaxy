#ifndef VENDORS_H
#define VENDORS_H

#include <QString>


namespace vendor{


typedef unsigned int id_t;

namespace id{
    static const id_t invalid = 0;
    static const id_t nVidia = 0x10de;
    static const id_t AMD = 0x1002;
    static const id_t Intel = 0x8086;

    id_t byname(const QString& ven_name);
}

namespace name{
    static const QString invalid = "";
    static const QString nVidia = "NVIDIA Corporation";
    static const QString AMD = "Advanced Micro Devices, Inc.";
    static const QString Intel = "Intel(R) Corporation";

    QString byid(id_t vid);
}

}

#endif // VENDORS_H
