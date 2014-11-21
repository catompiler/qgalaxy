#include "clexception.h"
#include <QObject>



CLException::CLException() throw()
    :std::exception()
{
}

CLException::CLException(const QString &what_, cl_int err_code_) throw()
    :std::exception()
{
    _what = what_;
    _err_code = err_code_;
}

CLException::~CLException() throw()
{
}

const char *CLException::what() const throw()
{
    return _what.toUtf8().data();
}

cl_int CLException::errorCode() const throw()
{
    return _err_code;
}

CLException CLException::make(cl_int err_code_)
{
    switch(err_code_){
    default:
        return CLException(QObject::tr("Bad error code"), err_code_);
    case CL_INVALID_VALUE:
        return CLException(QObject::tr("Invalid arguments"), err_code_);
    case CL_OUT_OF_HOST_MEMORY:
        return CLException(QObject::tr("Failure to allocate resources "
                              "required by the OpenCL implementation "
                              "on the host"), err_code_);
    case CL_INVALID_PLATFORM:
        return CLException(QObject::tr("Invalid platform"), err_code_);
    case CL_INVALID_DEVICE_TYPE:
        return CLException(QObject::tr("Invalid device type"), err_code_);
    case CL_DEVICE_NOT_FOUND:
        return CLException(QObject::tr("No OpenCL devices that matched "
                              "device_type were found"), err_code_);
    case CL_OUT_OF_RESOURCES:
        return CLException(QObject::tr("Failure to allocate resources required by the "
                              "OpenCL implementation on the device"), err_code_);
    case CL_INVALID_PROPERTY:
        return CLException(QObject::tr("Context property name in properties "
                              "is not a supported property name, or "
                              "the value specified for a supported "
                              "property name is not valid, or the "
                              "same property name is specified "
                              "more than once"), err_code_);
    case CL_INVALID_DEVICE:
        return CLException(QObject::tr("Devices list contains an invalid device"), err_code_);
    case CL_DEVICE_NOT_AVAILABLE:
        return CLException(QObject::tr("A device is currently not available "
                              "even though the device was returned "
                              "by clGetDeviceIDs"), err_code_);
    case CL_INVALID_CONTEXT:
        return CLException(QObject::tr("Invalid context"), err_code_);
    case CL_INVALID_QUEUE_PROPERTIES:
        return CLException(QObject::tr("Values specified in properties are valid "
                              "but are not supported by the device"), err_code_);
    case CL_INVALID_COMMAND_QUEUE:
        return CLException(QObject::tr("Invalid command queue"), err_code_);
    case CL_INVALID_BUFFER_SIZE:
        return CLException(QObject::tr("Buffer size is 0 or size is greater than "
                              "CL_DEVICE_MAX_MEM_ALLOC_SIZE"), err_code_);
    case CL_INVALID_HOST_PTR:
        return CLException(QObject::tr("Invalid host pQObject::tr"), err_code_);
    case CL_MEM_OBJECT_ALLOCATION_FAILURE:
        return CLException(QObject::tr("Failure to allocate memory for "
                              "buffer object"), err_code_);
    case CL_INVALID_MEM_OBJECT:
        return CLException(QObject::tr("Invalid mem object"), err_code_);
    case CL_INVALID_EVENT_WAIT_LIST:
        return CLException(QObject::tr("Invalid event wait list"), err_code_);
    case CL_MISALIGNED_SUB_BUFFER_OFFSET:
        return CLException(QObject::tr("Offset specified when the sub-buffer "
                              "object is created is not aligned to "
                              "CL_DEVICE_MEM_BASE_ADDR_ALIGN value "
                              "for device "), err_code_);
    case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
        return CLException(QObject::tr("The read and write operations are blocking "
                              "and the execution status of any of the "
                              "events in event_wait_list is a negative "
                              "integer value"), err_code_);
    case CL_MEM_COPY_OVERLAP:
        return CLException(QObject::tr("Copy overlap"), err_code_);
    case CL_MAP_FAILURE:
        return CLException(QObject::tr("Failure to map the requested region "
                              "into the host address space"), err_code_);
    case CL_INVALID_BINARY:
        return CLException(QObject::tr("Invalid program binary was encountered "
                              "for any device"), err_code_);
    case CL_INVALID_PROGRAM:
        return CLException(QObject::tr("Invalid program"), err_code_);
    case CL_INVALID_BUILD_OPTIONS:
        return CLException(QObject::tr("Invalid build options"), err_code_);
    case CL_INVALID_OPERATION:
        return CLException(QObject::tr("Invalid operation"), err_code_);
    case CL_COMPILER_NOT_AVAILABLE:
        return CLException(QObject::tr("Compiler not available"), err_code_);
    case CL_BUILD_PROGRAM_FAILURE:
        return CLException(QObject::tr("Failure to build the program executable"), err_code_);
    case CL_INVALID_PROGRAM_EXECUTABLE:
        return CLException(QObject::tr("No successfully built executable "
                              "for program"), err_code_);
    case CL_INVALID_KERNEL_NAME:
        return CLException(QObject::tr("Kernel name is not found in program"), err_code_);
    case CL_INVALID_KERNEL_DEFINITION:
        return CLException(QObject::tr("The function definition for __kernel function "
                              "given by kernel_name such as the number of "
                              "arguments, the argument types are not the "
                              "same for all devices for which the program "
                              "executable has been built"), err_code_);
    case CL_INVALID_KERNEL:
        return CLException(QObject::tr("Invalid kernel"), err_code_);
    case CL_INVALID_ARG_INDEX:
        return CLException(QObject::tr("Invalid argument index"), err_code_);
    case CL_INVALID_ARG_VALUE:
        return CLException(QObject::tr("Invalid argument value"), err_code_);
    case CL_INVALID_ARG_SIZE:
        return CLException(QObject::tr("Invalid argument size"), err_code_);
    case CL_INVALID_SAMPLER:
        return CLException(QObject::tr("Invalid sampler"), err_code_);
    case CL_INVALID_WORK_DIMENSION:
        return CLException(QObject::tr("Invalid work dimension"), err_code_);
    case CL_INVALID_GLOBAL_WORK_SIZE:
        return CLException(QObject::tr("Invalid global work size"), err_code_);
    case CL_INVALID_GLOBAL_OFFSET:
        return CLException(QObject::tr("Invalid global offset"), err_code_);
    case CL_INVALID_WORK_GROUP_SIZE:
        return CLException(QObject::tr("Invalid work group size"), err_code_);
    case CL_INVALID_WORK_ITEM_SIZE:
        return CLException(QObject::tr("Invalid work item size"), err_code_);
    case CL_INVALID_IMAGE_SIZE:
        return CLException(QObject::tr("Invalid image size"), err_code_);
    case CL_INVALID_EVENT:
        return CLException(QObject::tr("Invalid event"), err_code_);
    case CL_INVALID_GL_OBJECT:
        return CLException(QObject::tr("Invalid OpenGL object"), err_code_);
    /*case CL_:
        return CLException(QObject::tr(""), err_code_);
    case CL_:
        return CLException(QObject::tr(""), err_code_);
    case CL_:
        return CLException(QObject::tr(""), err_code_);*/
    }
}

CLException CLException::make(const QString &what_, cl_int err_code_)
{
    return CLException(what_, err_code_);
}
