#include <WinSock2.h>
#include <Windows.h>
#include <vector>
#include <thread>
#include <iostream>
#include <unordered_set>
#include <mutex>
#include <chrono>
#include <queue>
#include "protocol.h"
#include <set>
#include <sqlext.h>
#include "d3dfun.h"
#include <chrono>
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#include "A_STAR.h"
#define NUM_THREADS 6

#define OP_RECV  1
#define OP_SEND  2
const int MOVE = 0;
const int ATTACK = 5;
const int EVENT_MOVE = 0;
const int LOBBY= 63;
const int ROOMFRAME = 64;
using namespace std;
using namespace chrono;
struct Overlap_ex {
	WSAOVERLAPPED original_overlap;
	int operation;
	WSABUF wsabuf;
	unsigned char iocp_buffer[MAX_BUFF_SIZE];
};
struct terrain
{
	int x, y, sx, sy, num;
};
terrain terr[21];
struct bullet
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 direction;
	int count = 0;
	bool active = false;
};
struct Player {
	float x;
	float y;
	float z;
	int playertype;
	int direction;
	int lastdirection;
	bool bmovetimer;
	int id;
	CGameObject object;
	bullet bullet[15];
	bool bulletalive=0;
	int bulletcount=0;
	int bulletindex=0;
	int playroom;
	bool team = 0;
};
struct Aiplayer {
	Player player;
	bool active = 0;


}aiplayer[100];

struct NPC {
	CGameObject object;
	int hp;
	wchar_t name[100];
	bool is_alive = false;
	bool line;
	bool team = 0;
	int flagline = 0;
	int anim;
	time_point<steady_clock> deathtime;
	bool deathon = false;
	int count, direct;
};

struct Room {
	int index;
	bool visible;
	bool start;
	unordered_set  <int> clients_list;
	int playtime;
	int count;
	chrono::time_point<chrono::steady_clock> timestart;
	chrono::time_point<chrono::steady_clock> minioncreatetimer;
	NPC redminion[100];
	NPC blueminion[100];
	int minionindex = 0;
}Room[1000];

struct event_type {
	int obj_id;
	unsigned int starttime;
	int do_event;
	int target_id;
};
class mycomparison
{
	bool reverse;
public:
	mycomparison() {}
	bool operator() (const event_type lhs, const event_type rhs) const
	{
		return (lhs.starttime > rhs.starttime);
	}
};
int new_id = 0;
int getID;
int errorcode;
bool moveonpacket = true;
priority_queue<event_type, vector<event_type>, mycomparison> timer_queue;
mutex tq_lock;
wchar_t temp1[100];
NPC monsters[NUM_OF_NPC];
int indexdummy=-1;
bool lobbytimer = false;
int startroom = 2;
struct Client {
	SOCKET s;
	bool is_connected;
	unordered_set  <int> view_list;
	mutex vl_lock;
	Player avatar;
	Overlap_ex recv_overlap;
	int	packet_size;
	int previous_size;
	unsigned char packet_buff[MAX_PACKET_SIZE];

};

Client clients[MAX_USER];
SOCKET new_client;
HANDLE g_hIocp;
bool	g_isshutdown = false;
#pragma comment (lib, "ws2_32.lib")



void error_display(char *msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("%s", msg);
	wprintf(L"에러%s\n", lpMsgBuf);
	LocalFree(lpMsgBuf);
}
void HandleDiagnosticRecord(SQLHANDLE      hHandle,
	SQLSMALLINT    hType,
	RETCODE        RetCode)
{
	SQLSMALLINT iRec = 0;
	SQLINTEGER  iError;
	WCHAR       wszMessage[1000];
	WCHAR       wszState[SQL_SQLSTATE_SIZE + 1];


	if (RetCode == SQL_INVALID_HANDLE)
	{
		fwprintf(stderr, L"Invalid handle!\n");
		return;
	}

	while (SQLGetDiagRec(hType,
		hHandle,
		++iRec,
		wszState,
		&iError,
		wszMessage,
		(SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)),
		(SQLSMALLINT *)NULL) == SQL_SUCCESS)
	{
		// Hide data truncated..
		if (wcsncmp(wszState, L"01004", 5))
		{
			fwprintf(stderr, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError);
		}
	}

}

bool Is_InRange(int id, int obj, int range)
{
	int me_x, ob_x;
	if (id < NPC_START) me_x = clients[id].avatar.object.m_d3dxmtxWorld._41;
	//else me_x = monsters[id - NPC_START].x;
	if (obj < NPC_START) ob_x = clients[obj].avatar.object.m_d3dxmtxWorld._41;;
	//else ob_x = monsters[obj - NPC_START].x
	int distx = (me_x - ob_x)*(me_x - ob_x);

	int me_y, ob_y;
	if (id < NPC_START) me_y = clients[id].avatar.object.m_d3dxmtxWorld._42;
//	else me_y = monsters[id - NPC_START].y;
	if (obj < NPC_START) ob_y = clients[id].avatar.object.m_d3dxmtxWorld._42;
	//else ob_y = monsters[obj - NPC_START].y;
	int disty = (me_y - ob_y)*(me_y - ob_y);

	if ((distx + disty) < range * range) return true;
	else return false;
}


void Initialize()
{
	for (auto i = 0; i < MAX_USER; ++i) {
		clients[i].is_connected = false;
		clients[i].recv_overlap.operation = OP_RECV;
		clients[i].recv_overlap.wsabuf.buf =
			reinterpret_cast<CHAR *>(clients[i].recv_overlap.iocp_buffer);
		clients[i].recv_overlap.wsabuf.len =
			sizeof(clients[i].recv_overlap.iocp_buffer);
		clients[i].view_list.clear();
		clients[i].avatar.direction = 2;
		
		clients[i].avatar.bmovetimer = false;
		
		
	}
	//Room[0].clients_list.insert(10);
//	Room[0].clients_list.insert(20);
//	Room[0].clients_list.insert(80);
//	Room[0].clients_list.insert(2000);
	for (auto i = 0; i < 100; ++i) 
	{
		
	}
	_wsetlocale(LC_ALL, L"korean");

	
	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	g_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
}

void SendPacket(int id, unsigned char *packet)
{
	Overlap_ex *over = new Overlap_ex;
	memset(over, 0, sizeof(Overlap_ex));
	over->operation = OP_SEND;
	over->wsabuf.buf = reinterpret_cast<CHAR *>(over->iocp_buffer);
	over->wsabuf.len = packet[0];
	memcpy(over->iocp_buffer, packet, packet[0]);

	int ret = WSASend(clients[id].s, &over->wsabuf, 1, NULL, 0,
		&over->original_overlap, NULL);
	if (0 != ret) {
		int error_no = WSAGetLastError();
		error_display("SendPacket:WSASend", error_no);
		while (true);
	}
}
void SendPutPlayerPacket(int client, int object, int x, int y, int anim, int di)
{
	sc_packet_put_player packet;
	packet.id = object;
	packet.size = sizeof(packet);
	packet.type = SC_PUT_PLAYER;
	packet.x = x;
	packet.z = y;
	//packet.direction = di;
	//wcsncpy_s(packet.message, clients[object].avatar.name, MAX_STR_SIZE);
	
	SendPacket(client, reinterpret_cast<unsigned char *>(&packet));
}
void SendPutPlayerPacket(int client, int object, int x, int y)
{
	sc_packet_put_player packet;
	packet.id = object;
	packet.size = sizeof(packet);
	packet.type = SC_PUT_PLAYER;
	packet.x = x;
	packet.z = y;

	if (object >= NPC_START)
	{
	
	//	wcsncpy_s(packet.message, monsters[object - NPC_START].name, MAX_STR_SIZE);
	}
	else {
		
	//	wcsncpy_s(packet.message, clients[object].avatar.name, MAX_STR_SIZE);
	}

	SendPacket(client, reinterpret_cast<unsigned char *>(&packet));
}
void AddTimer(int obj_id, int do_event, int wakeup_time)
{

	event_type new_event;
	new_event.do_event = do_event;
	new_event.obj_id = obj_id;
	new_event.starttime = wakeup_time + GetTickCount();
	tq_lock.lock();
	timer_queue.push(new_event);
	tq_lock.unlock();
}
void awake_monster(int obj,int timer)
{
//	AddTimer(obj, EVENT_MOVE, timer);
}
int API_awake_monster(lua_State *L)
{
	int user_id = (int)lua_tointeger(L, -2);
	int timer = (int)lua_tointeger(L, -1);
	lua_pop(L, 3);
	
	awake_monster(user_id+NPC_START, timer);
	return 1;
}



