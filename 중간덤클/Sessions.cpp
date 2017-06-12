#include "stdafx.h"

void build_display_array()
{
	for (int i = 0; i < TEST_PLAYER_NUM; i++)
		display_array[(player[i].x * MAP_X_LENGTH) / BOARD_WIDTH][(player[i].y * MAP_Y_LENGTH) / BOARD_WIDTH]++;

	for (int i = 0; i < MAP_X_LENGTH; i++)
		for (int j = 0; j < MAP_Y_LENGTH; j++)
			UpdateVB(i, j, display_array[i][j]);

	for (int i = 0; i < MAP_X_LENGTH; i++)
		for (int j = 0; j < MAP_Y_LENGTH; j++)
			display_array[i][j] = 0;
}


void UpdateThread()
{
	int rand_move = 0;
	srand(time(NULL));
	while (true) {
		Sleep(1000);


		for (int i = 0; i < TEST_PLAYER_NUM; i++) {
			unsigned int move_flag;
			move_flag = rand() % 4;
			if (player[i].GetConnect())
			{
				cs_packet_up MovePacket;
				MovePacket.size = sizeof(cs_packet_up);
				MovePacket.type = CS_UP + move_flag;
				//player[i].SendPacket(reinterpret_cast<CHAR*>(&MovePacket), MovePacket.size);

				cs_packet_soundchat my_packet;
				my_packet.size = sizeof(cs_packet_soundchat);
				my_packet.type = CS_SOUNDCHAT;
				CopyMemory(my_packet.buffer, "acdasd", 8);
				my_packet.length = 200;

				//	my_packet->buffer = pNewBuffer;

				// WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
				//player[i].SendPacket(reinterpret_cast<CHAR*>(&my_packet), my_packet.size);
			}
		}
	}//while
}

void ConnectThread()
{
	DWORD flag = 0;

	for (int i = 0; i<TEST_PLAYER_NUM; i++) {
		if (player[i].Connect(i)) {
			//printf("player %d connect!!\n", i);
			//----------------------------------------------------------------
			// Send Login Data.
			wchar_t temp[256];
			wsprintf(temp, L"%d", i);
			ZeroMemory(&player[i].m_overExRecv, sizeof(OVERLAPPED_EX));
			player[i].id_received = false;
			player[i].m_overExRecv.m_io_type = IO_RECV;
			player[i].m_uncomplete_size = 0;
			player[i].m_current_packet_size = 0;

			player[i].m_recv_wsa_buf.buf = player[i].real_recv_buf;
			player[i].m_recv_wsa_buf.len = MAX_BUFF_SIZE;
			int result = WSARecv(player[i].GetSocket(), &player[i].m_recv_wsa_buf, 1,
				NULL, &flag, reinterpret_cast<LPWSAOVERLAPPED>(&player[i].m_overExRecv), NULL);

			if (result == SOCKET_ERROR) {
				if (WSAGetLastError() != WSA_IO_PENDING) {
					printf("WSARecv() error!!\n");
					return;
				}
			}
			cs_packet_input my_packet;
			my_packet.size = sizeof(cs_packet_input);
			my_packet.type = CS_dummyclients;
			player[i].SendPacket(reinterpret_cast<CHAR*>(&my_packet), my_packet.size);


			//	my_packet->buffer = pNewBuffer;

			// WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
			//

		}
	}
	return ;
}

void WorkerThread()
{
	DWORD	iosize;
	DWORD	flag = 0;
	ULONG	comp_key;
	OVERLAPPED_EX *over_ptr	= NULL;
	char *buf = NULL;
	int remained_io_data_size = 0;
	int	result;

	while (true) {
		GetQueuedCompletionStatus(g_iocp, &iosize, &comp_key, (LPOVERLAPPED *)&over_ptr, INFINITE);
		if (0 == iosize) {
			closesocket(player[comp_key].m_socket);
		//	Process_disconnect(comp_key);
			break;
		}

		switch (over_ptr->m_io_type) {		
			case IO_RECV:				
				
				remained_io_data_size = iosize;
				buf = player[comp_key].real_recv_buf;

				while (remained_io_data_size > 0) {

					if (0 == player[comp_key].m_current_packet_size) player[comp_key].m_current_packet_size = buf[0];
							
					int remained_packet_data_size = player[comp_key].m_current_packet_size - player[comp_key].m_uncomplete_size;

					if (remained_packet_data_size <= remained_io_data_size) {

						memcpy(player[comp_key].m_uncomplete_packet + player[comp_key].m_uncomplete_size,
							buf,
							remained_packet_data_size);

						//ProceessPacket
						ProcessPacket(comp_key, player[comp_key].m_uncomplete_packet);

						player[comp_key].m_uncomplete_size = 0;
						player[comp_key].m_current_packet_size = 0;
						buf += remained_packet_data_size;
						remained_io_data_size -= remained_packet_data_size;
					} else { 
						memcpy(player[comp_key].m_uncomplete_packet + player[comp_key].m_uncomplete_size,
							buf,
							remained_io_data_size);
						player[comp_key].m_uncomplete_size += remained_io_data_size;						
						break;
					}
				}
				result = WSARecv( player[comp_key].GetSocket(), &player[comp_key].m_recv_wsa_buf, 1,
							NULL, &flag, reinterpret_cast<LPWSAOVERLAPPED>(&player[comp_key].m_overExRecv), NULL);

				if (result == SOCKET_ERROR) {
					if (WSAGetLastError() != WSA_IO_PENDING) {
						printf("WSARecv() error!!\n");
						return;
					}
				}
				break;

			case IO_SEND:
				delete over_ptr;
				//printf("Send Complete on User:%d\n",comp_key);
				break;
			default:
				printf("IO type error!!\n");
				break;
				
		}

	} // while()

	return;
}
