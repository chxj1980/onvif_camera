/************************************************************
*Copyright (C),lcb0281at163.com lcb0281atgmail.com
*FileName: common.h
*BlogAddr: https://blog.csdn.net/li_wen01
*Description:
*Date:	   2019-06-22
*Author:   Caibiao Lee
*Version:  V1.0
*Others:
*History:
***********************************************************/
#ifndef __COMMON_H_
#define __COMMON_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>	
#include <sys/socket.h>

/***************������*****************/
#define RTP_CLIENT_PORT      55182 //���ص�RTP��IPCͨ�ŵĶ˿ںű���Ϊż��
#define RTCP_CLIENT_PORT     44311 //���ص�RTCP��IPCͨ�ŵĶ˿ںŸ���RTPͨ������������Ϊ����

/****************������**********************/
#define RTP_CLIENT_SUBPORT   55184  
#define RTCP_CLIENT_SUBPORT  44587

/***********��������***********/
#define MAINSTREAM  0x01   /**ֻ����������**/
#define SUBSTREAM   0x03   /**ֻ����������**/

#define HTTP_CNONCE   "Caibiao_Lee"

typedef enum 
{
	false = 0, 
	true = 1,
} bool;


typedef struct 
{
	char arrs8Name[16];	   /**�û���**/
	char arrs8PassWord[16]; /**����**/
}CLIENT_USER_INFO_S;

typedef struct 
{
	int  s32AuthorType;
	char arrs8Type[16];
	char arrs8Control[16];
	char arrs8RTSPUrl[128];
	char arrs8Realm[32];
	char arrs8Nonce[128];
}RTSP_SDP_PARA_S;

typedef struct
{
	bool bRTPState;	            /**RTSP״̬**/
	int  s32SockFd;             /**RTSP socket**/
	int  s32StreamType;			/****/
	char arrs8SessionId[16];    /**RTSP�ỰID**/
	char arrs8RTSPUrl[128];     /**RTSP��ַ**/
	char arrs8ServerIP[32];     /**RTSP�����IP**/
	char arrs8SerRTPPort[12];	/**�����RTP �˿ں�**/
	char arrs8SerRTCPPort[12];	/**�����RTCP�˿ں�**/
	RTSP_SDP_PARA_S stSDPPara;  /**SDP ����**/
	CLIENT_USER_INFO_S stClientUser;/**�û���Ϣ**/
}RTSP_STATUS_S;


void COM_base64_bits_to_64(unsigned char *out, const unsigned char *in, int inlen);
int COM_base64_64_to_bits(char *out, const char *in);
int COM_authorization_digest(char *name,char *password,char *realm,char *nonce,unsigned char *response,char *meth,char *addr,char *qop,int cntnonce);
char *COM_get_substringstart(char *str,char *sub);
char *COM_get_begingstring(char *src,char *dest,char flag);
char *COM_get_localIP(char *IP);
void COM_genrate_digest(char *pwddigest_out,char *pwd,char *nonc, char *time) ;

int NET_SocketCreate(int s32Type);
int NET_SocketClose(int s32SocketFd);
int NET_SocketConnect(int s32SocketFd,char *pstIP,int s32Port);
int NET_SocketSendData(int s32SocketFd,char *ps8buf,int s32Size);
int NET_SocketRecvData(int s32SocketFd,void  *ps8buf,int s32Len);

#endif

