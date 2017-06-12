#include "stdafx.h"

DWORD WINAPI UpdateThread(LPVOID para)
{
	int rand_move = 0;
	srand(time(NULL));
	while(true){
		Sleep(1000);
		for (int i=0; i<TEST_PLAYER_NUM; i++) {
			unsigned int move_flag;
			move_flag = rand()%4;
			if( player[i].GetConnect() )
			{
				cs_packet_up MovePacket;
				MovePacket.size = sizeof(cs_packet_up);
				MovePacket.type = CS_UP + move_flag;
				player[i].SendPacket(reinterpret_cast<CHAR*>(&MovePacket), MovePacket.size);
			}
		}


		Objlist::iterator it;

//		for (int i=0; i<TEST_PLAYER_NUM; ++i) {
//			if (objectList.find(i) == objectList.end()) {
//			objectList[player[i].obj_id].x = player[i].GetX();
//			objectList[player[i].obj_id].y = player[i].GetY();
//			}

		for (it = objectList.begin(); it != objectList.end(); ++it)
			InterlockedIncrement(&display_array[(it->second.x * MAP_X_LENGTH)/BOARD_WIDTH][(it->second.y * MAP_Y_LENGTH)/BOARD_WIDTH]);
		
		for ( int i=0; i<MAP_X_LENGTH; i++)
			for (int j=0; j<MAP_Y_LENGTH; j++)
				UpdateVB( i, j, display_array[i][j]);

		for ( int i=0; i<MAP_X_LENGTH; i++)
			for (int j=0; j<MAP_Y_LENGTH; j++)
				display_array[i][j] = 0;

		

	}//while
}