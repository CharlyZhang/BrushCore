
#ifndef _CZGLCONTEXT_H_
#define _CZGLCONTEXT_H_

class CZGLContext
{
public:
    struct Impl;
	CZGLContext();
    ~CZGLContext();

	bool setAsCurrent();

    void* getRealContext();

private:
    Impl *impl;
};

#endif