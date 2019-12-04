/************************************************************
*Copyright (C),lcb0281at163.com lcb0281atgmail.com
*FileName: rtp_client.h
*BlogAddr: https://blog.csdn.net/li_wen01
*Description: RTP Э�� 
*Date:	   2019-10-05
*Author:   Caibiao Lee
*Version:  V1.0
*Others:
*History:
***********************************************************/
#ifndef __RTP_CLIENT_H__
#define __RTP_CLIENT_H__

#include "common.h"
      

/*****************************************************************************************
                        RTP��׼�ṹ�嶨�忪ʼ
******************************************************************************************/


/***************************************************************
0                   1                   2                   3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|V=2|P|X|  CC   |M|     PT      |       sequence number         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                           timestamp                           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|           synchronization source (SSRC) identifier            |
+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
|            contributing source (CSRC) identifiers             |
|                             ....                              |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*****************************************************************/ 
/**RTP ͷ�ṹ**/
typedef struct 
{
    /** byte 0 **/
    unsigned char bit4CsrcLen:4;        /** expect 0 **/
    unsigned char bit1Extension:1;      /** expect 1, see RTP_OP below **/
    unsigned char bit1Padding:1;        /** expect 0 **/
    unsigned char bit1Version:2;        /** expect 2 **/
    /** byte 1 **/
    unsigned char bit7PayLoadType:7;    /** RTP_PAYLOAD_RTSP **/
    unsigned char bit1Marker:1;         /** expect 1 **/
    /** bytes 2,3 **/
    unsigned int u32SeqNum;             /** RTP sequence number **/     
    /** bytes 4-7 **/
    unsigned int u32TimeStamp;          /** RTP sequence number **/
    /** bytes 8-11 **/
    unsigned int u32Ssrc;               /**stream number is used here **/
}RTP_HEADER_S;

/***************** 
+---------------+
|0|1|2|3|4|5|6|7|
+-+-+-+-+-+-+-+-+
|F|NRI|  Type   |
+---------------+
*****************/
/**�������㵥Ԫͷ�ṹ��**/
typedef struct 
{
    unsigned char bit5TYPE:5;
    unsigned char bit2NRI:2;
    unsigned char bit1F:1;        
}RTP_NALU_HEADER_S;

/****************
+---------------+
|0|1|2|3|4|5|6|7|
+-+-+-+-+-+-+-+-+
|F|NRI|  Type   |
+---------------+
*****************/
/**��Ƭ��ָʾ��**/
typedef struct 
{
	unsigned char Bit5TYPE:5;
	unsigned char BitNRI:2; 
	unsigned char BitF:1;              
}RTP_FU_INDICATOR_S; 


/******************
+---------------+
|0|1|2|3|4|5|6|7|
+-+-+-+-+-+-+-+-+
|S|E|R|  Type   |
+---------------+
*******************/
/**��Ƭ��ͷ�ṹ**/
typedef struct 
{
	unsigned char Bit5TYPE:5;
	unsigned char Bit1R:1;
	unsigned char Bit1E:1;
	unsigned char Bit1S:1;    
}RTP_FU_HEADER_S;



/**��ϰ�ͷ���Ƚṹ��**/
typedef struct 
{   
    unsigned short u16AUHeaderLen;
}RTP_AU_HEADER_LEN_S;


/**��ϰ�ͷ�ṹ��**/
typedef struct 
{   
    unsigned short u16AUHeader;
}RTP_AU_HEADER_S;


/*****************************************************************************************
                        RTP��׼�ṹ�嶨�����
******************************************************************************************/



/*****************************************************************************************
                        �Զ���ṹ�忪ʼ
******************************************************************************************/
/**RTP֡����**/
typedef enum 
{
    NAL_PACK = 0,    /**����**/
    AP_PACK,         /**��ϰ�**/
    FU_START_PACK,   /**��Ƭ����ʼ**/
    FU_MIDllE_PACK,  /***��Ƭ���м��*/
    FU_END_PACK,     /**��Ƭ��������**/
    H264_SEI,
    H264_PPS,
    H264_SPS,
    AUDIO_AAC,
}RTP_FRAME_TYPE; 

/**RTP ���ݰ��ṹ��**/
typedef struct
{
    unsigned char u8Version;      /**Version, 2 bits, MUST be 0x2**/
    unsigned char u8Padding;	  /**Padding bit, Padding MUST NOT be used**/
    unsigned char u8Extension;    /**Extension, MUST be zero**/
    unsigned char u8Cc;       	  /**CSRC count, normally 0 in the absence of RTP mixers**/ 
    unsigned char u8Marker;	      /**Marker bit**/
    unsigned char u8Pt;			  /**7 bits, Payload Type, dynamically established**/
    unsigned int  u32SeqNum;	  /**RTP sequence number, incremented by one for each sent packet**/
    unsigned int  u32TimeStamp;	  /**timestamp, 27 MHz for H.264**/
    unsigned int  u32Ssrc;        /**Synchronization Source, chosen randomly**/
    unsigned char * pu8Payload;   /**the payload including payload headers**/
    unsigned int  u32Paylen;      /**length of payload in bytes**/
}RTP_PACKET_S;


/**RTP ���ݽ��**/
typedef struct 
{
    unsigned char *pu8InputDataAddr;  /**�������ݰ���ַ**/
    unsigned int   u32InputDataLen;   /**�������ݰ�����**/
    RTP_FRAME_TYPE eFrameType;        /**������ݰ�����**/
    unsigned char  u8OutNaluType;     /***SPS PPS SEI������**/
    RTP_PACKET_S   stOutRTPPack;      /**���RTP���ݰ����������Ϣ**/
}RTP_UNPACK_S;

/**RTP �ͻ������ݽ������**/
typedef struct
{
    unsigned char u8Count[2];          /**���кż�����ͳ��packNum ����Ĵ���**/
    unsigned char u8PacketNum[2];      /**��������**/
    unsigned char u8Lost;              /**��������**/
    unsigned int  u32LostPackNum;      /**������**/
}RTP_RECV_INFO_S;

/*****************************************************************************************
                                 �Զ���ṹ�����
******************************************************************************************/
int RTP_Client_Init(RTP_STATUS_S *pstRTPClient);
int RTP_Client_Release(RTP_STATUS_S *pstRTPClient);
int RTP_CLient_Session(RTP_STATUS_S *pstRTPClient);
int RTP_Client_GetOnePacketData(RTP_STATUS_S *pstRTPClient);


#endif



