#include "ch.h"
#include "hal.h"

#include <stdio.h>
#include "port.h"

#include "lwip/api.h"
#include "lwip/tcp.h"
#include "lwip/sockets.h"

#include "mb.h"
#include "mbport.h"

#define MB_TCP_UID      6
#define MB_TCP_LEN      4
#define MB_TCP_FUNC      7

#define MB_TCP_DEFAULT_PORT     502
#define MB_TCP_BUF_SIZE (256+7)


void vMBPortEventClose(void);
#ifdef MB_TCP_DEBUG
void vMBPortLog(eMBPortLogLevel eLevel, const CHAR *szModule, const CHAR *szFmt,...);
#endif
static struct tcp_pcb *pxPCBListen;
static struct tcp_pcb *pxPCBClient;

static UCHAR aucTCPBuf[MB_TCP_BUF_SIZE];
static USHORT usTCPBufPos;
static USHORT usTCPFrameBytesLeft;

static err_t prvxMBTCPPortAccept(void *pvArg, struct tcp_pcb *pxPCB,err_t xErr);
static err_t prvxMBTCPPortReceive(void *pvArg, struct tcp_pcb *pxPCB, struct pbuf *p, err_t xErr);
static void prvvMBTCPPortError(void *pvArg, err_t xErr);

static struct netconn *servercon;
static thread_t *mbTcpThd;
thread_reference_t mbtcp_trp = NULL;

#define EV_START_MBTCP EVENT_MASK(0)
#define EV_STOP_MBTCP  EVENT_MASK(1)
#define EV_DELETE_CONN EVENT_MASK(2)

enum tcp_conn_state_e{
  ST_IDLE,
  ST_INIT,
  ST_WAITCLIENT,
  ST_CONNECTING,
  ST_CONNECTED,
  ST_DISCONNECT,
  ST_CLOSE
};
struct netconn *serverconn, *newconn;
struct netconn *clientConn;

int xListenSocket;
int xClientSocket = -1;
static fd_set allset;
_mb_instance_t *mbtcp;

static THD_WORKING_AREA(waMBTcpThread,512);
static THD_FUNCTION(mbTcpThread,arg)
{
  //(void)arg;
  USHORT *port = (USHORT*)arg;
  USHORT portNum;
  struct netconn *conn;
  err_t err;
  eMBErrorCode eStatus;
  thread_t *clientThread = NULL;
  bool connected = false;
  struct netbuf *inbuf;
  struct netconn *tmpconn;


//  if(port == 0)
//    portNum = MB_TCP_PORT_USE_DEFAULT;
//  else
//    portNum = *port;
//
//  serverconn = netconn_new(NETCONN_TCP);
//  if(!serverconn){
//    chThdResume(&mbtcp_trp, MSG_RESET);
//  }
//
//  err = netconn_bind(serverconn,NULL,portNum);
//  if(err != ERR_OK){
//    chThdResume(&mbtcp_trp, MSG_RESET);
//  }
//
//  err = netconn_listen(serverconn);
//  if(err != ERR_OK){
//    netconn_delete(serverconn);
//    chThdResume(&mbtcp_trp, MSG_RESET);
//  }

//  netconn_set_nonblocking(serverconn,1);
//  serverconn->recv_timeout = 100;

  // resume caller
  chThdResume(&mbtcp_trp, MSG_OK);
  char *buf;
  uint16_t bufLen;
  ip_addr_t *addr;
  uint16_t p;
  while(true){
//    err = netconn_accept(serverconn,&tmpconn);
//    if(err == ERR_OK){
//      if(connected){
//        netconn_disconnect(newconn);
//      }
//      newconn = tmpconn;
//      newconn->recv_timeout = 10;
//      netconn_set_nonblocking(newconn,1);
//      connected = true;
//    }
//
//    chThdSleepMilliseconds(10);
//    eventmask_t evt = chEvtWaitAnyTimeout(ALL_EVENTS,MS2ST(10));
//    if(evt & EV_START_MBTCP){
//      while(!connected){
//        err = netconn_accept(serverconn,&newconn);
//        if(err == ERR_OK){
//          newconn->recv_timeout = 100;
//          connected = true;
//        }
//        chThdSleepMilliseconds(10);
//      }
//    }
//    if(evt & EV_STOP_MBTCP){
//      netconn_delete(newconn);
//
//      connected = false;
//    }
//    if(evt & EV_DELETE_CONN){
//      netconn_delete(newconn);
//      connected = false;
//    }

//    if(connected){
//    if(newconn){
//      err = netconn_recv(newconn, &inbuf);
//        if(err == ERR_OK){
//          netbuf_data(inbuf,(void**)&buf,&bufLen);
//          netbuf_delete(inbuf);
//          if((usTCPBufPos + bufLen) > MB_TCP_BUF_SIZE){
//            connected = false;
//          }else{
//            USHORT usLength;
//            memcpy(&aucTCPBuf[usTCPBufPos],buf, bufLen);
//            usTCPBufPos += bufLen;
//            if(usTCPBufPos >= MB_TCP_FUNC){
//              usLength = aucTCPBuf[MB_TCP_LEN] << 8U;
//              usLength |= aucTCPBuf[MB_TCP_LEN+1];
//              if(usTCPBufPos < (MB_TCP_UID + usLength)){
//              }else if(usTCPBufPos == (MB_TCP_UID + usLength)){
//                (void)xMBPortEventPost(EV_FRAME_RECEIVED,mbtcp);
//              }else{
//                connected = false;
//              }
//            }
//          }
//        }else{
//          netconn_getaddr(newconn,addr,&p,0);
//          switch(err){
//          case ERR_CLSD:
//            connected = false;
//            break;
//          case ERR_TIMEOUT:
//            break;
//          default:
//            connected = false;
//          }
//        }
//    }
    eStatus = eMBPoll(1);
    if(eStatus != MB_ENOERR){
      p = 4;
    }
    //chSysLock();
    //chSysUnlock();
    chThdSleepMilliseconds(20);
  }
    
}

