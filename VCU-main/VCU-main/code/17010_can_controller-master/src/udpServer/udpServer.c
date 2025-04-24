/*
 * udpServer.c
 *
 *  Created on: 2017¦~8¤ë24¤é
 *      Author: Jason
 */

#include "lwip/api.h"
#include "lwip/tcp.h"
#include "lwip/sockets.h"
#include "udpServer.h"
#include "sysParam.h"

extern thread_t *sdThd, *tcpThd;

thread_t *thUdpServer;
thread_reference_t udpServer_trp = NULL;
struct netconn *udpConn;

static THD_WORKING_AREA(waUdpServer,1024);
static THD_FUNCTION(thdUdpServer,arg)
{
  err_t err;
  ip_addr_t *addr;
  ip_addr_t destip;
  uint16_t port;
  struct netbuf *buf,*sndBuf;
  char buffer[128];
  char *dptr;
  struct netif *thisif;
  char ifName[3] = "gi0";

  thisif = netif_find(ifName);

  udpConn = netconn_new(NETCONN_UDP);

  err = netconn_bind(udpConn,IP_ADDR_ANY,1290);
  //netconn_connect(server)
  if(err != ERR_OK){
    chThdResume(&udpServer_trp, MSG_RESET);
  }else{
    chThdResume(&udpServer_trp, MSG_OK);
  }

  while(!chThdShouldTerminateX()){
    err = netconn_recv(udpConn, &buf);
    if(err == ERR_OK){

      addr = netbuf_fromaddr(buf);
      destip = *addr;
      port = netbuf_fromport(buf);
      netbuf_copy(buf,buffer,buf->p->tot_len);
      buffer[buf->p->tot_len] = '\0';
      sndBuf = netbuf_new();
      dptr = netbuf_alloc(sndBuf,40);

      if(strncmp(buffer,"RESET",5)==0){
        chSysDisable();
        NVIC_SystemReset();
      }
      else if(strncmp(buffer,"SETIP",5) == 0){
        sscanf(buffer,"SETIP:%u.%u.%u.%u\0",&moduleParam.ip[0],&moduleParam.ip[1],&moduleParam.ip[2],&moduleParam.ip[3]);
      }
      else if(strncmp(buffer,"SETGATEWAY",10) == 0){
        sscanf(buffer,"SETGATEWAY:%u.%u.%u.%u\0",&moduleParam.gateway[0],&moduleParam.gateway[1],&moduleParam.gateway[2],&moduleParam.gateway[3]);
      }
      else if(strncmp(buffer,"SETNETMASK",10) == 0){
        sscanf(buffer,"SETNETMASK:%u.%u.%u.%u\0",&moduleParam.mask[0],&moduleParam.mask[1],&moduleParam.mask[2],&moduleParam.mask[3]);
      }
      else if(strncmp(buffer,"SETMAC",6) == 0){
        sscanf(buffer,"SETMAC:%x:%x:%x:%x:%x:%x\0",&moduleParam.macAddr[0],&moduleParam.macAddr[1],&moduleParam.macAddr[2],&moduleParam.macAddr[3],&moduleParam.macAddr[4],&moduleParam.macAddr[5]);
      }else if(strncmp(buffer,"SAVECFG",7) == 0){
        sysSaveParam();
      }else if(strncmp(buffer,"IDN?",4) == 0){
        chsnprintf(dptr,40,"%s,%02x:%02x:%02x:%02x:%02x:%02x,0x%04x,0x%04x\n",
                   BOARD_NAME,
                   thisif->hwaddr[0],thisif->hwaddr[1],thisif->hwaddr[2],thisif->hwaddr[3],thisif->hwaddr[4],thisif->hwaddr[5],
                   100,200);
//        moduleParam.version,moduleParam.serialNumber);
      }


      sndBuf->port = 1291;
      sndBuf->addr = destip;
      //buf->port = 55696;
      netconn_send(udpConn,sndBuf);
      //netconn_write(serverconn, resp, 2, NETCONN_COPY);
      //netconn_sendto(serverconn,sndBuf,&destip,port);
      netbuf_delete(buf);
      netbuf_delete(sndBuf);
    }

  }
}


void updServerInit()
{
  thUdpServer = chThdCreateStatic(waUdpServer,sizeof(waUdpServer),NORMALPRIO,thdUdpServer,NULL);
  chSysLock();
  chThdSuspendS(&udpServer_trp);
  chSysUnlock();
}
