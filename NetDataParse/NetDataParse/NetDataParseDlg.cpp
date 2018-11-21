
// NetDataParseDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "NetDataParse.h"
#include "NetDataParseDlg.h"
#include "afxdialogex.h"

#include "json.h"
#include <string.h>
#include <time.h>
#include <sys/timeb.h>
#include "Iphlpapi.h"
#include <malloc.h>
#include "Winsock2.h"

#include <stdlib.h>
#include <iostream>

#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CString g_strAddrAndPort;

// 套接字回调函数
// 服务器端
void SNewSocketCallBack(const SOCKADDR_IN *szSockAddr, SOCKET newSocket, void *pUserParam); // 客户端连接
void SNewMsgCallBack(const SOCKADDR_IN *pSockAddr, const SOCKET_HAND *pSocketHand, const SOCKET_DATA *pData, void *pUserParam); // 接收消息
void SCloseSocketCallBack(const char *szSocket, SOCKET newSocket, void *pUserParam); // 客户端断开连接



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CNetDataParseDlg 对话框

CNetDataParseDlg::CNetDataParseDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CNetDataParseDlg::IDD, pParent)
{
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}




void CNetDataParseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_com_NetProtocol);
	DDX_Control(pDX, IDC_STATIC1, m_stc_local_IP);
	DDX_Control(pDX, IDC_STATIC2, m_stc_local_Port);
	DDX_Control(pDX, IDC_EDIT7, m_edt_recvData);
	DDX_Control(pDX, IDC_CHECK1, m_ckb_recv2);
	DDX_Control(pDX, IDC_EDIT3, m_edt_locHostPort);
	DDX_Control(pDX, IDC_IPADDRESS2, m_ipa_lochostIP);
	//	DDX_Control(pDX, IDC_STATIC19, m_pic_flag);
	DDX_Control(pDX, IDC_STATIC19, m_pic_flag);
	DDX_Control(pDX, IDC_STATIC16, m_edt_recvNum);
	DDX_Control(pDX, IDC_STATIC17, m_edt_sendNum);
	DDX_Control(pDX, IDC_STATIC20, m_stc_newCon);
	DDX_Control(pDX, IDC_BUTTON1, m_btn_conDis);
	DDX_Control(pDX, IDC_STATIC15, m_edt_connectState);
	DDX_Control(pDX, IDC_EDIT4, m_edit_type);
	DDX_Control(pDX, IDC_EDIT6, m_edt_unit);
	DDX_Control(pDX, IDC_EDIT5, m_edt_value);
}

BEGIN_MESSAGE_MAP(CNetDataParseDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO1, &CNetDataParseDlg::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON1, &CNetDataParseDlg::OnBnClickedButton1)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS2, &CNetDataParseDlg::OnIpnFieldchangedIpaddress2)
	ON_EN_CHANGE(IDC_EDIT4, &CNetDataParseDlg::OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT7, &CNetDataParseDlg::OnEnChangeEdit7)
	ON_WM_TIMER()
END_MESSAGE_MAP()



UINT g_timer;

// CNetDataParseDlg 消息处理程序

BOOL CNetDataParseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	this->InitMenu();
	this->InitNetSetting();
	this->SetConectState(FALSE);
	//m_ckb_recv2.SetCheck(TRUE);

	m_nSendCount = 0;
	m_nRecvCount = 0;
	m_bConnect = FALSE;

	// 设置套接字回调函数
	m_server.m_pUserParam = this;
	m_server.NewSocketCallBack = SNewSocketCallBack;
	m_server.NewMsgCallBack = SNewMsgCallBack;
	m_server.CloseSocketCallBack = SCloseSocketCallBack;

	//设置字体
	static CFont font;
	font.DeleteObject();
	font.CreatePointFont(260, _T("新宋体"));

	CRect rect;
	m_edit_type.GetClientRect(&rect);

	CDC * pDC = m_edit_type.GetDC();
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	int nFontHeight = tm.tmHeight + tm.tmExternalLeading;
	int nMargin = (rect.Height() - nFontHeight) / 2;
	rect.DeflateRect(0,nMargin);
	m_edit_type.SetRectNP(&rect);

	CBitmap bitmap;
	HBITMAP hBmp;

	bitmap.LoadBitmap(IDI_ICON_DISCON);
	hBmp = (HBITMAP)bitmap.GetSafeHandle();
	m_pic_flag.SetBitmap(hBmp);


	m_edit_type.SetFont(&font);
	m_edt_unit.SetFont(&font);
	m_edt_value.SetFont(&font);
	m_edt_recvData.SetLimitText(20);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CNetDataParseDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CNetDataParseDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CNetDataParseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
