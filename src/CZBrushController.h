
#include "brush/CZBrush.h"

class CZBrushController : public CZBrushDelegate
{
public:
	void brushPropertyChanged(std::vector<CZProperty> &properties){};	///< 改变属性时实现该接口
	void brushGeneratorChanged(CZStampGenerator *gen){/// to do
	};				///< 改变和替换生成器时都实现该接口
};