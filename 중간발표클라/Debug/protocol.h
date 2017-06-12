#include <Windows.h>

#include <d3dx11.h>
#include <D3DX10Math.h>

#define MAX_BUFF_SIZE  4000
#define MAX_PACKET_SIZE  255

#define BOARD_WIDTH   400
#define BOARD_HEIGHT  400

#define VIEW_RADIUS   7
#define MAX_USER 500

#define NPC_START  500
#define NUM_OF_NPC  200

#define MY_SERVER_PORT  4000

#define MAX_STR_SIZE  100

#define CS_UP    1
#define CS_DOWN  2
#define CS_LEFT  4
#define CS_RIGHT    8
#define CS_CHAT		5
#define CS_SOUNDCHAT	45
#define CS_ATTACK		6
#define CS_IDSERCH  7
#define CS_LEFTROTATION 40
#define CS_RIGHTROTATION 41
#define CS_UPROTATION 42
#define CS_DOWNROTATION 43
#define CS_gameplay	50
#define CS_freeveiw 51
#define CS_exitroom 52
#define CS_changeteam 53
#define CS_selectroom 54  //+ createroom
#define CS_roomnumveiw 55 
#define CS_roomcontrol 56
#define CS_addAIplayer 57
#define CS_deleteAIplayer 58
#define SC_MINIONPOS 59
#define CS_RESTART	20
#define CS_COLL 47
#define CS_LKVECTOR	27
#define SC_POS           1
#define SC_PUT_PLAYER    2
#define SC_REMOVE_PLAYER 3
#define SC_ROTATE 3
#define SC_CHAT		12
#define SC_ATTACK		5
#define SC_HIT		6
#define SC_IDSERCH  7
#define SC_bullet 60
#pragma pack (push, 1)

struct cs_packet_input {
	BYTE size;
	BYTE type;
};

struct cs_packet_roominfor {
	BYTE size;
	BYTE type;
	int roomnum;
};

struct cs_packet_changeteam {
	BYTE size;
	BYTE type;
	WORD id;
	BOOL team;
	WORD roomnum;
	bool state;
};
struct cs_packet_aipacket {
	BYTE size;
	BYTE type;
	WORD id;
	BOOL team;
	WORD roomnum;

};
struct sc_packet_collision {
	BYTE size;
	BYTE type;
	WORD id;
	WORD index;
	int state;


};


struct cs_packet_roomveiw {
	BYTE size;
	BYTE type;
	WORD num;
	WORD count;
	WORD playtime;
};

struct cs_packet_bullet {
	BYTE size;
	BYTE type;
	WORD id;
	WORD index;
	float x;
	float y;
	float z;
	bool team;
};
struct cs_packet_lookvector {
	BYTE size;
	BYTE type;
	float x;
	float y;
	float z;
};
struct sc_packet_minionpos {
	BYTE size;
	BYTE type;
	WORD id;
	bool team;
	D3DXMATRIX mtx;
};


struct sc_packet_rotation {
	BYTE size;
	BYTE type;
	WORD id;
	BYTE lr;
	bool team;
};

struct cs_packet_move {
	BYTE size;
	BYTE type;
	WORD id;
	float x;
	float y;
};

struct cs_packet_soundchat {
	BYTE size;
	BYTE type;
	//INT id;
	BYTE buffer[4000];
	INT length;
};


struct cs_packet_up {
	BYTE size;
	BYTE type;
};

struct cs_packet_down {
	BYTE size;
	BYTE type;
};
struct cs_packet_IDserch {
	BYTE size;
	BYTE type;
	BYTE id;
};
struct cs_packet_left {
	BYTE size;
	BYTE type;
};

struct cs_packet_right {
	BYTE size;
	BYTE type;
};

struct cs_packet_chat {
	BYTE size;
	BYTE type;
	WCHAR message[MAX_STR_SIZE];
};
struct sc_packet_hit {
	BYTE size;
	BYTE type;
	WORD id;
	INT target;
	BYTE hittype;
};

struct sc_packet_pos {
	BYTE size;
	BYTE type;
	WORD id;
	bool team;
	float x;
	float z;
	D3DXMATRIX mtx;
};

struct sc_packet_put_player {
	BYTE size;
	BYTE type;
	WORD id;
	WORD x;
	WORD z;
	D3DXMATRIX mtx;
	bool team;
};
struct sc_packet_remove_player {
	BYTE size;
	BYTE type;
	WORD id;
};
struct sc_packet_arrow {
	BYTE size;
	BYTE type;
	WORD id;
	WORD x;
	WORD y;
	BYTE state;
	BYTE arrowdirection;
	INT hp;
};

struct sc_packet_chat {
	BYTE size;
	BYTE type;
	WORD id;
	WCHAR message[MAX_STR_SIZE];
};

#pragma pack (pop)