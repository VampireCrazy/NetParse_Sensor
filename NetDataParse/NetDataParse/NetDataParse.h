
// NetDataParse.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CNetDataParseApp:
// �йش����ʵ�֣������ NetDataParse.cpp
//

class CNetDataParseApp : public CWinApp
{
public:
	CNetDataParseApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CNetDataParseApp theApp;