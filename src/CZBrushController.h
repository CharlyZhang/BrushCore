
#include "brush/CZBrush.h"

class CZBrushController : public CZBrushDelegate
{
public:
	void brushPropertyChanged(std::vector<CZProperty> &properties){};	///< �ı�����ʱʵ�ָýӿ�
	void brushGeneratorChanged(CZStampGenerator *gen){/// to do
	};				///< �ı���滻������ʱ��ʵ�ָýӿ�
};