// 加载菜单项
void CNetDataParseDlg::InitMenu()
{
	m_Menu.LoadMenu( IDR_MENU1 );
	this->SetMenu(&m_Menu);
}

// 设置连接/断开图片标识，连接/断开按钮，状态栏连接状态
void CNetDataParseDlg::SetConectState(BOOL bConnect)
{
	HICON hIcon;
	CString strBtn, strEdt;
	if ( bConnect )
	{
		//hIcon = AfxGetApp()->LoadIcon( IDI_ICON_DISCON );
		strBtn = _T("断开");
		strEdt = _T("已连接");
	}
	else
	{
		//hIcon = AfxGetApp()->LoadIcon( IDI_ICON_DISCON );
		strBtn = _T("连接");
		strEdt = _T("未连接");
	}

	//m_pic_flag.SetIcon(hIcon);
	m_btn_conDis.SetWindowText(strBtn);
	m_edt_connectState.SetWindowText(_T("连接状态：") + strEdt);
}

void CNetDataParseDlg::InitNetSetting()
{
	// 协议类型
	m_com_NetProtocol.AddString(_T("UDP"));
	m_com_NetProtocol.AddString(_T("TCP Server"));
	m_com_NetProtocol.AddString(_T("TCP Client"));
	m_com_NetProtocol.SetCurSel(0);

	// 本地IP地址
	char *pszLocIp = m_server.GetLocalIP();
	if (pszLocIp = "0.0.0.0")
		pszLocIp = "127.0.0.1";
	m_ipa_lochostIP.SetWindowText((LPCTSTR)pszLocIp);

	// 本地端口号
	m_edt_locHostPort.SetWindowText(_T("8080"));
}

//协议类型改变
void CNetDataParseDlg::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码

	CString strLocalIP;
	CString strLocalPort;
	// TCP Client
	if (m_com_NetProtocol.GetCurSel() == 0)
	{
		strLocalIP = _T("服务器IP地址");
		strLocalPort = _T("服务器端口");
	}
	else
	{
		strLocalIP = _T("本地IP地址");
		strLocalPort = _T("本地端口号");
	}

	m_stc_local_IP.SetWindowText(strLocalIP);
	m_stc_local_Port.SetWindowText(strLocalPort);
}

// 获取IP
CString CNetDataParseDlg::GetIPAddr(CIPAddressCtrl *ipCtrl)
{
	BYTE byField0, byField1, byField2, byField3;
	ipCtrl->GetAddress(byField0, byField1, byField2, byField3);
	CString strIP;
	strIP.Format(_T("%d.%d.%d.%d"), byField0, byField1, byField2, byField3);
	return strIP;
}

// 设置接收区内容
void CNetDataParseDlg::SetRecvData(CString strData)
{
	CString strOld;
	m_edt_recvData.GetWindowText(strOld);	

	TCHAR buff[128];
	int nCount = m_edt_recvData.GetLine( m_edt_recvData.GetLineCount() - 1,buff, 128);
	if ( m_ckb_recv2.GetCheck() && nCount != 0)
		strData = _T("\r\n") + strData;
	strOld += strData;
	m_edt_recvData.SetWindowText(strOld);
	int nLineCount = m_edt_recvData.GetLineCount();
	m_edt_recvData.LineScroll(nLineCount - 1); // 设置滚动条一直在底部
}

