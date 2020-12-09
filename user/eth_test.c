#include <stdio.h>
#include "eth_test.h"
#include "socket.h"
#include "wizchip_conf.h"
#include "main.h"

//初始化配置定义
wiz_NetInfo gWIZNETINFO = { .mac = {0x78, 0x83, 0x68, 0x88, 0x56, 0x38},
                            .ip =  {192, 168, 0,17},
                            .sn =  {255,255,255,0},
                            .gw =  {192, 168,0,1},
                            .dns = {180,76,76,76},
                            .dhcp = NETINFO_DHCP};


//初始化芯片参数
void ChipParametersConfiguration(void)
{
  uint8_t tmp;

  uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};
    if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1)
    {
        printf("WIZCHIP Initialized fail.\r\n");
        while(1);
    }
    do
    {
        if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1)
        {
            printf("Unknown PHY Link stauts.\r\n");
        }
    }while(tmp == PHY_LINK_OFF);
}

//初始化网络通讯参数,mac,ip等
void NetworkParameterConfiguration(void)  //Intialize the network information to be used in WIZCHIP

{

  uint8_t tmpstr[6];

  ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);

  ctlnetwork(CN_GET_NETINFO, (void*)&gWIZNETINFO);

  ctlwizchip(CW_GET_ID,(void*)tmpstr);
    //串口打印出参数
    printf("\r\n=== %s NET CONF ===\r\n",(char*)tmpstr);
    printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],
            gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
    printf("SIP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
    printf("GAR: %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);
    printf("SUB: %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);
    printf("DNS: %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0],gWIZNETINFO.dns[1],gWIZNETINFO.dns[2],gWIZNETINFO.dns[3]);
    printf("======================\r\n");

}

void W5500_ChipInit(void)
{
    ChipParametersConfiguration();
    NetworkParameterConfiguration();
}  
#include "loopback.h"
uint8_t rev_buf[1024];
uint8_t send_buf[1024];
uint8_t ip[] = {192,168,0,123};
void w5500_test(void)
{
    loopback_tcps(0, rev_buf, 100);  
    loopback_tcps(1, rev_buf, 100); 
    loopback_tcps(2, rev_buf, 100); 
    loopback_tcps(3, rev_buf, 100); 
    
    loopback_tcpc(4, send_buf, ip, 1004);
    loopback_tcpc(5, send_buf, ip, 1004);
    loopback_tcpc(6, send_buf, ip, 1004);
    loopback_tcpc(7, send_buf, ip, 1004);
 
}