#ifndef GLBUFFER_H
#define GLBUFFER_H

#include <GL/gl.h>
#include <GL/glext.h>

class GLBuffer
{
public:
	
	enum UsagePattern {
		StreamDraw = 0x88E0, StreamRead = 0x88E1, StreamCopy = 0x88E2,
		StaticDraw = 0x88E4, StaticRead = 0x88E5, StaticCopy = 0x88E6,
		DynamicDraw = 0x88E8, DynamicRead = 0x88E9, DynamicCopy = 0x88EA
	};
	
	enum Type {
		VertexBuffer = 0x8892, IndexBuffer = 0x8893,
		PixelPackBuffer = 0x88EB, PixelUnpackBuffer = 0x88EC
	};
	
	enum Access {
		ReadOnly = 0x88B8, WriteOnly = 0x88B9, ReadWrite = 0x88BA
	};
	
    GLBuffer();
    GLBuffer(GLBuffer::Type type);
    GLBuffer(const GLBuffer &other);
    ~GLBuffer();
    void allocate(const void * data, int count);
    void allocate(int count);
    bool bind();
    GLuint bufferId() const;
    bool create();
    void destroy();
    bool isCreated() const;
    void* map(GLBuffer::Access access);
    bool read(int offset, void * data, int count);
    void release();
    void setUsagePattern(GLBuffer::UsagePattern value);
    int	size() const;
    GLBuffer::Type	type() const;
    bool unmap();
    GLBuffer::UsagePattern	usagePattern() const;
    void write(int offset, const void * data, int count);
    GLBuffer &	operator= (const GLBuffer & other);
    static void release(GLBuffer::Type type);
private:
    GLuint m_id;
    GLBuffer::Type m_type;
    GLBuffer::UsagePattern m_usage;
    GLsizei m_size;

    static PFNGLBINDBUFFERPROC glBindBuffer;
    static PFNGLDELETEBUFFERSPROC glDeleteBuffers;
    static PFNGLGENBUFFERSPROC glGenBuffers;
    static PFNGLISBUFFERPROC glIsBuffer;
    static PFNGLBUFFERDATAPROC glBufferData;
    static PFNGLBUFFERSUBDATAPROC glBufferSubData;
    static PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData;
    static PFNGLMAPBUFFERPROC glMapBuffer;
    static PFNGLUNMAPBUFFERPROC glUnmapBuffer;
    static PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv;
    static PFNGLGETBUFFERPOINTERVPROC glGetBufferPointerv;
    static bool gl_funcs_initialized;

    static bool init_gl_functions();
};

#endif // GLBUFFER_H