/*
 *  changed on 8/9, 2017, use netconn with callback fufnction
 */
void clientcb(struct netconn *pconn, enum netconn_evt event, uint16_t len)
{
  if(newconn == pconn){
    switch(event){
    case NETCONN_EVT_RCVPLUS:
      if(len > 0){
        err_t err;
        struct netbuf *inbuf;
        char *buf;
        uint16_t bufLen;

        err = netconn_recv(pconn, &inbuf);
        if(err == ERR_OK){
          netbuf_data(inbuf,(void**)&buf,&bufLen);
          netbuf_delete(inbuf);
          if((usTCPBufPos + bufLen) > MB_TCP_BUF_SIZE){
//            connected = false;
          }else{
            USHORT usLength;
            memcpy(&aucTCPBuf[usTCPBufPos],buf, bufLen);
            usTCPBufPos += bufLen;
            if(usTCPBufPos >= MB_TCP_FUNC){
              usLength = aucTCPBuf[MB_TCP_LEN] << 8U;
              usLength |= aucTCPBuf[MB_TCP_LEN+1];
              if(usTCPBufPos < (MB_TCP_UID + usLength)){
              }else if(usTCPBufPos == (MB_TCP_UID + usLength)){
                (void)xMBPortEventPost(EV_FRAME_RECEIVED,mbtcp);
              }else{
//                connected = false;
              }
            }
          }
        }
      }

      break;
    case NETCONN_EVT_RCVMINUS:
      break;
    case NETCONN_EVT_SENDPLUS:
      break;
    case NETCONN_EVT_SENDMINUS:
      break;
    case NETCONN_EVT_ERROR:
      break;
    }
  }
}