// 显示接收到的传感器数据
void CNetDataParseDlg::DisplayData(const char* strData,UINT len)
{
	CString str_data;
	CString temp;
	char crc[2];
	char crc_buf[128];
	CString strOld;
	TCHAR buff[128];
	//CRC 校验
	WORD CRC_t,CRC_Parse;

	CRC_Parse = 0;
	memset(crc_buf,0,128);
	memset(crc,0,2);
	
	for(int j=0;j<len;j++)
	{
		crc_buf[j]=strData[j];
	}
	CRC_t=m_CRC.CalCrcFast(crc_buf,5);
	crc[0] = CRC_t & 0xff;
	crc[1] = CRC_t>>8 & 0xff;
	//if(crc[0]==crc_buf[5] && crc[1]==crc_buf[6]) //CRC 校验成功
	if(1)
	{
		m_edt_recvData.GetWindowText(strOld);	
		str_data.Format("%02X ",(unsigned char)strData[0]);
		for(int i=1;i<len;i++)
		{
			temp.Format("%02X ",(unsigned char)strData[i]);
			str_data +=temp;
		}

		int nCount = m_edt_recvData.GetLine( m_edt_recvData.GetLineCount() - 1,buff, 128);
		if ( m_ckb_recv2.GetCheck() && nCount != 0)
			str_data = _T("\r\n") + str_data;
		
	

		strOld += str_data;
		m_edt_recvData.SetWindowText(strOld);
		int nLineCount = m_edt_recvData.GetLineCount();
		m_edt_recvData.LineScroll(nLineCount - 1); // 设置滚动条一直在底部
	}
	else
	{
		
		m_edt_recvData.GetWindowText(strOld);	
		str_data = "接收数据错误\r\n";
		int nCount = m_edt_recvData.GetLine( m_edt_recvData.GetLineCount() - 1,buff, 128);
		if ( m_ckb_recv2.GetCheck() && nCount != 0)
			str_data = _T("\r\n") + str_data;
		strOld += str_data;
		//data.Format("%02X %02X %02X %02X",strData[0],strData[1],strData[2],strData[3]);
		m_edt_recvData.SetWindowText(strOld);
		int nLineCount = m_edt_recvData.GetLineCount();
		m_edt_recvData.LineScroll(nLineCount - 1); // 设置滚动条一直在底部

	}
}


