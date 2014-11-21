#ifndef UTILS_H
#define UTILS_H


#define CONCAT_IMPL(a,b) a ## b
#define CONCAT(a,b) CONCAT_IMPL(a,b)
#define CL_ERR_THROW(func)\
    do{\
        cl_int CONCAT(res, __LINE__) = func;\
        if(CONCAT(res, __LINE__) != CL_SUCCESS){\
            throw(CLException::make(CONCAT(res, __LINE__)));\
        }\
    }while(0)


#endif // UTILS_H