void firstputplayer()
{
	clients[new_id].avatar.object.SetPosition(D3DXVECTOR3(1000, 0,1000));
	clients[new_id].avatar.z =0;
	clients[new_id].avatar.id = new_client;
	clients[new_id].s = new_client;
	clients[new_id].packet_size = 0;
	clients[new_id].previous_size = 0;
	memset(&clients[new_id].recv_overlap.original_overlap, 0,
		sizeof(clients[new_id].recv_overlap.original_overlap));
		sc_packet_put_player enter_packet;
		enter_packet.id = new_id;
		enter_packet.size = sizeof(enter_packet);
		enter_packet.type = SC_PUT_PLAYER;
		enter_packet.x = clients[new_id].avatar.x;
		enter_packet.z = clients[new_id].avatar.y;
		//enter_packet.direction = clients[new_id].avatar.direction;
		enter_packet.mtx = clients[new_id].avatar.object.m_d3dxmtxLocal;
	    SendPacket(new_id, reinterpret_cast<unsigned char *>(&enter_packet));
		
}
void SendRemovePlayerPacket(int client, int object)
{
	sc_packet_remove_player packet;
	packet.id = object;
	packet.size = sizeof(packet);
	packet.type = SC_REMOVE_PLAYER;

	SendPacket(client, reinterpret_cast<unsigned char *>(&packet));
}

