#include "stdafx.h"
#include "DataSender.h"
#include "json.h"
#include <string.h>

BOOL DataSender::ConnectServer(const char* URL)
{
	// ≥ı ºªØlibcurl
	CURLcode return_code;

	return_code = curl_global_init(CURL_GLOBAL_WIN32);
	if(return_code != CURLE_OK)
	{
		return FALSE;
	}

	m_easy_handle = curl_easy_init();
	curl_easy_setopt(m_easy_handle, CURLOPT_URL,"http://192.168.10.121/device/Jinher.ICP.Device.ISV.ISensorManageSV.svc/SensorReportData");

	return TRUE;
}

BOOL DataSender::SendToServier(char * data)
{
	char* p;
	Json::Value arrayObj;

	Json::Value new_item,new_item2;
	SYSTEMTIME st;
	long time_ms;

	time_ms = GetTickCount();



	sprintf(p,"/Date(%ld)/",time_ms);
	new_item2["Id"] = 0;
	new_item2["SampleTime"] = p;
	new_item2["DeviceType"] = 8;
	sprintf(p,"%d",data[3]*100+data[4]);
	new_item2["SampleData"] = p;
	new_item2["SensorID"] = "testSensor2018";
	new_item["OperationEnum"] = 52;
	new_item["Data"].append(new_item2);

	arrayObj.append(new_item);

	std::string  send_string;
	send_string = arrayObj.toStyledString();

	curl_easy_setopt(m_easy_handle, CURLOPT_POSTFIELDS,send_string);

	curl_easy_perform(m_easy_handle);

	return TRUE;
}

void DataSender::CloseConnect()
{
	curl_easy_cleanup(m_easy_handle);
	curl_global_cleanup();
}