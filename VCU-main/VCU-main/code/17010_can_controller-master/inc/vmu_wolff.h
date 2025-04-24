typedef struct {
	uint32_t pgn;
	uint8_t buffer[8];
	uint8_t len;
	uint8_t dst;
	uint8_t src;
	uint8_t prio;
}CAN20A_msg_t;