void ProcessPacket(int id, unsigned char buf[])
{

	int x = clients[id].avatar.x;
	int z = clients[id].avatar.z;
	clients[id].avatar.lastdirection = clients[id].avatar.direction;
	int type = 0;
	switch (buf[1])
	{//충돌체크 
	case CS_UP:
		clients[id].avatar.direction = CS_UP;
		type = MOve;
		break;
	case CS_DOWN:
		clients[id].avatar.direction = CS_DOWN;
		type = MOve;
		break;
	case CS_LEFT:
		clients[id].avatar.direction = CS_LEFT;
		type = MOve;
		break;
	case CS_RIGHT:
		clients[id].avatar.direction = CS_RIGHT;
		type = MOve;
		break;
	case 5:// 5 ,9 ,6 10 대각선처리   0 정지
		clients[id].avatar.direction = 5;
		type = MOve;
		break;
	case 9:
		clients[id].avatar.direction = 9;
		type = MOve;
		break;
	case 6:
		clients[id].avatar.direction = 6;
		type = MOve;
		break;
	case 10 :
		clients[id].avatar.direction = 10;
		type = MOve;
		break;
	case 0:
		clients[id].avatar.direction = 0; // 타이머정지
		break;
	case CS_LEFTROTATION:
		type = ROtation;
	
		break;
	case CS_RIGHTROTATION:

		type = ROtation;
		break;
	case CS_UPROTATION:

		type = ROtation;
		break;
	case CS_DOWNROTATION:

		type = ROtation;
		break;
	case CS_LKVECTOR: {
		cs_packet_lookvector *my_packet = reinterpret_cast<cs_packet_lookvector *>(buf);
			clients[id].avatar.bullet[clients[id].avatar.bulletindex].position.x = clients[id].avatar.object.m_d3dxmtxLocal._41;  //캐릭위치서 옵셋
			clients[id].avatar.bullet[clients[id].avatar.bulletindex].position.y = clients[id].avatar.object.m_d3dxmtxLocal._42+415;
			clients[id].avatar.bullet[clients[id].avatar.bulletindex].position.z = clients[id].avatar.object.m_d3dxmtxLocal._43;
			clients[id].avatar.bullet[clients[id].avatar.bulletindex].direction.x = my_packet->x;// 총방향 ( 카메라 룩벡터)
			clients[id].avatar.bullet[clients[id].avatar.bulletindex].direction.y = my_packet->y;
			clients[id].avatar.bullet[clients[id].avatar.bulletindex].direction.z = my_packet->z;
			if (!clients[id].avatar.bullet[clients[id].avatar.bulletindex].active)
			{
				clients[id].avatar.bullet[clients[id].avatar.bulletindex].active = true;
			}
			clients[id].avatar.bulletindex++;
			if (clients[id].avatar.bulletindex == 15)
			{
				clients[id].avatar.bulletindex = 0;

			}
			if (!clients[id].avatar.bulletalive) {
		
				AddTimer(id, ATTACK, 100);
				clients[id].avatar.bulletalive = true;
			}
		
		break;
	}
	case CS_SOUNDCHAT:
	{
		cs_packet_soundchat *my_packet = reinterpret_cast<cs_packet_soundchat *>(buf);
		cs_packet_soundchat soundchat_packet;
	


		//soundchat_packet.id = id;
		

		CopyMemory(soundchat_packet.buffer, my_packet->buffer, 240);
		printf("%s %d \n", my_packet->buffer, my_packet->length);

	//	CopyMemory(soundchat_packet.buffer, my_packet->buffer, my_packet->length);
		soundchat_packet.length = 2000;
		soundchat_packet.size = sizeof(soundchat_packet);
		soundchat_packet.type = CS_SOUNDCHAT;
		SendPacket(id, reinterpret_cast<unsigned char *>(&soundchat_packet));
	
		for (int i = 0; i < MAX_USER; i++)
		{
			if (i == id)continue;
			if (clients[i].is_connected) {
				SendPacket(i, reinterpret_cast<unsigned char *>(&soundchat_packet));
			}
		}


	}
		break;
	case CS_IDSERCH: {

	}break;
	case CS_RESTART: {

	}break;
	
	case CS_dummyclients: {
		//패킷받는정보  몇번방 선택했는지

		
		indexdummy++;
		
		Room[2 + indexdummy / 6].clients_list.insert(id);
		if (indexdummy % 5 == 0) Room[2 + indexdummy / 6].visible = true;
		clients[id].avatar.playroom=2 + indexdummy / 6;
		if ((indexdummy % 6) % 2 == 0) {
			clients[id].avatar.team = 0;
		}
		else if ((indexdummy % 6) % 2 == 1)
		{
			clients[id].avatar.team = 1;
		}
		Room[2 + indexdummy / 6].count++;
	}break;
	
	case CS_addAIplayer: {
		cs_packet_aipacket *my_packet = reinterpret_cast<cs_packet_aipacket *>(buf);
		int temp = 0;
		for (int i = 0; i < 100; i++)
		{
			if (!aiplayer[i].active)
			{
				aiplayer[i].active = true;
				aiplayer[i].player.playroom = my_packet->roomnum;
				aiplayer[i].player.team = my_packet->team;
				aiplayer[i].player.id =i+5000;
				Room[clients[id].avatar.playroom].clients_list.insert(aiplayer[i].player.id);
				Room[clients[id].avatar.playroom].count++;
				temp = aiplayer[i].player.id;
				break;
			}
		}
		
		for (int i : Room[clients[id].avatar.playroom].clients_list)
		{
			if (i > 4999)
				continue;
			cs_packet_aipacket send_packet;
			send_packet.size = sizeof(cs_packet_aipacket);
			DWORD iobyte;
			send_packet.type = CS_addAIplayer;
			send_packet.id = temp;
			send_packet.team = my_packet->team;
			SendPacket(i, reinterpret_cast<unsigned char *>(&send_packet));
		}



	}break;
	case CS_deleteAIplayer: {
		cs_packet_aipacket *my_packet = reinterpret_cast<cs_packet_aipacket *>(buf);
		
		for (int i : Room[clients[id].avatar.playroom].clients_list)
		{
			if (i == my_packet->id) {
				Room[clients[id].avatar.playroom].clients_list.erase(i);
				Room[clients[id].avatar.playroom].count--;
				aiplayer[i - 5000].active = false;
				break;
			}
		}
		for (int i : Room[clients[id].avatar.playroom].clients_list)
		{
			if (i > 4999)
				continue;
			cs_packet_aipacket  send_packet;
			send_packet.size = sizeof(cs_packet_aipacket);
			DWORD iobyte;
			send_packet.type = CS_deleteAIplayer;
			send_packet.id = my_packet->id;
			SendPacket(i, reinterpret_cast<unsigned char *>(&send_packet));
		}


	}break;
	case CS_gameplay: {
		cs_packet_roominfor *my_packet = reinterpret_cast<cs_packet_roominfor *>(buf);
		if (Room[clients[id].avatar.playroom ].start == false) {
			Room[clients[id].avatar.playroom].start = true;
			Room[clients[id].avatar.playroom].timestart= high_resolution_clock::now();
			Room[clients[id].avatar.playroom].minioncreatetimer = high_resolution_clock::now();
			AddTimer(clients[id].avatar.playroom, ROOMFRAME, 100);
			//ok싸인
			for (int i : Room[clients[id].avatar.playroom].clients_list)
			{

				if (i > 4999)
				{
					if (aiplayer[i - 5000].player.team == 0) aiplayer[i - 5000].player.object.SetPosition(1025, 0, 230);
					else {
						aiplayer[i - 5000].player.object.SetPosition(1025, 0, 1801);
					//	aiplayer[i - 5000].player.object.Rotate(0, 180, 0);
					}
				}
				else {
					if (clients[i].avatar.team == 0) { clients[i].avatar.object.SetPosition(1025, 0, 230); }
					else {
						clients[i].avatar.object.SetPosition(1025, 0, 1801);
						clients[i].avatar.object.Rotate(0, 180, 0);
					}
				}
			}
			for (int i : Room[clients[id].avatar.playroom].clients_list)
			{
				if (i > 4999)
					continue;
				for (int j : Room[clients[id].avatar.playroom].clients_list)
				{
					sc_packet_put_player enter_packet;
					if (j > 4999) {
						enter_packet.id = j;
						enter_packet.size = sizeof(enter_packet);
						enter_packet.type = SC_PUT_PLAYER;
						enter_packet.mtx = aiplayer[j - 5000].player.object.m_d3dxmtxLocal;
						enter_packet.team = aiplayer[j - 5000].player.team;
					}
					else {
						enter_packet.id = j;
						enter_packet.size = sizeof(enter_packet);
						enter_packet.type = SC_PUT_PLAYER;
						enter_packet.mtx = clients[j].avatar.object.m_d3dxmtxLocal;
						enter_packet.team = clients[j].avatar.team;
					}
					SendPacket(i, reinterpret_cast<unsigned char *>(&enter_packet));
				}
			}




			for (int i : Room[clients[id].avatar.playroom].clients_list)
			{
				if (i > 4999)
					continue;
				cs_packet_roominfor send_packet;
				send_packet.size = sizeof(cs_packet_roominfor);
				DWORD iobyte;
				send_packet.type = CS_gameplay;
				SendPacket(i, reinterpret_cast<unsigned char *>(&send_packet));
			}

			
			//첫월드샛팅 여기서 ㄱㄱㄱ


		}
	}break;
	case CS_freeveiw: {
		printf("관전 ");
		cs_packet_roominfor *my_packet = reinterpret_cast<cs_packet_roominfor *>(buf);
		//if (Room[my_packet->roomnum].start == true) {
			// ok 싸인 
			
			cs_packet_roominfor send_packet;
			send_packet.size = sizeof(cs_packet_roominfor);
			DWORD iobyte;
			send_packet.type = CS_freeveiw;
			SendPacket(id, reinterpret_cast<unsigned char *>(&send_packet));
		//}
	}break;
	case CS_exitroom: {//방 나가기
		cs_packet_roominfor *my_packet = reinterpret_cast<cs_packet_roominfor *>(buf);
		
		for (int i : Room[my_packet->roomnum].clients_list)
		{
			if (i == id) {
				Room[my_packet->roomnum].clients_list.erase(i);
				Room[my_packet->roomnum].count--;
				if(i>4999)
					D3DXMatrixIdentity(&aiplayer[i].player.object.m_d3dxmtxLocal);
				else {
					D3DXMatrixIdentity(&clients[i].avatar.object.m_d3dxmtxLocal);
					printf("초기화염");
				}
				if (Room[my_packet->roomnum].count == 0)
				{
					Room[my_packet->roomnum].visible = false;
					Room[my_packet->roomnum].start = false;
				}
				clients[id].avatar.team = 0;
				break;
			}
		}
		for (int i : Room[my_packet->roomnum].clients_list)
		{
			if (i > 4999)
				continue;
			cs_packet_roomveiw  send_packet;
			send_packet.size = sizeof(cs_packet_roominfor);
			DWORD iobyte;
			send_packet.type = CS_exitroom;
			send_packet.num = id;
			send_packet.count = Room[my_packet->roomnum].count;
			SendPacket(i, reinterpret_cast<unsigned char *>(&send_packet));
		}
		cs_packet_roomveiw  end_packet;
		end_packet.size = sizeof(cs_packet_roominfor);
		DWORD iobyte;
		end_packet.type = CS_exitroom;
		end_packet.num = id;
		end_packet.count = Room[my_packet->roomnum].count;
		SendPacket(id, reinterpret_cast<unsigned char *>(&end_packet));
	}break;
	case CS_changeteam: {
		cs_packet_roominfor *my_packet = reinterpret_cast<cs_packet_roominfor *>(buf);
		if (my_packet->roomnum == 1)
		{
			clients[id].avatar.team = 0;
		}
		else if (my_packet->roomnum == 2) 
		{
			clients[id].avatar.team = 1;
			
		}
		for (int i : Room[clients[id].avatar.playroom].clients_list)
		{
			if (i > 4999)
				continue;
			cs_packet_changeteam send_packet;
			send_packet.size = sizeof(cs_packet_changeteam);
			DWORD iobyte;
			send_packet.type = CS_changeteam;
			send_packet.id = id;
			send_packet.state = Room[clients[id].avatar.playroom].start;
			send_packet.team= clients[id].avatar.team;
			SendPacket(i, reinterpret_cast<unsigned char *>(&send_packet));
		}


	}break;
	case CS_selectroom: {
		cs_packet_roominfor *my_packet = reinterpret_cast<cs_packet_roominfor *>(buf);
		printf("방생성");
		//플레이어 방이 2개가 넘으면 생성 불허
		if (my_packet->roomnum == -1)//플레이어방생성
		{
			int c = 0;
			if (Room[0].visible == false)
			{
				Room[0].visible = true;
				Room[0].clients_list.insert(id);
				clients[id].avatar.team = 0;
				clients[id].avatar.playroom = 0;
				Room[0].count++;
			}
			else
			{
				if (Room[1].visible == false)
				{
					Room[1].visible = true;
				
					Room[1].clients_list.insert(id);
				
					clients[id].avatar.team= 0;
					clients[id].avatar.playroom = 1;
					Room[1].count++;
					c = 1;
				}
				else {
					break;
				}
			}
			cs_packet_changeteam send_packet;
			send_packet.size = sizeof(cs_packet_changeteam);
			DWORD iobyte;
			send_packet.type = CS_selectroom;
			send_packet.id = id;
			send_packet.team = 0;
			send_packet.roomnum = c;
			send_packet.state = Room[clients[id].avatar.playroom].start;
			SendPacket(id, reinterpret_cast<unsigned char *>(&send_packet));

		}
		else {
			// 그외 한번 방정보 쭉보내야지보내야지
			Room[my_packet->roomnum].clients_list.insert(id);
			clients[id].avatar.playroom = my_packet->roomnum;
			clients[id].avatar.team = 0;
			Room[my_packet->roomnum].count++;
			for (auto i : Room[my_packet->roomnum].clients_list)
			{
				if (i == id)
					continue;
				if (i > 4999)
					continue;
				cs_packet_changeteam send_packet;
				send_packet.size = sizeof(cs_packet_changeteam);
				DWORD iobyte;
				send_packet.type = CS_selectroom;
				send_packet.state = Room[clients[id].avatar.playroom].start;
				send_packet.id = id;
				send_packet.team = clients[id].avatar.team;
				send_packet.roomnum = my_packet->roomnum;
				SendPacket(i, reinterpret_cast<unsigned char *>(&send_packet));
			}
			for (auto i : Room[my_packet->roomnum].clients_list)
			{
				
				cs_packet_changeteam send_packet;
				send_packet.size = sizeof(cs_packet_changeteam);
				DWORD iobyte;
				send_packet.type = CS_selectroom;
				send_packet.id = i;
				if (i > 4999) {
					send_packet.team = aiplayer[i - 5000].player.team;
				}
				else {
					send_packet.team = clients[i].avatar.team;
				}
				send_packet.roomnum = my_packet->roomnum;
				
				send_packet.state = Room[clients[id].avatar.playroom].start;
				SendPacket(id, reinterpret_cast<unsigned char *>(&send_packet));
			}
			
		}
		//패키보내기
		//패킷받는정보  몇번방 선택했는지
		//int index = 0; //방번호 임시
		//Room[index].clients_list.insert[id];
		
	}break;
	case CS_roomnumveiw: {
		for (int i = 0; i < 6; i++) {
			if (Room[i].visible) {
				cs_packet_roomveiw send_packet;
				send_packet.size = sizeof(cs_packet_roomveiw);
				DWORD iobyte;
				send_packet.type = CS_roomnumveiw;
				send_packet.num = i;
				send_packet.count = Room[i].count;
				if (Room[i].start)
				{
					printf("안시작? ");
					auto du = high_resolution_clock::now() - Room[i].timestart;
					auto msec = std::chrono::duration_cast<chrono::seconds >(du);
					send_packet.playtime = msec.count();
					printf("%d ", msec.count());
				}
				else {
					send_packet.playtime = Room[i].playtime;
				}
		
				SendPacket(id, reinterpret_cast<unsigned char *>(&send_packet));
			}
		}
		

	}break;
	case CS_roomcontrol: {
		cs_packet_roominfor *my_packet = reinterpret_cast<cs_packet_roominfor *>(buf);
		for (int i = my_packet->roomnum; i < 	my_packet->roomnum+6; i++) {
			if (Room[i].visible) {
				cs_packet_roomveiw send_packet;
				send_packet.size = sizeof(cs_packet_roomveiw);
				DWORD iobyte;
				send_packet.type = CS_roomcontrol;
				send_packet.num = i;
				send_packet.count = Room[i].count;
				if (Room[i].start)
				{
					auto du = high_resolution_clock::now() - Room[i].timestart;
					auto msec = std::chrono::duration_cast<chrono::seconds >(du);
					send_packet.playtime = msec.count();
				}
				else {
					send_packet.playtime = Room[i].playtime;
				}
				//printf("%d ", send_packet.team);
				SendPacket(id, reinterpret_cast<unsigned char *>(&send_packet));
			}
		}

		

	}break;
	default:// cout << "Unknown type packet received!\n";
		break;
	}
	if (type == MOve && clients[id].avatar.bmovetimer == FALSE) // 한개의 타이머만 사용
	{
		AddTimer(id, EVENT_MOVE, 100);
		clients[id].avatar.bmovetimer =TRUE;
	}
	if (type == ROtation)
	{
		if (buf[1]== CS_LEFTROTATION) {

			clients[id].avatar.object.Rotate(0, -2, 0);
		}
		else if (buf[1] == CS_RIGHTROTATION)
		{
			clients[id].avatar.object.Rotate(0, 2, 0);
		}
		else if (buf[1] == CS_UPROTATION)
		{
			//clients[id].avatar.object.Rotate(1, 0, 0);
		}
		else if (buf[1] == CS_DOWNROTATION)
		{
			//clients[id].avatar.object.Rotate(-1, 0, 0);
		}
		sc_packet_rotation rotate_packet;
		rotate_packet.id = id;
		rotate_packet.size = sizeof(rotate_packet);
		rotate_packet.type = SC_ROTATE;
		rotate_packet.lr = buf[1];
		rotate_packet.team = clients[id].avatar.team;
		SendPacket(id, reinterpret_cast<unsigned char *>(&rotate_packet));
		for (auto i : Room[clients[id].avatar.playroom].clients_list)
		{
			if (i == id)
				continue;
			if (i > 4999)
				continue;
			SendPacket(i, reinterpret_cast<unsigned char *>(&rotate_packet));
		}
		
	}

}
	

	


