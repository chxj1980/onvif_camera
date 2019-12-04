/************************************************************
*Copyright (C),lcb0281at163.com lcb0281atgmail.com
*FileName: rtp_client.c
*BlogAddr: https://blog.csdn.net/li_wen01
*Description: RTP Э�� 
*Date:	   2019-10-05
*Author:   Caibiao Lee
*Version:  V1.0
*Others:
*History:
***********************************************************/
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "rtp_client.h"


#define RTP_RECV_DATA_LEN       1500

/**for debug **/
FILE * g_WriteFd = NULL;


/******************************************************** 
Function:    IPC_RTP_GetCompleteFrame  
Description: ��ʼ��RTP���ݽ������
Input:  *pstRTPInfo 
OutPut: *pstRTPInfo
Return: 0 �ɹ�����0ʧ��
Others: 
Author: Caibiao Lee
Date:   2019-10-05
*********************************************************/
int RTP_Client_Init(RTP_STATUS_S *pstRTPClient)
{
	if(NULL==pstRTPClient)
	{
		printf("%s %d input para error \n",__FUNCTION__,__LINE__);
		return -1;
	}

	pstRTPClient->bRTPState = false;
	memset(pstRTPClient->arrs8SessionId,0,sizeof(pstRTPClient->arrs8SessionId));
	pstRTPClient->s32RTPSockFd = -1;

	return 0;
}

/******************************************************** 
Function:    RTP_Client_Release  
Description: �ͷ�RTPЭ���������Դ
Input:  *pstRTPInfo 
OutPut: *pstRTPInfo
Return: 0 �ɹ�����0ʧ��
Others: 
Author: Caibiao Lee
Date:   2019-10-05
*********************************************************/
int RTP_Client_Release(RTP_STATUS_S *pstRTPClient)
{
	
	pstRTPClient->bRTPState = false;
    if(pstRTPClient->s32RTPSockFd>0)
    {
        NET_SocketClose(pstRTPClient->s32RTPSockFd);
        pstRTPClient->s32RTPSockFd = -1;
    }

    if(NULL!=g_WriteFd)
    {
        fclose(g_WriteFd);
    }

	return 0;
}

/******************************************************** 
Function:    IPC_RTP_Session  
Description: ����RTP�Ự
Input:  *pstRTPClient
OutPut: *pstRTPClient
Return: 0 �ɹ�����0ʧ��
Others: 
    1.RTP ���ｨ������UDP����
Author: Caibiao Lee
Date:   2019-10-05
*********************************************************/
int RTP_CLient_Session(RTP_STATUS_S *pstRTPClient)
{
    int l_s32Sockfd;
    int l_s32Ret;
    unsigned int l_u32ClientPort;
    unsigned int l_u328ServPort;
    bool l_bRTPState;

	if(NULL==pstRTPClient)
	{
		printf("%s %d input para error \n",__FUNCTION__,__LINE__);
		return -1;
	}

	l_s32Sockfd     = pstRTPClient->s32RTPSockFd;
	l_u32ClientPort = RTP_CLIENT_PORT;
	l_u328ServPort  = pstRTPClient->u32SerRTPPort;
	l_bRTPState     = pstRTPClient->bRTPState;
	
	if(true == l_bRTPState)
	{
		printf("%s %d RTP is already start \n",__FUNCTION__,__LINE__);
		return 0;
	}
	
	if(l_s32Sockfd > 0)
	{
		NET_SocketClose(l_s32Sockfd);
	}

	/**����UDP����**/
    l_s32Sockfd = NET_SocketCreate(SOCK_DGRAM);
    if(l_s32Sockfd < 0)
    {
        printf("%s %d Socket create error",__FUNCTION__,__LINE__);
        return -3;
    }
	
	printf("%s %d ID = %d,ClientPort = %d,ServPort = %d,Server IP = %s\n",
		__FUNCTION__,__LINE__,l_s32Sockfd,l_u32ClientPort,l_u328ServPort,
		pstRTPClient->arrs8ServerIP);
	
    l_s32Ret = NET_SocketBind(l_s32Sockfd,l_u32ClientPort);
    if(l_s32Ret < 0)
    {
        printf("%s %d Socket Bind error\n",__FUNCTION__,__LINE__);
        return -4;
    }
   
    l_s32Ret = NET_SocketConnect(l_s32Sockfd,pstRTPClient->arrs8ServerIP,l_u328ServPort);
    if(l_s32Ret < 0)
    {
        printf("%s %d Socket Connect Error \n",__FUNCTION__,__LINE__);
        return -5;
    }

	pstRTPClient->bRTPState = true;
	pstRTPClient->s32RTPSockFd = l_s32Sockfd;
	
    return 0;

}

