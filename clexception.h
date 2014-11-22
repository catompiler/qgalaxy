#ifndef CLEXCEPTION_H
#define CLEXCEPTION_H

#include <exception>
#include <CL/opencl.h>
#include <QString>


class CLException
        :public std::exception
{
public:
    CLException() noexcept;
    CLException(const QString& what_str, cl_int err_code) noexcept;
    ~CLException() noexcept;

    const char* what() const noexcept;
    cl_int errorCode() const noexcept;

    static CLException make(cl_int err_code);
    static CLException make(const QString& what_str, cl_int err_code);

protected:
    QString m_what;
    cl_int m_err_code;
};

#endif // CLEXCEPTION_H
