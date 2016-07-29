#ifndef _CZBRUSHCELL_H_
#define _CZBRUSHCELL_H_

#include "basic/CZProperty.h"
#include <set>

class CZBrushCell : public CZBrushDelegate
{
public:
	void brushPropertyChanged(std::set<CZProperty> &properties){///TO DO
	};
	void brushGeneratorChanged(CZStampGenerator *gen){///to do
	};
};

#endif