/******************************************************** 
Function:    RTP_Client_H264StreamStore  
Description: �洢H264������
Input:  pstRTPInfo 
OutPut: pstRTPInfo
Return: 0 �ɹ�����0ʧ��
Others: 
    1.h264 RTP ������û�� 00 00 00 01��ǩ,�洢��ʱ����Ҫ��
      �ñ�ǩ����ϡ�
    2.���������SPS��PPS,SEI ֡���ͱ�ǩ������֡�����ڽ��ʱ�����
    3.Ϊ�������룬������Ҫ�ȵ�������SPS,PPS֮��Ž���������Ƶ֡
Author: Caibiao Lee
Date:   2019-10-05
*********************************************************/
int RTP_Client_H264StreamStore(RTP_UNPACK_S stRtpUnpack)
{
    static bool ls_bWriteFlag = false;
#if 0    
    printf("\n\n");
    printf("eFrameType    :%d \n",stRtpUnpack.eFrameType);
    printf("u8OutNaluType :%d \n",stRtpUnpack.u8OutNaluType);
    printf("u8Version     :%d \n",stRtpUnpack.stOutRTPPack.u8Version);
    printf("u8Padding     :%d \n",stRtpUnpack.stOutRTPPack.u8Padding);
    printf("u8Extension   :%d \n",stRtpUnpack.stOutRTPPack.u8Extension);
    printf("u8Cc          :%d \n",stRtpUnpack.stOutRTPPack.u8Cc);
    printf("u8Marker      :%d \n",stRtpUnpack.stOutRTPPack.u8Marker);
    printf("u8Pt          :%d \n",stRtpUnpack.stOutRTPPack.u8Pt);
    printf("u32SeqNum     :%d \n",stRtpUnpack.stOutRTPPack.u32SeqNum);
    printf("u32TimeStamp  :%d \n",stRtpUnpack.stOutRTPPack.u32TimeStamp);
    printf("u32Ssrc       :%u \n",stRtpUnpack.stOutRTPPack.u32Ssrc);
    printf("pu32Paylen    :%d \n",stRtpUnpack.stOutRTPPack.u32Paylen);
#endif

    unsigned char l_arrH264Flag[5] = {0};
    
    switch(stRtpUnpack.eFrameType)
    {
        case NAL_PACK: /**����**/
        {
            break;
        }

        case AP_PACK:  /**��ϰ�**/
        {
            break;
        }
        
        case FU_START_PACK:
        {
            l_arrH264Flag[0] = 0x00;
            l_arrH264Flag[1] = 0x00;    
            l_arrH264Flag[2] = 0x00;
            l_arrH264Flag[3] = 0x01;

            if((stRtpUnpack.stOutRTPPack.u32Paylen>0)&&(true==ls_bWriteFlag))
            {
                /**ע�ⳤ�ȣ�ls_arrH264Flag[4] ��ֵ�ڽ���ʱ�Ѿ��������**/
                fwrite(l_arrH264Flag,1,4,g_WriteFd);
            }

            break;
        }

        case FU_MIDllE_PACK:
        {
            break;
        }

        case FU_END_PACK:
        {
            
            break;
        }

        case H264_SEI:
        {
            l_arrH264Flag[0] = 0x00;
            l_arrH264Flag[1] = 0x00;    
            l_arrH264Flag[2] = 0x00;
            l_arrH264Flag[3] = 0x01;
            l_arrH264Flag[4] = 0x66;
            fwrite(l_arrH264Flag,1,5,g_WriteFd);
            ls_bWriteFlag = true;
            break;
        }

        case H264_PPS:
        {
            l_arrH264Flag[0] = 0x00;
            l_arrH264Flag[1] = 0x00;    
            l_arrH264Flag[2] = 0x00;
            l_arrH264Flag[3] = 0x01;
            l_arrH264Flag[4] = 0x68;
            fwrite(l_arrH264Flag,1,5,g_WriteFd);
            ls_bWriteFlag = true;
            break;

        }

        case H264_SPS:
        {
            l_arrH264Flag[0] = 0x00;
            l_arrH264Flag[1] = 0x00;    
            l_arrH264Flag[2] = 0x00;
            l_arrH264Flag[3] = 0x01;
            l_arrH264Flag[4] = 0x67;
            fwrite(l_arrH264Flag,1,5,g_WriteFd);
            ls_bWriteFlag = true;
            break;
        }
        
        default :
            break;
    }

    
    if((stRtpUnpack.stOutRTPPack.u32Paylen>0)&&(true==ls_bWriteFlag))
    {  
        //static unsigned int ls_u32Count = 0;
        //printf("u32SeqNum     :%d  ls_u32Count = %d  len = %d \n",stRtpUnpack.stOutRTPPack.u32SeqNum,ls_u32Count++,
        //    stRtpUnpack.stOutRTPPack.u32Paylen);
        fwrite(stRtpUnpack.stOutRTPPack.pu8Payload,1,stRtpUnpack.stOutRTPPack.u32Paylen,g_WriteFd);
    }
 
    return 0;
};

