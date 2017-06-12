#include "stdafx.h"

LONG display_array[101][101];
int	id_table[MAX_USER];
HANDLE g_iocp;
SYSTEM_INFO sysInfo;
SOCKADDR_IN clntAdr;
DWORD recvBytes, flags = 0;
unsigned int randNum;
HINSTANCE g_hInst;
playerInfo player[MAX_PLAYER_NUM];
Objlist objectList;
HWND g_hWnd = NULL;
CRITICAL_SECTION cs;
wchar_t *g_ClassName = L"더미클라이언트";
wchar_t *g_WindowName = L"더미클라이언트";
HWND hIp;
TCHAR tc_ip[20];
char c_ip[20];
int g_threadCount;
HANDLE *h_threadHandle;


LRESULT CALLBACK	MsgProc(HWND, UINT, WPARAM, LPARAM);
BOOL InitWindow(int width, int height);
void ResizeWindow(HWND hWnd, UINT NewWidth, UINT NewHeight);
BOOL MessagePump();
void DisplayUpdateVB(int display_array_index);
void InitializeDisplay();
void AddRecvObjectList(int user_id);
void AddDisplayArray(int user_id);
bool CheckContainPlayer(int user_id);
int GetPhysicalProcessorsNum(void);
void RandomMove(cs_packet_up* MovePacket, int move_flag);
HRESULT UpdateVB( int row, int col, int input_bSelect );

void ProcessMovePacket(char *packet);
void ProcessLoginOkPacket(int user_id, char *packet);
void ProcessPlayerInfoPacket(int user_id, char *packet);
void ProcessPutObjectPacket(int user_id, char *packet);
bool ProcessPacket(int user_id, char *packet);

BOOL CALLBACK InputServerIpDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);//서버주소입력 대화 상자