// 解析传感器数据
void CNetDataParseDlg::ParseData(const char * strData)
{

	//数据校验


	//Sender.SendToServier((char*)strData);
	//解析传感器类型
	

	if(strData[2] == 0x02)
	{
		CString device_type;
		device_type = "可燃气体";   //可燃气体
		m_edit_type.SetWindowText(device_type);
		m_edt_unit.SetWindowText("ppm");

		char data_real[4];
		CString data;

		data_real[0] = (strData[3]>>4)&0x0f;
		data_real[1] = strData[3]&0x0f;
		data_real[2] = (strData[4]>>4)&0x0f;
		data_real[3] = (strData[4]&0x0f);
		int a;
		a = (data_real[0]*16*16*16) + (data_real[1]*16*16) + (data_real[2]*16) + data_real[3];
		int b[2];
		b[0]= a/100;
		b[1]= a%100;
		data.Format("%d.%d",b[0],b[1]); 
		//data.Format("%d",a);

		m_edt_value.SetWindowText(data);

	}
	else if(strData[2] == 0x0E)
	{
		CString device_type;
		device_type = "温度";   //温度
		m_edit_type.SetWindowText(device_type);
		m_edt_unit.SetWindowText("℃");

		CString data;

		char data_real[4];

		data_real[0] = (strData[15]>>4)&0x0f;
		data_real[1] = strData[15]&0x0f;
		data_real[2] = (strData[16]>>4)&0x0f;
		data_real[3] = (strData[16]&0x0f);
		int a;
		a = (data_real[0]*16*16*16) + (data_real[1]*16*16) + (data_real[2]*16) + data_real[3];
		int b[2];
		b[0]= (a-400)/10;
		b[1]= (a-400)%10;
		//data.Format("%d.%d",b[0],b[1]); 

		data.Format("%d.%d",b[0],b[1]);

		m_edt_value.SetWindowText(data);
	
	}
	else if(strData[0] == TYPE_HUMIDITY+1)
	{
		CString device_type;
		device_type = "湿度";   //湿度
		m_edit_type.SetWindowText(device_type);
		m_edt_unit.SetWindowText("%");

		CString data;

		data.Format("%d.%d",(unsigned char)strData[3],(unsigned char)strData[4]);

		m_edt_value.SetWindowText(data);
	}
	else if(strData[0] == TYPE_UITRAVIOLET+1)
	{


	}
	else if(strData[0] == 0x73)
	{
		CString device_type;
		device_type = "称重器";   
		m_edit_type.SetWindowText(device_type);
		m_edt_unit.SetWindowText("Kg");

		CString data;
		float Number;
		Number = (strData[20]*16+strData[21])*0.1;
		data.Format("%.1f",Number);

		m_edt_value.SetWindowText(data);

		char buffer[6];

		buffer[0] = 0x74;
		buffer[1] = 0x04;
		buffer[2] = 0xF0;
		buffer[3] = strData[3];
		buffer[4] = strData[4];
		buffer[5] = 0x00;
		m_server.SendData(m_VecSocket.at(0),buffer,6);
	}



}


// 设置网络设置控件是否可编辑
void CNetDataParseDlg::SetNetSetCtrlEnable(BOOL Enable)
{
	m_com_NetProtocol.EnableWindow(Enable);
	m_stc_local_IP.EnableWindow(Enable);
	m_stc_local_Port.EnableWindow(Enable);
}


BOOL CNetDataParseDlg::CheckPortIsOccupy(DWORD dwPort)
{
	PMIB_TCPTABLE pTcptable;
	pTcptable = (MIB_TCPTABLE*)malloc(sizeof(MIB_TCPTABLE));
	DWORD dwsize = sizeof(MIB_TCPTABLE);
	if (GetTcpTable(pTcptable, &dwsize, 2) == ERROR_INSUFFICIENT_BUFFER)
	{
		free(pTcptable);
		pTcptable = (PMIB_TCPTABLE)malloc(dwsize); 
	}

	if (GetTcpTable(pTcptable, &dwsize, 2) == NO_ERROR)
	{
		for (int i = 0; i < (int)pTcptable->dwNumEntries; i++)
		{
			DWORD port = ntohs(pTcptable->table[i].dwLocalPort);
			if (port == dwPort)
			{
				return TRUE;
			} 
		}
	} 

	free(pTcptable);
	return FALSE;
}

// 获取端口号
DWORD CNetDataParseDlg::GetPort(CEdit *edtCtrl)
{
	CString strPort;
	edtCtrl->GetWindowText(strPort);
	int nPort = _ttoi(strPort);
	return (DWORD)nPort;
}