void collision(int key)
{
	int check = 0;
	
	for (int i = 0; i < 15; i++)
	{

		if (clients[key].avatar.bullet[i].active) {
			check++;
			clients[key].avatar.bullet[i].count++;
			if (clients[key].avatar.bullet[i].count > 200)// 총알 사거리  40 * 30 = 1200
			{
				clients[key].avatar.bullet[i].active = false;
				clients[key].avatar.bullet[i].count = 0;
				cs_packet_bullet bullet_packet;
				bullet_packet.id = key;
				bullet_packet.index = i;
				bullet_packet.size = sizeof(cs_packet_bullet);
				bullet_packet.type = SC_bullet;
				bullet_packet.team = clients[key].avatar.team;
				bullet_packet.x = 4419;
				bullet_packet.y = 6000;

				SendPacket(key, reinterpret_cast<unsigned char *>(&bullet_packet));

				for (auto i : Room[clients[key].avatar.playroom].clients_list)
				{
					if (i == key)
						continue;
					if (i > 4999)
						continue;
					SendPacket(i, reinterpret_cast<unsigned char *>(&bullet_packet));
				}
			}
			else {
				clients[key].avatar.bullet[i].position.x += clients[key].avatar.bullet[i].direction.x * 20;
				clients[key].avatar.bullet[i].position.y += clients[key].avatar.bullet[i].direction.y * 20;
				clients[key].avatar.bullet[i].position.z += clients[key].avatar.bullet[i].direction.z * 20;
				cs_packet_bullet bullet_packet;
				bullet_packet.id = key;
				bullet_packet.index = i;
				bullet_packet.size = sizeof(cs_packet_bullet);
				bullet_packet.type = SC_bullet;
				bullet_packet.x = clients[key].avatar.bullet[i].position.x;
				bullet_packet.y = clients[key].avatar.bullet[i].position.y;
				bullet_packet.z = clients[key].avatar.bullet[i].position.z;
				bullet_packet.team = clients[key].avatar.team;
				SendPacket(key, reinterpret_cast<unsigned char *>(&bullet_packet));

				for (auto i : Room[clients[key].avatar.playroom].clients_list)
				{
					if (i == key)
						continue;
					if (i > 4999)
						continue;
					SendPacket(i, reinterpret_cast<unsigned char *>(&bullet_packet));
				}

			}

		}
		for (int j : Room[clients[key].avatar.playroom].clients_list)
		{
			if (key == j)
				continue;
			if ((clients[key].avatar.bullet[i].position.x > clients[j].avatar.object.GetMainboundingbox(0).x) && (clients[key].avatar.bullet[i].position.x < clients[j].avatar.object.GetMainboundingbox(1).x)  && (clients[key].avatar.bullet[i].position.z > clients[j].avatar.object.GetMainboundingbox(0).z) && (clients[key].avatar.bullet[i].position.z < clients[j].avatar.object.GetMainboundingbox(1).z) &&(clients[key].avatar.bullet[i].position.y > clients[j].avatar.object.GetMainboundingbox(0).y+390) && (clients[key].avatar.bullet[i].position.y < clients[j].avatar.object.GetMainboundingbox(1).y+390))
			{
				printf("충돌했스미다\n");
				if ((((clients[key].avatar.bullet[i].position.x > clients[j].avatar.object.GetBoundingCubeBody(0).x) && (clients[key].avatar.bullet[i].position.x < clients[j].avatar.object.GetBoundingCubeBody(1).x))  || ((clients[key].avatar.bullet[i].position.z > clients[j].avatar.object.GetBoundingCubeBody(0).z) && (clients[key].avatar.bullet[i].position.z < clients[j].avatar.object.GetBoundingCubeBody(1).z)))&& ((clients[key].avatar.bullet[i].position.y > clients[j].avatar.object.GetBoundingCubeBody(0).y + 390) && (clients[key].avatar.bullet[i].position.y < clients[j].avatar.object.GetBoundingCubeBody(1).y + 390)))
				{
					printf("몸통\n");
					sc_packet_collision bullet_packet;
					bullet_packet.id = i;
					bullet_packet.index = j;
					bullet_packet.size = sizeof(cs_packet_bullet);
					bullet_packet.type = CS_COLL;
					bullet_packet.state = 0;
					SendPacket(key, reinterpret_cast<unsigned char *>(&bullet_packet));
					break;;
				}
				else if ((((clients[key].avatar.bullet[i].position.x > clients[j].avatar.object.GetBoundingCubeHead(0).x) && (clients[key].avatar.bullet[i].position.x < clients[j].avatar.object.GetBoundingCubeHead(1).x)) || ((clients[key].avatar.bullet[i].position.z > clients[j].avatar.object.GetBoundingCubeHead(0).z) && (clients[key].avatar.bullet[i].position.z < clients[j].avatar.object.GetBoundingCubeHead(1).z))) &&  ((clients[key].avatar.bullet[i].position.y > clients[j].avatar.object.GetBoundingCubeHead(0).y + 390) && (clients[key].avatar.bullet[i].position.y < clients[j].avatar.object.GetBoundingCubeHead(1).y + 390)))
				{
					printf("뚝배기\n");
					//뚝배기
					sc_packet_collision bullet_packet;
					bullet_packet.id = i;
					bullet_packet.index = j;
					bullet_packet.size = sizeof(cs_packet_bullet);
					bullet_packet.type = CS_COLL;
					bullet_packet.state = 1;
					SendPacket(key, reinterpret_cast<unsigned char *>(&bullet_packet));
				}
				else {
					sc_packet_collision bullet_packet;
					bullet_packet.id = i;
					bullet_packet.index = j;
					bullet_packet.size = sizeof(cs_packet_bullet);
					bullet_packet.type = CS_COLL;
					bullet_packet.state = 3;
					SendPacket(key, reinterpret_cast<unsigned char *>(&bullet_packet));
				}
				/*
				if ((((clients[key].avatar.bullet[i].position.x > clients[j].avatar.object.GetBoundingCubeLhand(0).x) && (clients[key].avatar.bullet[i].position.x < clients[j].avatar.object.GetBoundingCubeLhand(1).x)) || ((clients[key].avatar.bullet[i].position.z > clients[j].avatar.object.GetBoundingCubeLhand(0).z) && (clients[key].avatar.bullet[i].position.z < clients[j].avatar.object.GetBoundingCubeLhand(1).z))) && ((clients[key].avatar.bullet[i].position.y > clients[j].avatar.object.GetBoundingCubeLhand(0).y + 390) && (clients[key].avatar.bullet[i].position.y < clients[j].avatar.object.GetBoundingCubeLhand(1).y + 390)))
				{
					printf("왼손\n");
					//왼손
					
				}
				if ((((clients[key].avatar.bullet[i].position.x > clients[j].avatar.object.GetBoundingCubeRhand(0).x) && (clients[key].avatar.bullet[i].position.x < clients[j].avatar.object.GetBoundingCubeRhand(1).x)) && ((clients[key].avatar.bullet[i].position.z > clients[j].avatar.object.GetBoundingCubeRhand(0).z) && (clients[key].avatar.bullet[i].position.z < clients[j].avatar.object.GetBoundingCubeRhand(1).z)))&& ((clients[key].avatar.bullet[i].position.y > clients[j].avatar.object.GetBoundingCubeRhand(0).y + 390) && (clients[key].avatar.bullet[i].position.y < clients[j].avatar.object.GetBoundingCubeRhand(1).y + 390)))
				{
					printf("오른손\n");
					//오손
				
				}
				if ((((clients[key].avatar.bullet[i].position.x > clients[j].avatar.object.GetBoundingCubeLleg(0).x) && (clients[key].avatar.bullet[i].position.x < clients[j].avatar.object.GetBoundingCubeLleg(1).x)) && ((clients[key].avatar.bullet[i].position.z > clients[j].avatar.object.GetBoundingCubeLleg(0).z) && (clients[key].avatar.bullet[i].position.z < clients[j].avatar.object.GetBoundingCubeLleg(1).z))) && ((clients[key].avatar.bullet[i].position.y > clients[j].avatar.object.GetBoundingCubeLleg(0).y + 390) && (clients[key].avatar.bullet[i].position.y < clients[j].avatar.object.GetBoundingCubeLleg(1).y + 390)))
				{
					printf("왼쪽다리\n");
					//왼다
			
				}
				if ((((clients[key].avatar.bullet[i].position.x > clients[j].avatar.object.GetBoundingCubeRleg(0).x) && (clients[key].avatar.bullet[i].position.x < clients[j].avatar.object.GetBoundingCubeRleg(1).x)) && ((clients[key].avatar.bullet[i].position.z > clients[j].avatar.object.GetBoundingCubeRleg(0).z) && (clients[key].avatar.bullet[i].position.z < clients[j].avatar.object.GetBoundingCubeRleg(1).z))) && ((clients[key].avatar.bullet[i].position.y > clients[j].avatar.object.GetBoundingCubeRleg(0).y + 390) && (clients[key].avatar.bullet[i].position.y < clients[j].avatar.object.GetBoundingCubeRleg(1).y + 390)))
				{
					printf("오른쪽다리\n");
					//오다
				}
				else {
					printf("그외");
				}
				*/
			}
		}

	}
	
				/*
					//플레이어간 1차 충돌처리
					if ((clients[key].avatar.bullet[temp].position.x >(clients[j].avatar.object.m_d3dxmtxLocal._41 - clients[j].avatar.object.m_bcBoundingCubeBody.Min.x)) && (clients[key].avatar.bullet[temp].position.x < (clients[j].avatar.object.m_d3dxmtxLocal._41 + clients[j].avatar.object.m_bcBoundingCubeBody.Max.x)) && (clients[key].avatar.bullet[temp].position.z >(clients[j].avatar.object.m_d3dxmtxLocal._43 - clients[j].avatar.object.m_bcBoundingCubeBody.Min.z)) && (clients[key].avatar.bullet[temp].position.z < (clients[j].avatar.object.m_d3dxmtxLocal._43 + clients[j].avatar.object.m_bcBoundingCubeBody.Max.z)) && (clients[key].avatar.bullet[temp].position.y >(clients[j].avatar.object.m_d3dxmtxLocal._42 - clients[j].avatar.object.m_bcBoundingCubeBody.Min.y)) && (clients[key].avatar.bullet[temp].position.y < (clients[j].avatar.object.m_d3dxmtxLocal._42 + clients[j].avatar.object.m_bcBoundingCubeBody.Max.y)))
					{
						//몸통
						continue;
					}
					if ((clients[key].avatar.bullet[temp].position.x >(clients[j].avatar.object.m_d3dxmtxLocal._41 - clients[j].avatar.object.m_bcBoundingCubeLhand.Min.x)) && (clients[key].avatar.bullet[temp].position.x < (clients[j].avatar.object.m_d3dxmtxLocal._41 + clients[j].avatar.object.m_bcBoundingCubeLhand.Max.x)) && (clients[key].avatar.bullet[temp].position.z >(clients[j].avatar.object.m_d3dxmtxLocal._43 - clients[j].avatar.object.m_bcBoundingCubeLhand.Min.z)) && (clients[key].avatar.bullet[temp].position.z < (clients[j].avatar.object.m_d3dxmtxLocal._43 + clients[j].avatar.object.m_bcBoundingCubeLhand.Max.z)) && (clients[key].avatar.bullet[temp].position.y >(clients[j].avatar.object.m_d3dxmtxLocal._42 - clients[j].avatar.object.m_bcBoundingCubeLhand.Min.y)) && (clients[key].avatar.bullet[temp].position.y < (clients[j].avatar.object.m_d3dxmtxLocal._42 + clients[j].avatar.object.m_bcBoundingCubeLhand.Max.y)))
					{
						//왼손444444444
						continue;
					}
					if ((clients[key].avatar.bullet[temp].position.x >(clients[j].avatar.object.m_d3dxmtxLocal._41 - clients[j].avatar.object.m_bcBoundingCubeRhand.Min.x)) && (clients[key].avatar.bullet[temp].position.x < (clients[j].avatar.object.m_d3dxmtxLocal._41 + clients[j].avatar.object.m_bcBoundingCubeRhand.Max.x)) && (clients[key].avatar.bullet[temp].position.z >(clients[j].avatar.object.m_d3dxmtxLocal._43 - clients[j].avatar.object.m_bcBoundingCubeRhand.Min.z)) && (clients[key].avatar.bullet[temp].position.z < (clients[j].avatar.object.m_d3dxmtxLocal._43 + clients[j].avatar.object.m_bcBoundingCubeRhand.Max.z)) && (clients[key].avatar.bullet[temp].position.y >(clients[j].avatar.object.m_d3dxmtxLocal._42 - clients[j].avatar.object.m_bcBoundingCubeRhand.Min.y)) && (clients[key].avatar.bullet[temp].position.y < (clients[j].avatar.object.m_d3dxmtxLocal._42 + clients[j].avatar.object.m_bcBoundingCubeRhand.Max.y)))
					{
						//오른손
						continue;
					}
					if ((clients[key].avatar.bullet[temp].position.x >(clients[j].avatar.object.m_d3dxmtxLocal._41 - clients[j].avatar.object.m_bcBoundingCubeLleg.Min.x)) && (clients[key].avatar.bullet[temp].position.x < (clients[j].avatar.object.m_d3dxmtxLocal._41 + clients[j].avatar.object.m_bcBoundingCubeLleg.Max.x)) && (clients[key].avatar.bullet[temp].position.z >(clients[j].avatar.object.m_d3dxmtxLocal._43 - clients[j].avatar.object.m_bcBoundingCubeLleg.Min.z)) && (clients[key].avatar.bullet[temp].position.z < (clients[j].avatar.object.m_d3dxmtxLocal._43 + clients[j].avatar.object.m_bcBoundingCubeLleg.Max.z)) && (clients[key].avatar.bullet[temp].position.y >(clients[j].avatar.object.m_d3dxmtxLocal._42 - clients[j].avatar.object.m_bcBoundingCubeLleg.Min.y)) && (clients[key].avatar.bullet[temp].position.y < (clients[j].avatar.object.m_d3dxmtxLocal._42 + clients[j].avatar.object.m_bcBoundingCubeLleg.Max.y)))
					{
						//왼다
						continue;
					}
					if ((clients[key].avatar.bullet[temp].position.x >(clients[j].avatar.object.m_d3dxmtxLocal._41 - clients[j].avatar.object.m_bcBoundingCubeRleg.Min.x)) && (clients[key].avatar.bullet[temp].position.x < (clients[j].avatar.object.m_d3dxmtxLocal._41 + clients[j].avatar.object.m_bcBoundingCubeRleg.Max.x)) && (clients[key].avatar.bullet[temp].position.z >(clients[j].avatar.object.m_d3dxmtxLocal._43 - clients[j].avatar.object.m_bcBoundingCubeRleg.Min.z)) && (clients[key].avatar.bullet[temp].position.z < (clients[j].avatar.object.m_d3dxmtxLocal._43 + clients[j].avatar.object.m_bcBoundingCubeRleg.Max.z)) && (clients[key].avatar.bullet[temp].position.y >(clients[j].avatar.object.m_d3dxmtxLocal._42 - clients[j].avatar.object.m_bcBoundingCubeRleg.Min.y)) && (clients[key].avatar.bullet[temp].position.y < (clients[j].avatar.object.m_d3dxmtxLocal._42 + clients[j].avatar.object.m_bcBoundingCubeRleg.Max.y)))
					{
						//오른다
						continue;
					}
					if ((clients[key].avatar.bullet[temp].position.x >(clients[j].avatar.object.m_d3dxmtxLocal._41 - clients[j].avatar.object.m_bcBoundingCubeHead.Min.x)) && (clients[key].avatar.bullet[temp].position.x < (clients[j].avatar.object.m_d3dxmtxLocal._41 + clients[j].avatar.object.m_bcBoundingCubeHead.Max.x)) && (clients[key].avatar.bullet[temp].position.z >(clients[j].avatar.object.m_d3dxmtxLocal._43 - clients[j].avatar.object.m_bcBoundingCubeHead.Min.z)) && (clients[key].avatar.bullet[temp].position.z < (clients[j].avatar.object.m_d3dxmtxLocal._43 + clients[j].avatar.object.m_bcBoundingCubeHead.Max.z)) && (clients[key].avatar.bullet[temp].position.y >(clients[j].avatar.object.m_d3dxmtxLocal._42 - clients[j].avatar.object.m_bcBoundingCubeHead.Min.y)) && (clients[key].avatar.bullet[temp].position.y < (clients[j].avatar.object.m_d3dxmtxLocal._42 + clients[j].avatar.object.m_bcBoundingCubeHead.Max.y)))
					{
						//뚝배기
						continue;
					}
				}
			}


		}*/
	


		
	if (check == 0)
		clients[key].avatar.bulletalive = false;

}


