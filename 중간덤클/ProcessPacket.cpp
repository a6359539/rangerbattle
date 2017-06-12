#include "stdafx.h"

void ProcessMovePacket(int user_id, char *packet)
{
	struct sc_packet_pos *movePacket = (struct sc_packet_pos *)packet;

//	if (movePacket->id >= TEST_PLAYER_NUM) return;
	if (user_id != movePacket->id) return;
//	if (player[user_id] != movePacket->id) return;
	player[user_id].x = movePacket->x;
	player[user_id].y = movePacket->y;
}


void ProcessPutObjectPacket(int user_id, char *packet)
{
	struct sc_packet_put_player *PutObject = reinterpret_cast<sc_packet_put_player *>(packet);

	if (PutObject->id >= TEST_PLAYER_NUM) return;
	if (false == player[user_id].id_received) {
		player[user_id].obj_id = PutObject->id;
	}

	player[user_id].x = PutObject->x;
	player[user_id].y = PutObject->z;	
}

bool ProcessPacket(int user_id, char *packet)
{
	switch (packet[1]) {

		case SC_PUT_PLAYER:
			
			ProcessPutObjectPacket(user_id, packet);
			break;

		case SC_POS:
			ProcessMovePacket(user_id, packet);
			break;

		case SC_REMOVE_PLAYER:
		//	ProcessRemovePacket(packet);
			break;


			break;
	}
	

	return TRUE;
}