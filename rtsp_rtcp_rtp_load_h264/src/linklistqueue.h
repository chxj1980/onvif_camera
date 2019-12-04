/************************************************************
*Copyright (C),lcb0281at163.com lcb0281atgmail.com
*FileName: linklistqueue.h
*BlogAddr: https://blog.csdn.net/li_wen01
*Description: ������ʵ��һ�����еĹ���
*Date:	   2019-09-28
*Author:   Caibiao Lee
*Version:  V1.1
*Others:
*History:
***********************************************************/
#ifndef __LINK_LIST_QUEUE_H__
#define __LINK_LIST_QUEUE_H__

#define LINK_LIST_MAX_COUNT		60  /**���֧����������**/


/**�洢����**/
typedef struct link_data  
{
    unsigned int   u32Len;        /**���ݳ���**/
    unsigned char *pu8DataAddr;   /**���ݴ�ŵ�ַ**/
}__attribute__ ((packed))LINK_DATA_S;

/**�����н��ṹ**/
typedef struct queue_node
{
	LINK_DATA_S stData;           /**���ݽڵ�**/
	struct QUEUE_NODE_S *stNext;  /**��һ�����ݽڵ�**/
}QUEUE_NODE_S;
 
/**�����нṹ**/
typedef struct list_queue
{
	int s32Length;              /**���Ա��ʵ��ʹ�ó���**/
	int s32Capacity;            /**�����Ա����󳤶�**/
	unsigned char s8QueueNum;	/**���к�**/
	QUEUE_NODE_S *stFront;      /**����ͷ**/ 
	QUEUE_NODE_S *stRear;       /**����β**/
}LIST_QUEUE_S;

LIST_QUEUE_S *LinkListQueue_Create(int s32Capacity,unsigned char u8QueueNum);
int LinkListQueue_Input(LIST_QUEUE_S *pstListQueue, LINK_DATA_S stLinkData);
int LinkListQueue_Output(LIST_QUEUE_S *pstListQueue,LINK_DATA_S *pstLinkData);
int LinkListQueue_Destroy(LIST_QUEUE_S *pstListQueue);

#endif