void AcceptThreadStart()
{
	struct sockaddr_in listen_addr;

	SOCKET accept_socket = WSASocket(AF_INET, SOCK_STREAM,
		IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	memset(&listen_addr, 0, sizeof(listen_addr));
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = htonl(ADDR_ANY);
	listen_addr.sin_port = htons(MY_SERVER_PORT);
	// listen_addr.sin_zero = 0;
	::bind(accept_socket,
		reinterpret_cast<sockaddr *>(&listen_addr), sizeof(listen_addr));
	listen(accept_socket, 10);
	// listen
	while (false == g_isshutdown)
	{
		struct sockaddr_in client_addr;
		int addr_size = sizeof(client_addr);
		new_client = WSAAccept(accept_socket,
			reinterpret_cast<sockaddr *>(&client_addr), &addr_size,
			NULL, NULL);

		if (INVALID_SOCKET == new_client) {
			int error_no = WSAGetLastError();
			error_display("Accept::WSAAccept", error_no);
			while (true);
		}

		new_id = -1;
		for (auto i = 0; i < MAX_USER; ++i) {
			if (false == clients[i].is_connected) {
				new_id = i;
				clients[new_id].is_connected = true;
				break;
			}
		}
		if (-1 == new_id) {
			cout << "Max Concurrent User excceded!\n";
			closesocket(new_client);
			continue;
		}
	
		memset(&clients[new_id].recv_overlap.original_overlap, 0,
			sizeof(clients[new_id].recv_overlap.original_overlap));

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(new_client),
			g_hIocp, new_id, 0);
	
		firstputplayer();

		DWORD flags = 0;
		int ret = WSARecv(new_client, &clients[new_id].recv_overlap.wsabuf, 1, NULL,
			&flags, &clients[new_id].recv_overlap.original_overlap, NULL);
		if (0 != ret)
		{
			int error_no = WSAGetLastError();
			if (WSA_IO_PENDING != error_no)
				error_display("Accept:WSARecv", error_no);
		}
		
		if (lobbytimer == false && new_id > 200)
		{
			AddTimer(0, LOBBY, 5000);
			lobbytimer = true;
		}
		
		printf("오 크들이 온다 \n");
		
	}
}

