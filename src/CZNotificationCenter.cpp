
///  \file CZNotificationCenter.cpp
///  \brief This is the file implements the Class CZNotificationCenter and the Interface CZObserver.
///
///		CZNotificationCenter是一个单例，以观察者模式实现了消息的收集和转发。
///		Observer为纯虚类，提供观察者接口
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-10-14
///  \note

#include "CZNotificationCenter.h"
#include <iostream>

using namespace std;

CZNotificationCenter::CZNotificationCenter()
{
	center.clear();
}
CZNotificationCenter::~CZNotificationCenter()
{
	for(map<string,vector<ObserverItem*>* >::iterator itr = center.begin();	\
		itr != center.end(); itr++)
	{
		vector<ObserverItem*> *list = itr->second;
		for(vector<ObserverItem*>::iterator it = list->begin(); it != list->end(); it++)
			delete *it;
		list->clear();
		delete list;
	}

	center.clear();
}

/// 添加观察者
void CZNotificationCenter::addObserver(std::string &notificationName,CZObserver *observer,void* sender /* = NULL */)
{
	map<string,vector<ObserverItem*>* >::iterator itr = center.find(notificationName);

	if(itr == center.end())
	{
		vector<ObserverItem*> *list = new vector<ObserverItem*>;

		pair<map<string,vector<ObserverItem*>* >::iterator, bool> ret;
		ret = center.insert(pair<string,vector<ObserverItem*>* >(notificationName,list));
		if(ret.second)
		{
			itr = ret.first;
		}
		else
		{
			cerr << "CZNotificationCenter::addObserver - map insert fail\n";
			return ;
		}
	}

	vector<ObserverItem*> *list = itr->second;
	ObserverItem *item = new ObserverItem;
	item->observer = observer;
	item->sender = sender;

	list->push_back(item);

}

/// 移除观察者
void CZNotificationCenter::removeObserver(std::string &notificationName,CZObserver *observer)
{
	map<string,vector<ObserverItem*> *>::iterator itr = center.find(notificationName);

	if(itr != center.end())
	{
		vector<ObserverItem*> *list = itr->second;

		for(vector<ObserverItem*>::iterator it = list->begin(); it != list->end();)
		{
			if((*it)->observer == observer)
			{
				delete *it;
				it = list->erase(it);
			}
			else
				it++;
		}
	}
	else
	{
		cerr << "CZNotificationCenter::removeObserver - can not find the observer\n";
	}
}

/// 发送消息
void CZNotificationCenter::notify(std::string &notificationName, void* sender, void* data /* = NULL */)
{
	map<string,vector<ObserverItem*>* >::iterator itr = center.find(notificationName);

	if(itr != center.end())
	{
		vector<ObserverItem*> *list = itr->second;
		for(vector<ObserverItem*>::iterator it = list->begin(); it != list->end(); it++)
		{
			if((*it)->sender == NULL || (*it)->sender == sender)
				(*it)->observer->updateObserver(notificationName,data);
		}
	}
	else
	{
		cerr << "CZNotificationCenter::notify - can not find the observer\n";
	}
}