// TCP Server
void CNetDataParseDlg::TCPSerConOrDis()
{
	//已经连接的情况下
	if(m_bConnect)
	{
		if( m_server.StopListening()) //停止监听
		{
			SetRecvData(_T("服务器已断开连接！\r\n"));
			m_bConnect = FALSE;
			SetConectState(FALSE);
			//m_cmb_conTar.ResetContent();
			//m_cmb_conTar.AddString(_T("All Connections"));
			//m_cmb_conTar.setCursel(0);

			SetNetSetCtrlEnable(TRUE);
			CloseConnect();
			KillTimer(TYPE_GAS);
			//ResizeRecvCtrl(FALSE);
			//SetSerCtrlAreaVisable(FALSE);
		}
		else
		{
			SetRecvData(_T("服务器断开连接错误! \r \n"));
		}
		return;
	}

	if(!m_bConnect)
	{
		DWORD dwPort = GetPort(&m_edt_locHostPort);
		//检测端口是否可用
		if(CheckPortIsOccupy(dwPort))
		{
			SetRecvData(_T("创建服务器失败，端口已经被其他应用程序占用!\r\n"));
			return;
		}
		if(m_server.StartListening(dwPort,TCP_TYPE))
		{
			SetRecvData(_T("服务器已创建！\r\n"));
			m_bConnect = TRUE;
			SetConectState(TRUE);

			SetNetSetCtrlEnable(FALSE);

			ConnectServer("abc");
			//ResizeRecvCtrl(TRUE);
			//SetSerCtrlAreaVisable(TRUE);
		
		}
		else
		{
			SetRecvData(_T("创建服务器失败！\r\n"));
		}
		return;
	}

}

// 接收字节数
void CNetDataParseDlg::SetRecvNum()	
{
	CString strRecvNum;
	strRecvNum.Format("%d", m_nRecvCount);
	m_edt_recvNum.SetWindowText(_T("接收：") + strRecvNum);
}

// 发送字节数
void CNetDataParseDlg::SetSendNum()	
{
	CString strSendNum;
	strSendNum.Format("%d", m_nSendCount);
	m_edt_sendNum.SetWindowText(_T("发送：") + strSendNum);
}



void CNetDataParseDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	switch(m_com_NetProtocol.GetCurSel())
	{
		case 0: //TCP Client

			break;
		case 1: //TCP Server
			TCPSerConOrDis();
			break;
		case 2:	//UDP

			break;


	}
}


void CNetDataParseDlg::OnIpnFieldchangedIpaddress2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


BOOL CNetDataParseDlg::ConnectServer(const char* URL)
{
	// 初始化libcurl
	CURLcode return_code;
	
	struct curl_slist *headers = NULL;
	return_code = curl_global_init(CURL_GLOBAL_WIN32);
	if(return_code != CURLE_OK)
	{
		return FALSE;
	}

	m_easy_handle = curl_easy_init();
	curl_easy_setopt(m_easy_handle, CURLOPT_URL,"http://icp.iuoooo.com/device/Jinher.ICP.Device.ISV.ISensorManageSV.svc/SensorReportData");
	/* 设置http协议头*/
	headers = curl_slist_append(headers,"Content-Type:application/json");
	headers = curl_slist_append(headers,"charset:utf-8");
	curl_easy_setopt(m_easy_handle,CURLOPT_HTTPHEADER,headers);
	return TRUE;
}

