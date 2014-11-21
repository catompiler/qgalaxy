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
    CLException(const QString& what_str, cl_int err_code) throw();
    ~CLException() throw();

    const char* what() const throw();
    cl_int errorCode() const throw();

    static CLException make(cl_int err_code);
    static CLException make(const QString& what_str, cl_int err_code);

protected:
    QString m_what;
    cl_int m_err_code;
};

#endif // CLEXCEPTION_H
