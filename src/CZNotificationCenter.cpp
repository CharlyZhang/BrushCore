
///  \file CZNotificationCenter.cpp
///  \brief This is the file implements the Class CZNotificationCenter and the Interface CZObserver.
///
///		CZNotificationCenter��һ���������Թ۲���ģʽʵ������Ϣ���ռ���ת����
///		ObserverΪ�����࣬�ṩ�۲��߽ӿ�
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

/// ��ӹ۲���
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

/// �Ƴ��۲���
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

/// ������Ϣ
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