BOOL CNetDataParseDlg::SendToServier(char * data)
{
	char* p = NULL;
	char buffer[125];
	Json::Value arrayObj;
	struct curl_slist *post = NULL;
	Json::Value new_item,new_item2;
	long long time_last;  
	time_last = time(NULL);     //总秒数
	struct timeb t1;  
	ftime(&t1); 


	CURLcode Code;

	sprintf(buffer,"/Date(%I64u)/",(time_last*1000+t1.millitm));
	new_item2["Id"] = Json::Value(0);
	new_item2["SampleTime"] = buffer;
	
	memset(buffer,0,125);
	//sprintf(buffer,"%d",data[3]*100+data[4]);


	if(data[0] == 0x02)
	{
		char data_real[4];

		data_real[0] = (data[3]>>4)&0x0f;
		data_real[1] = data[3]&0x0f;
		data_real[2] = (data[4]>>4)&0x0f;
		data_real[3] = (data[4]&0x0f);
		int a;
		a = (data_real[0]*16*16*16) + (data_real[1]*16*16) + (data_real[2]*16) + data_real[3];
		int b[2];
		b[0]= a/100;
		b[1]= a%100;
		//data.Format("%d.%d",b[0],b[1]); 
		sprintf(buffer,"%d.%d",b[0],b[1]);
		new_item2["SampleData"] = buffer;
		new_item2["DeviceType"] = Json::Value(9);
		new_item2["Unit"] = Json::Value("ppm");
		new_item2["SensorID"] = Json::Value("027995465-9-0");
	}
	
	else if(data[2] == 0x0E)
	{
		char data_real[4];

		data_real[0] = (data[15]>>4)&0x0f;
		data_real[1] = data[15]&0x0f;
		data_real[2] = (data[16]>>4)&0x0f;
		data_real[3] = (data[16]&0x0f);
		int a;
		a = (data_real[0]*16*16*16) + (data_real[1]*16*16) + (data_real[2]*16) + data_real[3];
		int b[2];
		b[0]= (a-400)/10;
		b[1]= (a-400)%10;
		//data.Format("%d.%d",b[0],b[1]); 
		sprintf(buffer,"%d.%d",b[0],b[1]);

		new_item2["SampleData"] = buffer;
		new_item2["DeviceType"] = Json::Value(6);
		new_item2["Unit"] = Json::Value("Centigrade");
		new_item2["SensorID"] = Json::Value("027dtrxde-6-0");
		
	} 
	else if(data[0] == TYPE_HUMIDITY + 1)
	{
		new_item2["DeviceType"] = Json::Value(7);
		new_item2["Unit"] = Json::Value("%");
		new_item2["SensorID"] = Json::Value("027978421-7-0");
	}
	
	
	new_item["OperationEnum"] = Json::Value(52);
	new_item["Data"].append(new_item2);
	memset(buffer,0,125);
	sprintf(buffer,"/Date(%I64u)/",(time_last*1000+t1.millitm));
	new_item["ReportTime"] = buffer;
	new_item["DeviceType"] = 0;
	new_item["ProductorCode"] = "";
	new_item["DeviceSN"] = "";
	new_item["DeviceID"] = "0";
	new_item["DeviceGlobalKey"] = "-";
	new_item["SessionId"] = "";
	new_item["CallbackURL"] = "";
	new_item["CallbackTopic"] = "";
	new_item["OpId"] = "63e57858-3122-48f3-8ce7-2a6513c4afdb";
	//new_item["PExtendObj"] = 0;

	arrayObj["pDTO"] = new_item;
	std::string  send_string;
	send_string = arrayObj.toStyledString();
	//post=curl_slist_append(post,send_string);
	const char *buffer_data = send_string.data();
	int len = send_string.length();
	

	curl_easy_setopt(m_easy_handle, CURLOPT_POSTFIELDS,buffer_data);
	curl_easy_setopt(m_easy_handle, CURLOPT_POSTFIELDSIZE,len);
	//curl_easy_setopt(m_easy_handle,CURLOPT_POST,1);
	curl_easy_setopt(m_easy_handle, CURLOPT_LOW_SPEED_LIMIT , 50);
	Code = curl_easy_perform(m_easy_handle);

	if (Code == CURLE_OK)
	{
		//cout << "upload successfully." << endl;
		return TRUE;
	}
	else
	{
		return FALSE;
	}

	
}

void CNetDataParseDlg::CloseConnect()
{
	curl_easy_cleanup(m_easy_handle);
	curl_global_cleanup();
}


/************************************************************************/
/*                           套接字回调函数                         */
/************************************************************************/

// 服务器端 Server
// 客户端连接
void SNewSocketCallBack(const SOCKADDR_IN *szSockAddr, SOCKET newSocket, void *pUserParam)
{
	CNetDataParseDlg *p = (CNetDataParseDlg *)pUserParam;
	p->m_VecSocket.push_back(newSocket);

	CString strAddr = inet_ntoa(szSockAddr->sin_addr); // 得到发送者IP地址
	char szPort[6];
	itoa(szSockAddr->sin_port, szPort, 10);	// 得到发送者本地端口
	CString strAddrPort = strAddr + _T(":") + szPort;
	//p->m_cmb_conTar.AddString(strAddrPort);
	p->SetTimer(TYPE_GAS,5000,NULL);
	p->SetTimer(TYPE_TEMPERATURE,5000,NULL);

	//p->m_stc_newCon.SetWindowText(_T("New Connection:") + strAddrPort);
}

