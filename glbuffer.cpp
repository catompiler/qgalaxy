#include "glbuffer.h"
#include <QGLContext>
#include <QString>


PFNGLBINDBUFFERPROC GLBuffer::glBindBuffer = nullptr;
PFNGLDELETEBUFFERSPROC GLBuffer::glDeleteBuffers = nullptr;
PFNGLGENBUFFERSPROC GLBuffer::glGenBuffers = nullptr;
PFNGLISBUFFERPROC GLBuffer::glIsBuffer = nullptr;
PFNGLBUFFERDATAPROC GLBuffer::glBufferData = nullptr;
PFNGLBUFFERSUBDATAPROC GLBuffer::glBufferSubData = nullptr;
PFNGLGETBUFFERSUBDATAPROC GLBuffer::glGetBufferSubData = nullptr;
PFNGLMAPBUFFERPROC GLBuffer::glMapBuffer = nullptr;
PFNGLUNMAPBUFFERPROC GLBuffer::glUnmapBuffer = nullptr;
PFNGLGETBUFFERPARAMETERIVPROC GLBuffer::glGetBufferParameteriv = nullptr;
PFNGLGETBUFFERPOINTERVPROC GLBuffer::glGetBufferPointerv = nullptr;
bool GLBuffer::gl_funcs_initialized = false;


bool GLBuffer::init_gl_functions()
{
    if(gl_funcs_initialized) return true;

    const QGLContext* glcxt = QGLContext::currentContext();
    if(glcxt == nullptr) return false;

    glBindBuffer = (PFNGLBINDBUFFERPROC)glcxt->getProcAddress("glBindBuffer");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)glcxt->getProcAddress("glDeleteBuffers");
    glGenBuffers = (PFNGLGENBUFFERSPROC)glcxt->getProcAddress("glGenBuffers");
    glIsBuffer = (PFNGLISBUFFERPROC)glcxt->getProcAddress("glIsBuffer");
    glBufferData = (PFNGLBUFFERDATAPROC)glcxt->getProcAddress("glBufferData");
    glBufferSubData = (PFNGLBUFFERSUBDATAPROC)glcxt->getProcAddress("glBufferSubData");
    glGetBufferSubData = (PFNGLGETBUFFERSUBDATAPROC)glcxt->getProcAddress("glGetBufferSubData");
    glMapBuffer = (PFNGLMAPBUFFERPROC)glcxt->getProcAddress("glMapBuffer");
    glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)glcxt->getProcAddress("glUnmapBuffer");
    glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC)glcxt->getProcAddress("glGetBufferParameteriv");
    glGetBufferPointerv = (PFNGLGETBUFFERPOINTERVPROC)glcxt->getProcAddress("glGetBufferPointerv");
    // = ()glcxt->getProcAddress("");
    gl_funcs_initialized = (glBindBuffer != nullptr) &&
            (glDeleteBuffers != nullptr) &&
            (glGenBuffers != nullptr) &&
            (glIsBuffer != nullptr) &&
            (glBufferData != nullptr) &&
            (glBufferSubData != nullptr) &&
            (glGetBufferSubData != nullptr) &&
            (glMapBuffer != nullptr) &&
            (glUnmapBuffer != nullptr) &&
            (glGetBufferParameteriv != nullptr) &&
            (glGetBufferPointerv != nullptr);

    return gl_funcs_initialized;
}

GLBuffer::GLBuffer()
{
    m_id = 0;
    m_type = GLBuffer::VertexBuffer;
    m_usage = GLBuffer::StaticDraw;
    m_size = 0;
}

GLBuffer::GLBuffer(GLBuffer::Type type)
{
    m_id = 0;
    m_type = type;
    m_usage = GLBuffer::StaticDraw;
    m_size = 0;
}

GLBuffer::GLBuffer(const GLBuffer &other)
{
    m_id = other.m_id;
    m_type = other.m_type;
    m_usage = other.m_usage;
    m_size = other.m_size;
}

GLBuffer::~GLBuffer()
{
}

void GLBuffer::allocate(const void *data, int count)
{
    if(!init_gl_functions()) return;
    glBufferData(m_type, count, data, m_usage);
    m_size = count;
}

void GLBuffer::allocate(int count)
{
    allocate(nullptr, count);
}

bool GLBuffer::bind()
{
    if(!init_gl_functions()) return false;
    glBindBuffer(m_type, m_id);
    return m_id != 0;
}

GLuint GLBuffer::bufferId() const
{
    return m_id;
}

bool GLBuffer::create()
{
    if(!init_gl_functions()) return false;
    glGenBuffers(1, &m_id);
    return m_id != 0;
}

void GLBuffer::destroy()
{
    if(!init_gl_functions()) return;
    glDeleteBuffers(1, &m_id);
}

bool GLBuffer::isCreated() const
{
    return m_id != 0;
}

void *GLBuffer::map(GLBuffer::Access access)
{
    if(!init_gl_functions()) return nullptr;
    return glMapBuffer(m_type, access);
}

bool GLBuffer::read(int offset, void *data, int count)
{
    if(!init_gl_functions()) return false;
    glGetBufferSubData(m_type, offset, count, data);
    return true;
}

void GLBuffer::release()
{
    release(m_type);
}

void GLBuffer::setUsagePattern(GLBuffer::UsagePattern value)
{
    m_usage = value;
}

int GLBuffer::size() const
{
    /*if(!init_gl_functions()) return 0;
    GLint buf_size = 0;
    glGetBufferParameteriv(m_type, GL_BUFFER_SIZE, &buf_size);*/
    return m_size;
}

GLBuffer::Type GLBuffer::type() const
{
    return m_type;
}

bool GLBuffer::unmap()
{
    if(!init_gl_functions()) return false;
    glUnmapBuffer(m_type);
    return true;
}

GLBuffer::UsagePattern GLBuffer::usagePattern() const
{
    return m_usage;
}

void GLBuffer::write(int offset, const void *data, int count)
{
    if(!init_gl_functions()) return;
    glBufferSubData(m_type, offset, count, data);
}

GLBuffer &GLBuffer::operator=(const GLBuffer &other)
{
    m_id = other.m_id;
    m_type = other.m_type;
    m_usage = other.m_usage;
    m_size = other.m_size;
    return *this;
}

void GLBuffer::release(GLBuffer::Type type)
{
    if(!init_gl_functions()) return;
    glBindBuffer(type, 0);
}
