#ifndef __DATA_SENDER_H
#define __DATA_SENDER_H
#include <string>

#include "curl.h"

class DataSender
{
public:
	 DataSender();
	~DataSender();
public:
	BOOL ConnectServer(const char* URL);
	BOOL SendToServier(char* data);
	void CloseConnect();
private:
	CURL *m_easy_handle;
};

#endif
