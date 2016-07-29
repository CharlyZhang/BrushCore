
///  \file CZNotificationCenter.h
///  \brief This is the file declare the Class CZNotificationCenter and the Interface CZObserver.
///
///		CZNotificationCenter是一个单例，以观察者模式实现了消息的收集和转发。
///		Observer为纯虚类，提供观察者接口
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
	CZNotificationCenter();  //构造函数是私有的
	CZNotificationCenter(const CZNotificationCenter &);
	CZNotificationCenter & operator = (const CZNotificationCenter &);
	~CZNotificationCenter();

public:
	/// 完成单例获取函数
	static CZNotificationCenter * getInstance()
	{
		static CZNotificationCenter instance;   //局部静态变量  
		return &instance; 
	}
	/// 添加观察者
	///		/param notificationCenter	- 消息名称
	///		/param observer				- 观察者
	///		/param original				- 消息的发送者
	void addObserver(std::string &notificationName,CZObserver *observer,void* sender = NULL);
	/// 移除观察者
	void removeObserver(std::string &notificationName,CZObserver *observer);
	/// 发送消息
	void notify(std::string &notificationName, void* sender, void* data = NULL);
private:
	/// 定义观察者项目类
	class ObserverItem
	{
	public :
		CZObserver *observer;
		void	  *sender;
	};
	
	std::map<std::string,std::vector<ObserverItem*> *> center;			/// 登记观察者中心 
};

class CZObserver
{
public:
	virtual void updateObserver(std::string &notificationName, void* data = NULL) = 0;
};
#endif