/******************************************************** 
Function: pstUnpackData	
Description: ����RTPЭ�����������յ���H264���ݰ�
Input:	*pstUnpackData
OutPut: *pstUnpackData��
Return: 
    0���������
    С��0����������
    0xff:���ݰ�����
Others: 
    1.RTP�������Ϣ�����������ȡ
    2.��Ƭ����֡�����������жϣ����������������h264��ͷ
Author: Caibiao Lee
Date:	2019-10-05
*********************************************************/
static int RTP_Client_UnPackH264Stream(RTP_UNPACK_S *pstUnpackData)
{
    unsigned char *l_pu8InputDataAddr=NULL;
    unsigned int l_s32InputDataLen = 0;
    RTP_PACKET_S *l_pstRTPPack   = NULL;
    RTP_HEADER_S *l_pstRTPHeader = NULL;
    RTP_NALU_HEADER_S *l_pstNaluHeader = NULL;
    RTP_FU_HEADER_S *l_pstFUHeader     = NULL;
    RTP_FU_INDICATOR_S *l_pstFUIndicator = NULL;
 
    
    /**RTP ��ͷ����Ϊ12�ֽ�**/
    if((NULL==pstUnpackData)||(NULL==pstUnpackData->pu8InputDataAddr)||(pstUnpackData->u32InputDataLen<12))
    {
        printf("%s %d input para error \n",__FUNCTION__,__LINE__);
        return -1;
    };

    l_pstRTPPack   = (RTP_PACKET_S*)&pstUnpackData->stOutRTPPack;
    l_pstRTPHeader = (RTP_HEADER_S*)&pstUnpackData->pu8InputDataAddr[0];

    /**RTP ��Ϣ��ȡ**/
    l_pstRTPPack->u8Version     = l_pstRTPHeader->bit1Version;
	l_pstRTPPack->u8Padding     = l_pstRTPHeader->bit1Padding;
	l_pstRTPPack->u8Extension   = l_pstRTPHeader->bit1Extension;
	l_pstRTPPack->u8Cc          = l_pstRTPHeader->bit4CsrcLen;
	l_pstRTPPack->u8Marker      = l_pstRTPHeader->bit1Marker;
	l_pstRTPPack->u8Pt          = l_pstRTPHeader->bit7PayLoadType;

	/**RTP ���к�**/
    l_pstRTPPack->u32SeqNum = 0;
    l_pstRTPPack->u32SeqNum = (pstUnpackData->pu8InputDataAddr[2] & 0xff);
    l_pstRTPPack->u32SeqNum <<= 8;
    l_pstRTPPack->u32SeqNum |= (pstUnpackData->pu8InputDataAddr[3] & 0xff);

	/**RTP ʱ���**/
    l_pstRTPPack->u32TimeStamp = (pstUnpackData->pu8InputDataAddr[4] & 0xff);
    l_pstRTPPack->u32TimeStamp <<= 8;
    l_pstRTPPack->u32TimeStamp |= (pstUnpackData->pu8InputDataAddr[5] & 0xff);
    l_pstRTPPack->u32TimeStamp <<= 8;
    l_pstRTPPack->u32TimeStamp |= (pstUnpackData->pu8InputDataAddr[6] & 0xff);
    l_pstRTPPack->u32TimeStamp <<= 8;
    l_pstRTPPack->u32TimeStamp |= (pstUnpackData->pu8InputDataAddr[7] & 0xff);

	/**RTP ͬ��ԴID**/
    l_pstRTPPack->u32Ssrc = (pstUnpackData->pu8InputDataAddr[8] & 0xff);
    l_pstRTPPack->u32Ssrc <<= 8;
    l_pstRTPPack->u32Ssrc |= (pstUnpackData->pu8InputDataAddr[9] & 0xff);
    l_pstRTPPack->u32Ssrc <<= 8;
    l_pstRTPPack->u32Ssrc |= (pstUnpackData->pu8InputDataAddr[10] & 0xff);
    l_pstRTPPack->u32Ssrc <<= 8;
    l_pstRTPPack->u32Ssrc |= (pstUnpackData->pu8InputDataAddr[11] & 0xff);
    
    l_pstNaluHeader = (RTP_NALU_HEADER_S*)&pstUnpackData->pu8InputDataAddr[12];

    pstUnpackData->u8OutNaluType = l_pstNaluHeader->bit5TYPE;

    /**��ʼ�������**/
    if (0==l_pstNaluHeader->bit5TYPE)
	{
		printf("%s %d ������д���0�޶��� \n",__FUNCTION__,__LINE__);
        return -2;
        
	}
    else if(0x06==l_pstNaluHeader->bit5TYPE)
    {
        /**H264��Ƶ֡��SEI**/
        pstUnpackData->eFrameType = H264_SEI;
        pstUnpackData->stOutRTPPack.pu8Payload = &pstUnpackData->pu8InputDataAddr[13];
        pstUnpackData->stOutRTPPack.u32Paylen = pstUnpackData->u32InputDataLen - 13;
        return 0;
        
    }else if(0x07==l_pstNaluHeader->bit5TYPE)
    {
        /**H264��Ƶ֡��SPS**/
        pstUnpackData->eFrameType = H264_SPS;
        pstUnpackData->stOutRTPPack.pu8Payload = &pstUnpackData->pu8InputDataAddr[13];
        pstUnpackData->stOutRTPPack.u32Paylen = pstUnpackData->u32InputDataLen - 13;

        return 0;
        
    }else if(0x08==l_pstNaluHeader->bit5TYPE)
    {
        /**H264��Ƶ֡��PPS**/
        pstUnpackData->eFrameType = H264_PPS;
        pstUnpackData->stOutRTPPack.pu8Payload = &pstUnpackData->pu8InputDataAddr[13];
        pstUnpackData->stOutRTPPack.u32Paylen = pstUnpackData->u32InputDataLen - 13;

        return 0;
        
    }
    else if (24==l_pstNaluHeader->bit5TYPE)                    
	{	
		/**STAP-A   ��һʱ�����ϰ�**/
		printf("��ǰ��ΪSTAP-A\n");
        return 0xff;
        
	}else if (25==l_pstNaluHeader->bit5TYPE)                   
	{
        /**STAP-B   ��һʱ�����ϰ�**/
        printf("��ǰ��ΪSTAP-B\n");
        return 0xff;
                
	}else if (26==l_pstNaluHeader->bit5TYPE)                    
	{
		/**MTAP16   ���ʱ�����ϰ�**/
		printf("��ǰ��ΪMTAP16\n");
        return 0xff;
        
	}else if (27==l_pstNaluHeader->bit5TYPE)                   
	{
		/**MTAP24   ���ʱ�����ϰ�**/
		printf("��ǰ��ΪMTAP24\n");
        return 0xff;
        
	}else if (28==l_pstNaluHeader->bit5TYPE)
    {
        unsigned char F;
        unsigned char NRI;
        unsigned char TYPE;
        unsigned char nh;
    
        /**FU-A��Ƭ��������˳��ʹ���˳����ͬ**/
        l_pstFUIndicator = (RTP_FU_INDICATOR_S *)&pstUnpackData->pu8InputDataAddr[12];
        l_pstFUHeader    = (RTP_FU_HEADER_S *)&pstUnpackData->pu8InputDataAddr[13];

        F    =  l_pstFUIndicator->BitF << 7;
        NRI  =  l_pstFUIndicator->BitNRI << 5;
        TYPE =  l_pstFUHeader->Bit5TYPE;    
        nh = F | NRI | TYPE;
        /**��Ƭ�����һ����**/
        if(1==l_pstRTPHeader->bit1Marker)
        {
            pstUnpackData->eFrameType = FU_END_PACK;
            pstUnpackData->stOutRTPPack.pu8Payload = &pstUnpackData->pu8InputDataAddr[14];
            pstUnpackData->stOutRTPPack.u32Paylen = pstUnpackData->u32InputDataLen - 14;
            return 0;
            
        }else if(0==l_pstRTPHeader->bit1Marker)/**��Ƭ�� ���������һ����**/
        {
            if (1==l_pstFUHeader->Bit1S)/**��Ƭ�ĵ�һ����**/ 
            {
                pstUnpackData->eFrameType = FU_START_PACK;

                /**ע���һ����Ҫ���֡����**/
                pstUnpackData->pu8InputDataAddr[14-1] = nh;
                //printf("biao debug Falg = 0x%x \n",nh);
                pstUnpackData->stOutRTPPack.pu8Payload = &pstUnpackData->pu8InputDataAddr[14-1];
                /**�����һ���ֽڣ�������Ҫ�޸ĳ��ȣ���Ȼ�Ứ��**/
                pstUnpackData->stOutRTPPack.u32Paylen = pstUnpackData->u32InputDataLen - 14 + 1;
                
                return 0;
                
            }else/**������ǵ�һ����,Ҳ�����м��**/
            {
                pstUnpackData->eFrameType = FU_MIDllE_PACK;
                pstUnpackData->stOutRTPPack.pu8Payload = &pstUnpackData->pu8InputDataAddr[14];
                pstUnpackData->stOutRTPPack.u32Paylen = pstUnpackData->u32InputDataLen - 14;

                return 0;
                
            }
        }
    }else if (29==l_pstNaluHeader->bit5TYPE)
    {
            /**FU-B��Ƭ��������˳��ʹ���˳����ͬ**/
            if (1==l_pstRTPHeader->bit1Marker)                  
            {   
                /**��Ƭ�����һ����**/
                printf("��ǰ��ΪFU-B��Ƭ�����һ����\n");
            
            }
            else if (0==l_pstRTPHeader->bit1Marker)             
            {
                /**��Ƭ�� ���������һ����**/
                printf("��ǰ��ΪFU-B��Ƭ��\n");
            }
    }else
    {
        printf("������д���\n");
    }

    return 0xff;
}

