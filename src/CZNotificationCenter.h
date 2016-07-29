
///  \file CZNotificationCenter.h
///  \brief This is the file declare the Class CZNotificationCenter and the Interface CZObserver.
///
///		CZNotificationCenter��һ���������Թ۲���ģʽʵ������Ϣ���ռ���ת����
///		ObserverΪ�����࣬�ṩ�۲��߽ӿ�
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-10-14
///  \note

#ifndef _CZNOTIFICATIONCENTER_H_
#define _CZNOTIFICATIONCENTER_H_

#include <string>
#include <vector>
#include <map>

class CZObserver;

class CZNotificationCenter
{
private:
	CZNotificationCenter();  //���캯����˽�е�
	CZNotificationCenter(const CZNotificationCenter &);
	CZNotificationCenter & operator = (const CZNotificationCenter &);
	~CZNotificationCenter();

public:
	/// ��ɵ�����ȡ����
	static CZNotificationCenter * getInstance()
	{
		static CZNotificationCenter instance;   //�ֲ���̬����  
		return &instance; 
	}
	/// ��ӹ۲���
	///		/param notificationCenter	- ��Ϣ����
	///		/param observer				- �۲���
	///		/param original				- ��Ϣ�ķ�����
	void addObserver(std::string &notificationName,CZObserver *observer,void* sender = NULL);
	/// �Ƴ��۲���
	void removeObserver(std::string &notificationName,CZObserver *observer);
	/// ������Ϣ
	void notify(std::string &notificationName, void* sender, void* data = NULL);
private:
	/// ����۲�����Ŀ��
	class ObserverItem
	{
	public :
		CZObserver *observer;
		void	  *sender;
	};
	
	std::map<std::string,std::vector<ObserverItem*> *> center;			/// �Ǽǹ۲������� 
};

class CZObserver
{
public:
	virtual void updateObserver(std::string &notificationName, void* data = NULL) = 0;
};
#endif

