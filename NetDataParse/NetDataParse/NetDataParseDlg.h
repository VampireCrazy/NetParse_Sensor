
// NetDataParseDlg.h : ͷ�ļ�
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
	TYPE_GAS,				//��ȼ����
	TYPE_TEMPERATURE,		//�¶�
	TYPE_HUMIDITY,			//ʪ��
	TYPE_UITRAVIOLET,		//�����
}Transducer_TYPE_E;

// CNetDataParseDlg �Ի���
class CNetDataParseDlg : public CDialogEx
{
// ����
public:
	CNetDataParseDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_NETDATAPARSE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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

	// �׽���
	CServerSocket m_server;
	CClientSocket m_client;

	vector<SOCKET> m_VecSocket;		// ����ͻ����׽���


public:
	CComboBox m_com_NetProtocol;
	CStatic m_stc_local_IP;
	CStatic m_stc_local_Port;
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnBnClickedButton1();
/******************************function************************************/
	void		InitMenu();										// ��ʼ���˵���
	void		InitNetSetting();								// ��ʼ����������
	void		SetConectState(BOOL bConnect = FALSE);			// ��������״̬
	void		TCPSerConOrDis();								// TCP Server
	CString		GetIPAddr(CIPAddressCtrl *ipCtrl);				// ��ȡ�����ϵ�IP��ַ
	DWORD		GetPort(CEdit *edtCtrl);						// ��ȡ�����ϵĶ˿ں�
	void		SetRecvData(CString strData);					// ���ý���������
	void		DisplayData(const char * strData,UINT len);		// ���ô���������
	void		ParseData(const char * strData);				// ��������������
	void		SetNetSetCtrlEnable(BOOL Enable = TRUE);		// �����������ÿؼ��Ƿ�ɱ༭
	BOOL		CheckPortIsOccupy(DWORD dwPort);				// ���TCP�˿��Ƿ�ռ��
	void		ResizeRecvCtrl(BOOL bSmall = FALSE);			// �����������ݽ������ؼ���С


	void SetRecvNum();											// �����ֽ���
	void SetSendNum();											// �����ֽ���

	UINT findSocketInVector(SOCKET newSocket);					// �����в��Ҷ˿ں�


	CRC			m_CRC;											// CRC ����У��

	CURL *m_easy_handle;

	BOOL ConnectServer(const char* URL);
	BOOL SendToServier(char* data);
	void CloseConnect();

	



	CEdit m_edt_recvData;
	// �Զ�������ʾ
	CButton m_ckb_recv2;
	CEdit m_edt_locHostPort;
	afx_msg void OnIpnFieldchangedIpaddress2(NMHDR *pNMHDR, LRESULT *pResult);
	CIPAddressCtrl m_ipa_lochostIP;
	CStatic m_pic_flag;
	CStatic m_edt_recvNum;									//��ʾ�����ַ���Ŀ
	CStatic m_edt_sendNum;									//��ʾ�����ֽ���Ŀ
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
