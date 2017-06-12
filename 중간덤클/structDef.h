typedef struct {
	WSAOVERLAPPED	m_Overlapped;
	INT				m_io_type;
} OVERLAPPED_EX;

typedef struct {
	OVERLAPPED_EX	m_OverExSend;
	WSABUF			m_SendWSABuf;
	CHAR			m_SendBuffer[MAX_BUFF_SIZE];
} SEND_STRUCT;

struct location {
	int x;
	int y;
};
