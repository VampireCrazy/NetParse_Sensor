#if !defined(OVERLAPPED_SOCKET_H)
#define OVERLAPPED_SOCKET_H


#include <WinSock2.h>

#define WM_MSG_NEW_SOCKET		(WM_USER+0x0101)
#define WM_MSG_NEW_MSG			(WM_USER+0x0102)
#define WM_MSG_NEW_SVR_MSG		(WM_USER+0x0103)
#define WM_MSG_CLOSE_SOCKET		(WM_USER+0x0104)
#define TCP_TYPE        1
#define UDP_TYPE		2
#define DATA_BUFSIZE 4096


#ifdef OVERLAPPEDSOCKET_EXPORTS
#define OVERLAPPEDSOCKET_API __declspec(dllexport)
#else
#define OVERLAPPEDSOCKET_API __declspec(dllimport)
#endif
//	
#ifdef __cplusplus
extern "C" {
#endif
//
class CWinThread;
typedef struct tagSOCKET_DATA
{	
	const char * pData;
	long len;
}SOCKET_DATA;
typedef struct tagSOCKET_HAND
{
	SOCKET	svrSocket;
	SOCKET  cltSocket;
}SOCKET_HAND;
//win socket�����������
class OVERLAPPEDSOCKET_API CServerSocket
{
private:
	friend UINT _ServerListenThread(LPVOID lParam);
	friend DWORD WINAPI _OverlappedThread(LPVOID lParam);
public:	
	CServerSocket(void);
	~CServerSocket(void);
	char * GetLocalIP();
	int StartListening(const UINT& port,long nType = TCP_TYPE);//�򿪶˿ڣ���IP��ַ��ѡ��ͨѶЭ��
	int StopListening();//ͨѶ������ֹͣ����
	int BeginListening();
	//������Ϣ
	long SendData(SOCKET socket,char * buf,long len);
	//ָ���㲥�ĵ�ַ
	long SetAddr(char ** pszIp,long nLen);
public:
	HWND m_hNotifyWnd;//֪ͨ���ڵľ��
	DWORD m_msgNewMsg;//�յ��µ���Ϣ��֪ͨ
	DWORD m_msgNewSock;//�½��׽��ֵ���Ϣ
	DWORD m_msgCloseSock;//�ر��׽��ֵ���Ϣ
	SOCKET m_sockListen;//�������׽���
	char m_szErrDesc[64];
	UINT m_nPort;//�˿ں�
	long m_nSocketType;//�׽��ֵ�����
	void (*NewSocketCallBack)(const SOCKADDR_IN *szSockAddr, SOCKET newSocket, void * pUserParam);
	void (*NewMsgCallBack)(const SOCKADDR_IN *pSockAddr, const SOCKET_HAND *pSocketHand, const SOCKET_DATA* pData, void * pUserParam);
	void (*CloseSocketCallBack)(const char *szSocket, SOCKET newSocket, void * pUserParam);
	void *m_pUserParam;//������ָ����԰���������ָ�룬����������
private:
	SOCKADDR_IN * m_pAddLst;
	long		  m_nAddLen;
	//**********************************************************************
	CWinThread* m_pServerListenThread;//�����߳�
	HANDLE m_pOverlappedThread;//�ص��߳�
	SOCKET m_sockAcceptArray[WSA_MAXIMUM_WAIT_EVENTS]; // ��ͻ���ͨ�ŵ�SOCKET
	WSAEVENT m_eventArray[WSA_MAXIMUM_WAIT_EVENTS];    // ���ص��ṹ���׵��¼���
	SOCKET_DATA     m_socketData[WSA_MAXIMUM_WAIT_EVENTS];//WSA_MAXIMUM_WAIT_EVENTS = 64
	SOCKET_HAND		m_socketHand[WSA_MAXIMUM_WAIT_EVENTS];
	SOCKADDR_IN		m_clientAddr[WSA_MAXIMUM_WAIT_EVENTS]; // ����һ���ͻ��˵õ�ַ�ṹ��Ϊ����
	WSAOVERLAPPED m_acceptOverlapped[WSA_MAXIMUM_WAIT_EVENTS]; // �ص��ṹ��ÿ��SOCKET������Ӧһ��

	WSABUF m_dataBuf[WSA_MAXIMUM_WAIT_EVENTS];  // ���ջ�������WSARecv�Ĳ�����ÿ��SCOKET��Ӧһ��

	DWORD m_dwEventTotal;//�¼��߳�����
	DWORD m_dwRecvBytes;
	DWORD m_flags;
	long m_nMaxIdx;
	BOOL m_bOverlapped;       // �Ƿ����ص�����
	BOOL m_bListen;
	//**********************************************************************
	int GetEmptySocket();
	void freeOverlappenThread();
	void freeListenThread();
};
//
//win socket������ͻ���
typedef struct _SOCKET_INFORMATION {
	char *szIOBuffer;
	WSABUF *pDataBuf;
	WSAOVERLAPPED Overlapped;
	DWORD BytesSEND;
	DWORD BytesRECV;
} SOCKET_INFORMATION, * LPSOCKET_INFORMATION;



class  OVERLAPPEDSOCKET_API CClientSocket
{
public:
	CClientSocket();
	~CClientSocket();
	//����ConnectServerǰ����
	void SetSockType(long nType=TCP_TYPE);
	//����ConnectServerǰ����
	void SetSync(BOOL bSync);
	BOOL ConnectServer(const char *szIp, DWORD nPort, DWORD nLocalPort=0);
	void CloseSocket();
	int SendData(char * buf,long len,sockaddr *pAddr=NULL);
	//ͬ��ģʽ��������
	int RecvDataSync(char *buf, long len);
	BOOL IsConnect();
	SOCKET GetSocketHWnd();
	friend unsigned __stdcall ThreadGetData( void* pArguments );
public:
	HWND m_hNotifyWnd;
	sockaddr m_udpAddr;
	DWORD m_msg;
	char m_szErrDesc[64];
	void (*NewMsgCallBack)(long nPort, SOCKET svrSocket, const SOCKET_DATA* pData, void * pUserParam);
	void (*DisconnetCall)(void *pUsr);
	void *m_pUserParam;
	void *m_pDisConUsr;
private:
	BOOL RecvData();
	void FreeSockBuf();
private:
	long ProcIOEvent(DWORD dwEventIndex);
private:
	SOCKET m_cltSocket;
	HANDLE m_hThread;
	WSAEVENT m_eventArray[1];
	SOCKET_INFORMATION m_SIRecv;
	SOCKET_INFORMATION m_SISend;
	BOOL m_bConnect;
	long	m_type;
	BOOL	m_bSync;//�Ƿ�ͬ��ģʽ��Ĭ��Ϊ�첽ģʽ
};

#ifdef __cplusplus
}
#endif

#ifndef OVERLAPPEDSOCKET_EXPORTS
#ifdef _DEBUG
#pragma comment(lib, "OverlappedSocket.lib")
#else
#pragma comment(lib, "OverlappedSocket.lib")
#endif
#endif

#endif
