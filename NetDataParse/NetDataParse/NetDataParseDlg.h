
// NetDataParseDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "ZYSocket.h"
#include "afxcmn.h"
#include "crc.h"
#include "curl.h"
#include <vector>
using namespace std;

#define  SERVER_URL ""

typedef enum _tagTransducer_TYPE
{
	TYPE_GAS,				//可燃气体
	TYPE_TEMPERATURE,		//温度
	TYPE_HUMIDITY,			//湿度
	TYPE_UITRAVIOLET,		//紫外光
}Transducer_TYPE_E;

// CNetDataParseDlg 对话框
class CNetDataParseDlg : public CDialogEx
{
// 构造
public:
	CNetDataParseDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_NETDATAPARSE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	BOOL m_bConnect;
	CMenu m_Menu;
	int m_nRecvCount;
	int m_nSendCount;

	// 套接字
	CServerSocket m_server;
	CClientSocket m_client;

	vector<SOCKET> m_VecSocket;		// 保存客户端套接字


public:
	CComboBox m_com_NetProtocol;
	CStatic m_stc_local_IP;
	CStatic m_stc_local_Port;
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnBnClickedButton1();
/******************************function************************************/
	void		InitMenu();										// 初始化菜单栏
	void		InitNetSetting();								// 初始化网络设置
	void		SetConectState(BOOL bConnect = FALSE);			// 设置连接状态
	void		TCPSerConOrDis();								// TCP Server
	CString		GetIPAddr(CIPAddressCtrl *ipCtrl);				// 获取界面上的IP地址
	DWORD		GetPort(CEdit *edtCtrl);						// 获取界面上的端口号
	void		SetRecvData(CString strData);					// 设置接收区内容
	void		DisplayData(const char * strData,UINT len);		// 设置传感器数据
	void		ParseData(const char * strData);				// 解析传感器数据
	void		SetNetSetCtrlEnable(BOOL Enable = TRUE);		// 设置网络设置控件是否可编辑
	BOOL		CheckPortIsOccupy(DWORD dwPort);				// 检测TCP端口是否被占用
	void		ResizeRecvCtrl(BOOL bSmall = FALSE);			// 设置网络数据接收区控件大小


	void SetRecvNum();											// 接收字节数
	void SetSendNum();											// 发送字节数

	UINT findSocketInVector(SOCKET newSocket);					// 容器中查找端口号


	CRC			m_CRC;											// CRC 数据校验

	CURL *m_easy_handle;

	BOOL ConnectServer(const char* URL);
	BOOL SendToServier(char* data);
	void CloseConnect();

	



	CEdit m_edt_recvData;
	// 自动换行显示
	CButton m_ckb_recv2;
	CEdit m_edt_locHostPort;
	afx_msg void OnIpnFieldchangedIpaddress2(NMHDR *pNMHDR, LRESULT *pResult);
	CIPAddressCtrl m_ipa_lochostIP;
	CStatic m_pic_flag;
	CStatic m_edt_recvNum;									//显示接收字符数目
	CStatic m_edt_sendNum;									//显示发送字节数目
	CStatic m_stc_newCon;
	CButton m_btn_conDis;
	CStatic m_edt_connectState;
	CEdit m_edit_type;
	CEdit m_edt_unit;
	CEdit m_edt_value;
	afx_msg void OnEnChangeEdit4();
	afx_msg void OnEnChangeEdit7();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