void tcpCallback(struct netconn *pconn, enum netconn_evt event, uint16_t len)
{
  err_t err;
  struct netbuf *inbuf;
  char *buf;
  uint16_t bufLen;
  if(pconn == serverconn){
    switch(event){
    case NETCONN_EVT_RCVPLUS:
      if((err = netconn_accept(serverconn,&newconn)) != ERR_OK){
        return;
      }
      newconn->callback = tcpCallback;
      netconn_set_nonblocking(newconn,1);

      newconn->recv_timeout = 10;
      break;
    case NETCONN_EVT_RCVMINUS:
      break;
    case NETCONN_EVT_SENDPLUS:
      break;
    case NETCONN_EVT_SENDMINUS:
      break;
    case NETCONN_EVT_ERROR:
      break;
    }
  }
  else if(newconn == pconn){
    switch(event){
    case NETCONN_EVT_RCVPLUS:
      if(len > 0){

        err = netconn_recv(pconn, &inbuf);
        if(err == ERR_OK){
          netbuf_data(inbuf,(void**)&buf,&bufLen);
          netbuf_delete(inbuf);
          if((usTCPBufPos + bufLen) > MB_TCP_BUF_SIZE){
//            connected = false;
          }else{
            USHORT usLength;
            memcpy(&aucTCPBuf[usTCPBufPos],buf, bufLen);
            usTCPBufPos += bufLen;
            if(usTCPBufPos >= MB_TCP_FUNC){
              usLength = aucTCPBuf[MB_TCP_LEN] << 8U;
              usLength |= aucTCPBuf[MB_TCP_LEN+1];
              if(usTCPBufPos < (MB_TCP_UID + usLength)){
              }else if(usTCPBufPos == (MB_TCP_UID + usLength)){
                (void)xMBPortEventPost(EV_FRAME_RECEIVED,mbtcp);
              }else{
//                connected = false;
              }
            }
          }
        }
      }

      break;
    case NETCONN_EVT_RCVMINUS:
      break;
    case NETCONN_EVT_SENDPLUS:
      break;
    case NETCONN_EVT_SENDMINUS:
      break;
    case NETCONN_EVT_ERROR:
      break;
    }
  }
}

BOOL xMBTCPPortInit(USHORT usTCPPort)
{
  BOOL bOkay = FALSE;
  USHORT portNum;
  err_t err;
  bOkay = TRUE;

  mbtcp = &mbInstance[1];
//  mbTcpThd = chThdCreateStatic(waMBTcpThread,sizeof(waMBTcpThread),NORMALPRIO,mbTcpThread,(USHORT*)&usTCPPort);
//  chSysLock();
//  chThdSuspendS(&mbtcp_trp);
//  chSysUnlock();

  if(usTCPPort == 0)
    portNum = MB_TCP_PORT_USE_DEFAULT;
  else
    portNum = usTCPPort;

  serverconn = netconn_new(NETCONN_TCP);
  if(!serverconn){
    bOkay = FALSE;
  }

  err = netconn_bind(serverconn,NULL,portNum);
  if(err != ERR_OK){
    bOkay = FALSE;
  }

  err = netconn_listen(serverconn);
  if(err != ERR_OK){
    netconn_delete(serverconn);
    bOkay = FALSE;
  }else{
    netconn_set_nonblocking(serverconn,1);
    serverconn->callback = tcpCallback;
    bOkay = TRUE;
//    serverconn->recv_timeout = 100;
      mbTcpThd = chThdCreateStatic(waMBTcpThread,sizeof(waMBTcpThread),NORMALPRIO,mbTcpThread,(USHORT*)&usTCPPort);
      chSysLock();
      chThdSuspendS(&mbtcp_trp);
      chSysUnlock();
  }


  return bOkay;
  
  
}

void prvvMBPortReleaseClient(struct netconn *pconn)
{
   // chEvtSignal(mbTcpThd,EV_DELETE_CONN);
  netconn_delete(pconn);
//  lwip_close(id);
}

void vMBTCPPortClose()
{
//  prvvMBPortReleaseClient(pxPCBClient);
//  prvvMBPortReleaseClient(pxPCBListen);
 // chSysLock();
  //chEvtSignal(mbTcpThd,EV_TCP_CLOSE);
 // chSysUnlock();  
//  netconn_delete(serverconn);
  vMBPortEventClose();
}

void vMBTCPPortStart(void)
{
  chEvtSignal(mbTcpThd,EV_START_MBTCP);
}

void vMBTCPPortDisable(void)
{
    //chEvtSignal(mbTcpThd,EV_TCP_DISABLE);

  //prvvMBPortReleaseClient(pxPCBClient);
}


BOOL xMBTCPPortGetRequest(UCHAR **ppucMBTCPFrame, USHORT *usTCPLength)
{
  *ppucMBTCPFrame = &aucTCPBuf[0];
  *usTCPLength = usTCPBufPos;
  
  usTCPBufPos = 0;
  return TRUE;
}

BOOL xMBTCPPortSendResponse(const UCHAR *pucMBTCPFrame, USHORT usTCPLength)
{
  BOOL bFrameSent = FALSE;
  
  if(newconn){
    netconn_write(newconn, pucMBTCPFrame, usTCPLength, NETCONN_COPY);
    bFrameSent = TRUE;
  }
  

  return bFrameSent;
}

