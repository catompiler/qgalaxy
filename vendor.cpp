#include "vendor.h"

namespace vendor{

namespace id{
id_t byname(const QString& ven_name)
{
    if(ven_name.compare(::vendor::name::nVidia, Qt::CaseInsensitive) == 0){
        return nVidia;
    }else if(ven_name.compare(::vendor::name::AMD, Qt::CaseInsensitive) == 0){
        return AMD;
    }else if(ven_name.compare(::vendor::name::Intel, Qt::CaseInsensitive) == 0){
        return Intel;
    }
    return invalid;
}
}

namespace name{
QString byid(id_t vid)
{
    switch(vid){
    case ::vendor::id::nVidia:
        return nVidia;
    case ::vendor::id::AMD:
        return AMD;
    case ::vendor::id::Intel:
        return Intel;
    }
    return invalid;
}
}

}