void Cleanup()
{
	WSACleanup();
}

void moveheart_beat(int id)
{
	if(clients[id].avatar.direction)

	switch (clients[id].avatar.direction)
	{
	case CS_UP:
		clients[id].avatar.object.MoveForward(5.0f);
		break;
	case CS_DOWN:
		clients[id].avatar.object.MoveForward(-5.0f);
		break;
	case CS_LEFT:
		clients[id].avatar.object.MoveStrafe(-3.0f);
		break;
	case CS_RIGHT:
		clients[id].avatar.object.MoveStrafe(+3.0f);
		break;
	case 5:
		clients[id].avatar.object.MoveForward(5.0f);
		clients[id].avatar.object.MoveStrafe(-3.0f);
		break;
	case 9:
		clients[id].avatar.object.MoveStrafe(+3.0f);
		clients[id].avatar.object.MoveForward(5.0f);
		break;
	case 6:
		clients[id].avatar.object.MoveForward(-5.0f);
		clients[id].avatar.object.MoveStrafe(-3.0f);
		break;
	case 10:
		clients[id].avatar.object.MoveForward(-5.0f);
		clients[id].avatar.object.MoveStrafe(+3.0f);
		break;
	default:
		break;
	}
	if (clients[id].avatar.object.m_d3dxmtxLocal._41 < 0)
		clients[id].avatar.object.m_d3dxmtxLocal._41 = 0;
	else if (clients[id].avatar.object.m_d3dxmtxLocal._41 > 2040)
		clients[id].avatar.object.m_d3dxmtxLocal._41 = 2040;
	else if (clients[id].avatar.object.m_d3dxmtxLocal._43 <0)
		clients[id].avatar.object.m_d3dxmtxLocal._43 = 0;
	else if (clients[id].avatar.object.m_d3dxmtxLocal._43 > 2040)
		clients[id].avatar.object.m_d3dxmtxLocal._43 = 2040;
	else {
		sc_packet_pos mov_packet;
		mov_packet.id = id;
		mov_packet.size = sizeof(mov_packet);
		mov_packet.type = SC_POS;
		mov_packet.team = clients[id].avatar.team;
		mov_packet.x = clients[id].avatar.x;
		mov_packet.z = clients[id].avatar.z;
		mov_packet.mtx = clients[id].avatar.object.m_d3dxmtxLocal;
		SendPacket(id, reinterpret_cast<unsigned char *>(&mov_packet));

		for (auto i : Room[clients[id].avatar.playroom].clients_list)
		{
			if (i == id)
				continue;
			if (i > 4999)
				continue;
			SendPacket(i, reinterpret_cast<unsigned char *>(&mov_packet));
		}
	}
}