/******************************************************** 
Function:    RTP_Client_GetOnePacketData  
Description: ��ȡRTP�������ݰ�,���������Ҵ浽�ļ���ȥ
Input:  *pstRTPClient
OutPut: *pstRTPClient
Return: 0 �ɹ�����0ʧ��
Others: 
    1.�ú������ǻ�ȡһ��RTP��������ݣ�Ϊ�˲��ԣ�����ֱ��
      ǿ�ƻ�ȡ��Ҫ���Եİ���֮���ٷ��ء�
    2.RTP �������ݰ����Ϊ1500 �ֽڣ����Խ��ջ������Э������Ϊ1500
Author: Caibiao Lee
Date:   2019-10-05
*********************************************************/
int RTP_Client_GetOnePacketData(RTP_STATUS_S *pstRTPClient)
{
	int i = 0;
	int l_s32Ret = 0;
	int l_s32SocketFd = 0;
	unsigned char *l_pu8RcvBuf = NULL;
    RTP_UNPACK_S  l_stRtpUnpack = {0};
    RTP_UNPACK_S *l_pstRtpUnpack = &l_stRtpUnpack;

	if(NULL==pstRTPClient)
	{
		printf("%s %d input para error \n",__FUNCTION__,__LINE__);
		return -1;
	}
    
	l_s32SocketFd = pstRTPClient->s32RTPSockFd;

	if(0>=l_s32SocketFd)
	{
		printf("%s %d socket fd is close \n",__FUNCTION__,__LINE__);
		return -2;
	}

	l_pu8RcvBuf = (unsigned char*)malloc(RTP_RECV_DATA_LEN);
	if(NULL==l_pu8RcvBuf)
	{
		printf("%s %d malloc error \n",__FUNCTION__,__LINE__);
		return -3;
	}

    /**for debug**/
    if(NULL==g_WriteFd)
    {
        g_WriteFd = fopen("./data.h264","w+");
    };
    
    i=4000;
    while(i-->0)
    {
        bzero(l_pu8RcvBuf,RTP_RECV_DATA_LEN);
        l_s32Ret = NET_SocketRecvData(l_s32SocketFd,(void *)l_pu8RcvBuf,RTP_RECV_DATA_LEN); 
        if(l_s32Ret <= 0)
        {
            printf("%s %d :RTP Recv Data Error l_s32Ret = %d \n",__FUNCTION__,__LINE__,l_s32Ret);
            return -3;
        }

        l_pstRtpUnpack->pu8InputDataAddr = l_pu8RcvBuf;
        l_pstRtpUnpack->u32InputDataLen  = l_s32Ret;
        
        l_s32Ret = RTP_Client_UnPackH264Stream(l_pstRtpUnpack);
        if(0==l_s32Ret)
        {    
            RTP_Client_H264StreamStore(l_stRtpUnpack);

        }else if(0xff==l_s32Ret)
        {
            printf("%s %d Unknow data \n",__FUNCTION__,__LINE__);
        }else
        {
            printf("unpacket data error \n");
        }
    }
    
    return 0;
}

