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
//win socket处理服务器端
class OVERLAPPEDSOCKET_API CServerSocket
{
private:
	friend UINT _ServerListenThread(LPVOID lParam);
	friend DWORD WINAPI _OverlappedThread(LPVOID lParam);
public:	
	CServerSocket(void);
	~CServerSocket(void);
	char * GetLocalIP();
	int StartListening(const UINT& port,long nType = TCP_TYPE);//打开端口，绑定IP地址，选择通讯协议
	int StopListening();//通讯结束后，停止监听
	int BeginListening();
	//发送信息
	long SendData(SOCKET socket,char * buf,long len);
	//指定广播的地址
	long SetAddr(char ** pszIp,long nLen);
public:
	HWND m_hNotifyWnd;//通知窗口的句柄
	DWORD m_msgNewMsg;//收到新的消息的通知
	DWORD m_msgNewSock;//新建套接字的消息
	DWORD m_msgCloseSock;//关闭套接字的消息
	SOCKET m_sockListen;//监听的套接字
	char m_szErrDesc[64];
	UINT m_nPort;//端口号
	long m_nSocketType;//套接字的类型
	void (*NewSocketCallBack)(const SOCKADDR_IN *szSockAddr, SOCKET newSocket, void * pUserParam);
	void (*NewMsgCallBack)(const SOCKADDR_IN *pSockAddr, const SOCKET_HAND *pSocketHand, const SOCKET_DATA* pData, void * pUserParam);
	void (*CloseSocketCallBack)(const char *szSocket, SOCKET newSocket, void * pUserParam);
	void *m_pUserParam;//无类型指针可以包容有类型指针，反过来则不行
private:
	SOCKADDR_IN * m_pAddLst;
	long		  m_nAddLen;
	//**********************************************************************
	CWinThread* m_pServerListenThread;//监听线程
	HANDLE m_pOverlappedThread;//重叠线程
	SOCKET m_sockAcceptArray[WSA_MAXIMUM_WAIT_EVENTS]; // 与客户端通信的SOCKET
	WSAEVENT m_eventArray[WSA_MAXIMUM_WAIT_EVENTS];    // 与重叠结构配套的事件组
	SOCKET_DATA     m_socketData[WSA_MAXIMUM_WAIT_EVENTS];//WSA_MAXIMUM_WAIT_EVENTS = 64
	SOCKET_HAND		m_socketHand[WSA_MAXIMUM_WAIT_EVENTS];
	SOCKADDR_IN		m_clientAddr[WSA_MAXIMUM_WAIT_EVENTS]; // 定义一个客户端得地址结构作为参数
	WSAOVERLAPPED m_acceptOverlapped[WSA_MAXIMUM_WAIT_EVENTS]; // 重叠结构，每个SOCKET操作对应一个

	WSABUF m_dataBuf[WSA_MAXIMUM_WAIT_EVENTS];  // 接收缓冲区，WSARecv的参数，每个SCOKET对应一个

	DWORD m_dwEventTotal;//事件线程总数
	DWORD m_dwRecvBytes;
	DWORD m_flags;
	long m_nMaxIdx;
	BOOL m_bOverlapped;       // 是否处理重叠请求
	BOOL m_bListen;
	//**********************************************************************
	int GetEmptySocket();
	void freeOverlappenThread();
	void freeListenThread();
};
//
//win socket处理服客户端
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
	//需在ConnectServer前调用
	void SetSockType(long nType=TCP_TYPE);
	//需在ConnectServer前调用
	void SetSync(BOOL bSync);
	BOOL ConnectServer(const char *szIp, DWORD nPort, DWORD nLocalPort=0);
	void CloseSocket();
	int SendData(char * buf,long len,sockaddr *pAddr=NULL);
	//同步模式接收数据
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
	BOOL	m_bSync;//是否同步模式，默认为异步模式
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
