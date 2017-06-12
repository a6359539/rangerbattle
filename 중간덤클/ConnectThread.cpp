#include "stdafx.h"

DWORD WINAPI ConnectThread(LPVOID para)
{
	DWORD flag = 0;
	
	for (int i=0; i<TEST_PLAYER_NUM; i++) {
		if ( player[i].Connect(i) ) {
			//printf("player %d connect!!\n", i);
			//----------------------------------------------------------------
			// Send Login Data.
			wchar_t temp[256];
			wsprintf(temp,L"%d", i);
			ZeroMemory(&player[i].m_overExRecv, sizeof(OVERLAPPED_EX));
			player[i].m_overExRecv.m_io_type = IO_RECV;
			player[i].m_uncomplete_size = 0;
			player[i].m_current_packet_size = 0;

			player[i].m_recv_wsa_buf.buf = player[i].real_recv_buf;
			player[i].m_recv_wsa_buf.len = MAX_BUFF_SIZE;

			int result = WSARecv( player[i].GetSocket(), &player[i].m_recv_wsa_buf, 1,
							NULL, &flag, reinterpret_cast<LPWSAOVERLAPPED>(&player[i].m_overExRecv), NULL);

			if (result == SOCKET_ERROR) {
				if (WSAGetLastError() != WSA_IO_PENDING) {
					printf("WSARecv() error!!\n");
					return FALSE;
				}
			}
		}
	}
	return 0;
}