// 接收消息
void SNewMsgCallBack(const SOCKADDR_IN *pSockAddr, const SOCKET_HAND *pSocketHand, const SOCKET_DATA *pData, void *pUserParam)
{
	CNetDataParseDlg *p = (CNetDataParseDlg *)pUserParam;

	CString strAddr = inet_ntoa(pSockAddr->sin_addr); // 得到发送者IP地址
	char szPort[6];
	itoa(pSockAddr->sin_port, szPort, 10);	// 得到发送者本地端口
	CString strAddrPort = strAddr + _T(":") + szPort;

	CString strAddrPortOld = g_strAddrAndPort;
	g_strAddrAndPort = strAddrPort;
	if (strAddrPortOld != strAddrPort)
	{
		p->SetRecvData(_T("【Receive from ") + strAddrPort + _T("】:\r\n"));
	}
	if(!(pData->pData[2] == 0x02 || pData->pData[2] == 0x0E))
	{
		return;
	}
	p->DisplayData(pData->pData,pData->len);

	if(p->SendToServier((char *)pData->pData))
	{
		//std::cout << "Send Success"<<endl;
	}
	/*char buffer[256];
	long  len;
	memcpy(buffer,pData->pData,pData->len);
	len = pData->len;
	p->m_server.SendData(p->m_VecSocket.at(0),buffer,len);*/
	p->ParseData(pData->pData);
	
	p->m_nRecvCount += pData->len;
	p->SetRecvNum();
}

// 客户端断开连接
void SCloseSocketCallBack(const char *szSocket, SOCKET newSocket, void *pUserParam)
{
	CNetDataParseDlg *p = (CNetDataParseDlg *)pUserParam;
	p->m_stc_newCon.SetWindowText(szSocket);
	
	if (p->m_VecSocket.empty())
		return;

	UINT nItem = p->findSocketInVector(newSocket);
	if (nItem != -1)
	{
		//p->m_cmb_conTar.DeleteString(nItem+1);
		//p->m_cmb_conTar.SetCurSel(nItem);
	}
}


// 返回-1表示失败，否则返回索引
UINT CNetDataParseDlg::findSocketInVector(SOCKET newSocket)
{
	UINT nItem = -1;
	UINT i = 0;
	for (vector<SOCKET>::iterator it = m_VecSocket.begin(); it != m_VecSocket.end(); )
	{
		if (*it == newSocket)
		{
			m_VecSocket.erase(it);
			nItem = i;
			break;
		}
		it++;
		i++;
	}

	return nItem;
}

void CNetDataParseDlg::OnEnChangeEdit4()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CNetDataParseDlg::OnEnChangeEdit7()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CNetDataParseDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	char * data = new char[256];
	memset(data,0,256);
	int len = 0;
	switch(nIDEvent)
	{
		case TYPE_GAS:
			data[len++] = 0x02;		//设备类型
			data[len++] = 0x03;		
			data[len++] = 0x00;		
			data[len++] = 0x00;		
			data[len++] = 0x00;		
			data[len++] = 0x01;		
			data[len++] = 0x84;		
			data[len++] = 0x39;

			break;
		case TYPE_TEMPERATURE:
			data[len++] = 0x01;		//设备类型
			data[len++] = 0x03;		
			data[len++] = 0x00;		
			data[len++] = 0x00;		
			data[len++] = 0x00;		
			data[len++] = 0x07;		
			data[len++] = 0x04;		
			data[len++] = 0x08;

			break;
	}

	int i=0;
	for (vector<SOCKET>::iterator it = m_VecSocket.begin(); it != m_VecSocket.end();it++)
	{
		m_server.SendData(m_VecSocket[i],data,8);
		i++;
	}
	

	delete [] data;
}
