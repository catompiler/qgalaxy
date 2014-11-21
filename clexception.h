#ifndef CLEXCEPTION_H
#define CLEXCEPTION_H

#include <exception>
#include <CL/opencl.h>
#include <QString>


class CLException
        :public std::exception
{
public:
    CLException() throw();
    CLException(const QString& what_, cl_int err_code_) throw();
    ~CLException() throw();

    const char* what() const throw();
    cl_int errorCode() const throw();

    static CLException make(cl_int err_code_);
    static CLException make(const QString& what_, cl_int err_code_);

protected:
    QString _what;
    cl_int _err_code;
};

#endif // CLEXCEPTION_H
