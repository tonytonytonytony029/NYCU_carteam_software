#include "vmu_wolff.h"

static msg_t canSend(CANDriver* p, CAN20A_msg_t* m)
{
	CANTxFrame ptx;
	uint8_t i;

	ptx.EID = (m->prio << 26) | (m->pgn << 8) | m->src;
	ptx.IDE = CAN_IDE_EXT;
	ptx.RTR = CAN_RTR_DATA;
	ptx.DLC = m->len;
	memcpy(ptx.data8, m->buffer, m->len);

	return canTransmit(p, CAN_ANY_MAILBOX, &ptx, MS2ST(100));
}