void Process_Event(event_type curr_event)
{
	switch (curr_event.do_event) {
	case MOVE:
	{
		Overlap_ex *event_over = new Overlap_ex;
		event_over->operation = MOVE;
		PostQueuedCompletionStatus(g_hIocp, 1, curr_event.obj_id,
			reinterpret_cast<LPOVERLAPPED>(event_over));
		break;
	}
	case ROtation:
	{
		Overlap_ex *event_over = new Overlap_ex;
		event_over->operation = ROtation;
		PostQueuedCompletionStatus(g_hIocp, 1, curr_event.obj_id,
			reinterpret_cast<LPOVERLAPPED>(event_over));
		break;
	}
	case ATTACK:
	{
		Overlap_ex *event_over = new Overlap_ex;
		event_over->operation = ATTACK;
		PostQueuedCompletionStatus(g_hIocp, 1, curr_event.obj_id,
			reinterpret_cast<LPOVERLAPPED>(event_over));
		break;
	}
	case LOBBY:
	{
		Overlap_ex *event_over = new Overlap_ex;
		event_over->operation = LOBBY;
		PostQueuedCompletionStatus(g_hIocp, 1, curr_event.obj_id,
			reinterpret_cast<LPOVERLAPPED>(event_over));
		break;
	}
	case ROOMFRAME:
	{
		Overlap_ex *event_over = new Overlap_ex;
		event_over->operation = ROOMFRAME;
		PostQueuedCompletionStatus(g_hIocp, 1, curr_event.obj_id,
			reinterpret_cast<LPOVERLAPPED>(event_over));
		break;
	}
	default:
		break;
	}
}

void Timer_Thread()
{

	while (true) {
		Sleep(1);
		tq_lock.lock();
		if (timer_queue.size() == 0) {
			tq_lock.unlock();
			continue;
		}
		event_type top_event = timer_queue.top();
		while (top_event.starttime <= GetTickCount()) {
			timer_queue.pop();
			tq_lock.unlock();
			Process_Event(top_event);
			tq_lock.lock();
			if (timer_queue.size() == 0) {
				goto loop_end;
			}
			top_event = timer_queue.top();
		}
	loop_end:
		tq_lock.unlock();
	}
}


void lobby()
{
	if (Room[startroom].visible) {
		if (!Room[startroom].start)
		{
			Room[startroom].start = true;
			Room[startroom].timestart=high_resolution_clock::now();
			Room[startroom].minioncreatetimer = high_resolution_clock::now();
			AddTimer(startroom, ROOMFRAME, 100);
		}
		startroom++;
	}
	else
		lobbytimer = false;
}

