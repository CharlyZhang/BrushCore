
#ifndef _CZDATAPROVIDER_H_
#define _CZDATAPROVIDER_H_

#include <string>

typedef enum 
{
	kSaveStatusUnsaved,
	kSaveStatusTentative,
	kSaveStatusSaved,
} CZSaveStatus;

class CZDataProvider
{
public:
    virtual void * getData() = 0;
    virtual CZSaveStatus getSaveStatus() = 0;
    virtual std::string  getMediaType() = 0;
};

#endif