void roomrate(int key)
{
	auto now = high_resolution_clock::now() - Room[key].minioncreatetimer;
	auto msec = std::chrono::duration_cast<chrono::milliseconds>(now);

	if (msec.count() > 5000 ) // 미니언 6마리 스폰  코드
	{
		Room[key].minioncreatetimer = high_resolution_clock::now();
		for (int i = Room[key].minionindex; i <	Room[key].minionindex+2; i++)
		{
			if (Room[key].minionindex == 50) Room[key].minionindex = 0;
			if (!Room[key].redminion[Room[key].minionindex].is_alive)
			{
				Room[key].redminion[Room[key].minionindex].is_alive = true;
				Room[key].redminion[Room[key].minionindex].team = 0;// 레드팀
				if (i % 2 == 0)
					Room[key].redminion[Room[key].minionindex].line = 0;//왼쪽라인미니언
				else
					Room[key].redminion[Room[key].minionindex].line = 1;//오른쪽라인미니언
				Room[key].redminion[Room[key].minionindex].object.m_d3dxmtxLocal._41 = 1025;
				Room[key].redminion[Room[key].minionindex].object.m_d3dxmtxLocal._43 = 230;
				Room[key].redminion[Room[key].minionindex].object.Rotate(0.0f, 180.0f, 0.0f);

				sc_packet_minionpos send_packet;
				send_packet.id = Room[key].minionindex;
				send_packet.size = sizeof(send_packet);
				send_packet.type = SC_MINIONPOS;
				send_packet.team = 0;
				send_packet.mtx = Room[key].redminion[Room[key].minionindex].object.m_d3dxmtxLocal;

				for (auto i : Room[key].clients_list)
				{
					if (i > 4999)
						continue;
					SendPacket(i, reinterpret_cast<unsigned char *>(&send_packet));
				}
			}
		}
		for (int i = Room[key].minionindex; i < Room[key].minionindex + 2; i++)
		{
			if (!Room[key].blueminion[Room[key].minionindex].is_alive)
			{
				Room[key].blueminion[Room[key].minionindex].is_alive = true;
				Room[key].blueminion[Room[key].minionindex].team = 1;//블루팀
				if (i%2==0)
					Room[key].blueminion[Room[key].minionindex].line = 0;//왼쪽라인미니언
				else
					Room[key].blueminion[Room[key].minionindex].line = 1;//오른쪽라인미니언

				Room[key].blueminion[Room[key].minionindex].object.m_d3dxmtxLocal._41 = 1025;
				Room[key].blueminion[Room[key].minionindex].object.m_d3dxmtxLocal._43 = 1810;

				sc_packet_minionpos send_packet;
				send_packet.id = Room[key].minionindex;
				send_packet.size = sizeof(send_packet);
				send_packet.type = SC_MINIONPOS;
				send_packet.team = 1;
				send_packet.mtx = Room[key].blueminion[Room[key].minionindex].object.m_d3dxmtxLocal;
				for (auto i : Room[key].clients_list)
				{
					if (i > 4999)
						continue;
					SendPacket(i, reinterpret_cast<unsigned char *>(&send_packet));
				}

			}
		}
		Room[key].minionindex += 2;
	}
	
	for (int i = 0; i < 50; ++i)
	{
		if (Room[key].redminion[i].is_alive)
		{
			static string route;
			static int xA = Room[key].redminion[i].object.m_d3dxmtxLocal._41;
			static int yA = Room[key].redminion[i].object.m_d3dxmtxLocal._43;
			indextonode(xA, yA);

			static int xB, yB;
			if (0 == Room[key].redminion[i].line)
			{
				if (xA == 3 && yA == 5) Room[key].redminion[i].flagline = 1;
				if (xA == 3 && yA == 14) Room[key].redminion[i].flagline = 2;
				if (0 == Room[key].redminion[i].flagline && route.length() == 0)
				{
					xB = 300, yB = 500;
					indextonode(xB, yB);
					route = pathFind(xA, yA, xB, yB);
				}
				else if (1 == Room[key].redminion[i].flagline && route.length() == 0)
				{
					xB = 300, yB = 1400;
					indextonode(xB, yB);
					route = pathFind(xA, yA, xB, yB);
				}
				else if (2 == Room[key].redminion[i].flagline && route.length() == 0)
				{
					xB = 1000, yB = 1800;
					indextonode(xB, yB);
					route = pathFind(xA, yA, xB, yB);
				}
			}
			else if (1 == Room[key].redminion[i].line)
			{
				if (xA == 17 && yA == 5) Room[key].redminion[i].flagline = 1;
				if (xA == 17 && yA == 14) Room[key].redminion[i].flagline = 2;
				if (0 == Room[key].redminion[i].flagline && route.length() == 0)
				{
					xB = 1700, yB = 500;
					indextonode(xB, yB);
					route = pathFind(xA, yA, xB, yB);
				}
				else if (1 == Room[key].redminion[i].flagline && route.length() == 0)
				{
					xB = 1700, yB = 1400;
					indextonode(xB, yB);
					route = pathFind(xA, yA, xB, yB);
				}
				else if (2 == Room[key].redminion[i].flagline && route.length() == 0)
				{
					xB = 1000, yB = 1800;
					indextonode(xB, yB);
					route = pathFind(xA, yA, xB, yB);
				}
			}
			if (route.length() > 0)
			{
				int j; char c;
				c = route.at(0);
				j = atoi(&c);
				route.erase(0, 1);

				if (0 == j) Room[key].redminion[i].object.m_d3dxmtxLocal._41 += 1;
				if (1 == j) Room[key].redminion[i].object.m_d3dxmtxLocal._43 += 1;
				if (2 == j) Room[key].redminion[i].object.m_d3dxmtxLocal._41 -= 1;
				if (3 == j) Room[key].redminion[i].object.m_d3dxmtxLocal._43 -= 1;
			}

			/*
			for (auto j : Room[key].clients_list)
			{
				if (j > 4999)	continue;
				if (1 == clients[j].avatar.team)
				{
					if (abs(Room[key].redminion[i].object.GetPosition().x - clients[j].avatar.object.GetPosition().x) < 300)
					{
						if (abs(Room[key].redminion[i].object.GetPosition().z - clients[j].avatar.object.GetPosition().z) < 300)
						{
							xB = clients[j].avatar.object.GetPosition().x;
							yB = clients[j].avatar.object.GetPosition().z;
							indextonode(xB, yB);
						}
					}
				}
			}
			*/
			//string route = pathFind(xA, yA, xB, yB);
			//if (route.length() > 0)
			//{
			//	int j; char c;
			//	c = route.at(0);
			//	j = atoi(&c);

			//	if (0 == j) Room[key].redminion[i].object.m_d3dxmtxLocal._41 += 1;
			//	if (1 == j) Room[key].redminion[i].object.m_d3dxmtxLocal._43 += 1;
			//	if (2 == j) Room[key].redminion[i].object.m_d3dxmtxLocal._41 -= 1;
			//	if (3 == j) Room[key].redminion[i].object.m_d3dxmtxLocal._43 -= 1;
		//	}
			sc_packet_minionpos send_packet;
			send_packet.id = i;
			send_packet.size = sizeof(send_packet);
			send_packet.type = SC_MINIONPOS;
			send_packet.team = 0;
			send_packet.mtx = Room[key].redminion[i].object.m_d3dxmtxLocal;
			for (auto i : Room[key].clients_list)
			{
				if (i > 4999)
					continue;
				SendPacket(i, reinterpret_cast<unsigned char *>(&send_packet));
			}
		}
	}
	/*
	for (int i = 0; i < 100; ++i)
	{
		if (Room[key].blueminion[i].is_alive)
		{
			int xA = Room[key].blueminion[i].object.m_d3dxmtxLocal._41;
			int yA = Room[key].blueminion[i].object.m_d3dxmtxLocal._43;
			indextonode(xA, yA);

			int xB, yB;
			if (0 == Room[key].blueminion[i].line)
			{
				if (xA == 3 && yA == 14) Room[key].blueminion[i].flagline = 1;
				if (xA == 3 && yA == 5) Room[key].blueminion[i].flagline = 2;
				if (0 == Room[key].blueminion[i].flagline)
				{
					xB = 300, yB = 1400;
					indextonode(xB, yB);
				}
				else if (1 == Room[key].blueminion[i].flagline)
				{
					xB = 300, yB = 500;
					indextonode(xB, yB);
				}
				else if (2 == Room[key].blueminion[i].flagline)
				{
					xB = 1000, yB = 200;
					indextonode(xB, yB);
				}
			}
			else if (1 == Room[key].blueminion[i].line)
			{
				if (xA == 17 && yA == 14) Room[key].blueminion[i].flagline = 1;
				if (xA == 17 && yA == 5) Room[key].blueminion[i].flagline = 2;
				if (0 == Room[key].blueminion[i].flagline)
				{
					xB = 1700, yB = 1400;
					indextonode(xB, yB);
				}
				else if (1 == Room[key].blueminion[i].flagline)
				{
					xB = 1700, yB = 500;
					indextonode(xB, yB);
				}
				else if (2 == Room[key].blueminion[i].flagline)
				{
					xB = 1000, yB = 200;
					indextonode(xB, yB);
				}
			}
			/*
			for (auto j : Room[key].clients_list)
			{
				if (j > 4999)	continue;
				if (0 == clients[j].avatar.team)
				{
					if (abs(Room[key].blueminion[i].object.GetPosition().x - clients[j].avatar.object.GetPosition().x) < 300)
					{
						if (abs(Room[key].blueminion[i].object.GetPosition().z - clients[j].avatar.object.GetPosition().z) < 300)
						{
							xB = clients[j].avatar.object.GetPosition().x;
							yB = clients[j].avatar.object.GetPosition().z;
							indextonode(xB, yB);
						}
					}
				}
			}
			
			string route = pathFind(xA, yA, xB, yB);
			if (route.length() > 0)
			{
				int j; char c;
				c = route.at(0);
				j = atoi(&c);

				if (0 == j) Room[key].blueminion[i].object.m_d3dxmtxLocal._41 += 1;
				if (1 == j) Room[key].blueminion[i].object.m_d3dxmtxLocal._43 += 1;
				if (2 == j) Room[key].blueminion[i].object.m_d3dxmtxLocal._41 -= 1;
				if (3 == j) Room[key].blueminion[i].object.m_d3dxmtxLocal._43 -= 1;
			}
			sc_packet_minionpos send_packet;
			send_packet.id = i;
			send_packet.size = sizeof(send_packet);
			send_packet.type = SC_MINIONPOS;
			send_packet.team = 1;
			send_packet.mtx = Room[key].blueminion[i].object.m_d3dxmtxLocal;
			for (auto i : Room[key].clients_list)
			{
				if (i > 4999)
					continue;
				SendPacket(i, reinterpret_cast<unsigned char *>(&send_packet));
			}
		}
	}*/
}
void WorkerThreadStart()
{

	while (false == g_isshutdown)
	{
		DWORD iosize;
		DWORD key;
		Overlap_ex *my_overlap;
		BOOL result = GetQueuedCompletionStatus(g_hIocp,
			&iosize, &key, reinterpret_cast<LPOVERLAPPED *>(&my_overlap), INFINITE);
		if (FALSE == result) {
			// 에러 처리
		}
		if (0 == iosize) {
			//sc_packet_remove_player rem_player;
			//rem_player.id = key;
			//rem_player.size = sizeof(rem_player);
			//rem_player.type = SC_REMOVE_PLAYER;
			
		//	for (int i : clients[key].view_list) {
		//		if (clients[i].is_connected == false) continue;
		//		SendPacket(i, reinterpret_cast<unsigned char*>(&rem_player));
		//	}
			clients[key].is_connected = false;
		
		
			for (int i : Room[clients[key].avatar.playroom].clients_list)
			{
				if (i == key) {
					Room[clients[key].avatar.playroom].clients_list.erase(i);
					Room[clients[key].avatar.playroom].count--;
					if (Room[clients[key].avatar.playroom].count == 0)
					{
						Room[clients[key].avatar.playroom].visible = false;
						Room[clients[key].avatar.playroom].start = false;
					}
					clients[key].avatar.team = 0;
					D3DXMatrixIdentity(&clients[key].avatar.object.m_d3dxmtxLocal);
					break;
				}
			}
			
			closesocket(clients[key].s);
			continue;
		}

		if (OP_RECV == my_overlap->operation) {
			unsigned char *buf_ptr = clients[key].recv_overlap.iocp_buffer;
			int remained = iosize;
			while (0 < remained) {
				if (0 == clients[key].packet_size)
					clients[key].packet_size = buf_ptr[0];
				int required = clients[key].packet_size
					- clients[key].previous_size;
				if (remained >= required) {
					memcpy(
						clients[key].packet_buff + clients[key].previous_size,
						buf_ptr, required);
					ProcessPacket(key, clients[key].packet_buff);
					buf_ptr += required;
					remained -= required;
					clients[key].packet_size = 0;
					clients[key].previous_size = 0;
				}
				else {
					memcpy(clients[key].packet_buff
						+ clients[key].previous_size,
						buf_ptr, remained);
					buf_ptr += remained;
					clients[key].previous_size += remained;
					remained = 0;
				}
			}
			DWORD flags = 0;
			WSARecv(clients[key].s,
				&clients[key].recv_overlap.wsabuf, 1, NULL, &flags,
				&clients[key].recv_overlap.original_overlap, NULL);
		}
		else if (OP_SEND == my_overlap->operation) {
			delete my_overlap;
		}
		else if (MOVE == my_overlap->operation) {
			if (clients[key].avatar.direction != 0 && clients[key].avatar.bmovetimer==true) {
				moveheart_beat(key);
				AddTimer(key, EVENT_MOVE, 10);
			}
			else {
				clients[key].avatar.bmovetimer =false;
				
			}
			delete my_overlap;
		}
		
		else if (LOBBY == my_overlap->operation) {
			if (lobbytimer)
			{
				lobby();
				AddTimer(0, LOBBY, 5000);
			}
			delete my_overlap;
		}
		else if (ROOMFRAME == my_overlap->operation) {
			//if (lobbytimer)
			//{
			if (Room[key].start) {
				roomrate(key);
				AddTimer(key, ROOMFRAME, 200);
			}
		//	}
			delete my_overlap;
		}
		else if (ATTACK == my_overlap->operation) {
			if (clients[key].avatar.bulletalive==true) {
				collision(key);
				AddTimer(key, ATTACK, 10);
			}
		
			delete my_overlap;
		}
		else {
			cout << "Unknown IOCP event!\n";
			exit(-1);
		}
	}
}

int main()
{
	vector <thread *> worker_threads;

	Initialize();

	for (auto i = 0; i < NUM_THREADS; ++i)
		worker_threads.push_back(new thread{ WorkerThreadStart });

	
	thread Time_Thread = thread{ Timer_Thread };
	thread accept_thread{ AcceptThreadStart };

	while (false == g_isshutdown)
	{
		Sleep(1000);
	}

	for (auto th : worker_threads) {
		th->join();
		delete th;
	}
	accept_thread.join();
	Cleanup();
}