/************************************************************
*Copyright (C),lcb0281at163.com lcb0281atgmail.com
*FileName: LinkListQueue.c
*BlogAddr: https://blog.csdn.net/li_wen01
*Description: ������ʵ��һ�����еĹ���
*Date:	   2019-06-22
*Author:   Caibiao Lee
*Version:  V1.1
*Others:
*History:
***********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "linklistqueue.h" 

pthread_mutex_t g_stLinkListMutex[LINK_LIST_MAX_COUNT];
static unsigned char gs_LinkListUseFlag[LINK_LIST_MAX_COUNT]={0};

int LinkListQueue_InitMutex(unsigned char u8QueueNum)
{
	if(u8QueueNum>=LINK_LIST_MAX_COUNT)
	{
		printf("%s %d input para  %d error \n",__FUNCTION__,__LINE__,u8QueueNum);
		return -1;	
	}

	if(0==gs_LinkListUseFlag[u8QueueNum])
	{
		printf("%s %d Link is not in use \n",__FUNCTION__,__LINE__);
		return -2;
	}
	
    pthread_mutex_init(&g_stLinkListMutex[u8QueueNum],NULL);

	return 0;
}

int LinkListQueue_ReleaseMutex(unsigned char u8QueueNum)
{
	if(u8QueueNum>=LINK_LIST_MAX_COUNT)
	{
		printf("%s %d input para  %d error \n",__FUNCTION__,__LINE__,u8QueueNum);
		return -1;	
	}

	if(0==gs_LinkListUseFlag[u8QueueNum])
	{
		printf("%s %d Link is not in use \n",__FUNCTION__,__LINE__);
		return -2;
	}

    pthread_mutex_destroy(&g_stLinkListMutex[u8QueueNum]);

	return 0;
}

int LinkListQueue_Lock(unsigned char u8QueueNum)
{
	if(u8QueueNum>=LINK_LIST_MAX_COUNT)
	{
		printf("%s %d input para  %d error \n",__FUNCTION__,__LINE__,u8QueueNum);
		return -1;	
	}

	if(0==gs_LinkListUseFlag[u8QueueNum])
	{
		printf("%s %d Link is not in use \n",__FUNCTION__,__LINE__);
		return -2;
	}

	pthread_mutex_lock(&g_stLinkListMutex[u8QueueNum]);

	return 0;
}

int LinkListQueue_Unlock(unsigned char u8QueueNum)
{
	if(u8QueueNum>=LINK_LIST_MAX_COUNT)
	{
		printf("%s %d input para  %d error \n",__FUNCTION__,__LINE__,u8QueueNum);
		return -1;	
	}

	if(0==gs_LinkListUseFlag[u8QueueNum])
	{
		printf("%s %d Link is not in use \n",__FUNCTION__,__LINE__);
		return -2;
	}

	pthread_mutex_unlock(&g_stLinkListMutex[u8QueueNum]);

	return 0;
}

/******************************************************** 
Function: LinkListQueue_Create	
Description: ����������
Input:	
	s32Capacity ָ�������Ա�Ĵ�С
	u8QueueNum  ��Ҫ�����Ķ��к�
OutPut: none
Return: LIST_QUEUE_S*  ���ش�����������
Others: 
Author: Caibiao Lee
Date:	2019-09-28
*********************************************************/
LIST_QUEUE_S *LinkListQueue_Create(int s32Capacity,unsigned char u8QueueNum)
{
	LIST_QUEUE_S *pstListQueue = NULL;

	if((s32Capacity<=0)||(u8QueueNum>=LINK_LIST_MAX_COUNT))
	{
		printf("%s %d input para error \n",__FUNCTION__,__LINE__);
		return NULL;
	}

	if(1==gs_LinkListUseFlag[u8QueueNum])
	{
		printf("%s %d LinkList Num=%d is already in use \n",
			__FUNCTION__,__LINE__,u8QueueNum);
		return NULL;
	}
	
	pstListQueue=(LIST_QUEUE_S *)malloc(sizeof(LIST_QUEUE_S));
	if(NULL!=pstListQueue)
	{
		pstListQueue->s32Length = 0;
		pstListQueue->s32Capacity = s32Capacity;
		gs_LinkListUseFlag[u8QueueNum] = 1;
		return pstListQueue;
	}
	else
	{
		return NULL;
	}
	
}

/******************************************************** 
Function: LinkListQueue_Destroy	
Description: ���������
Input:	pstListQueue ������
OutPut: none
Return: 
Others: 0 �ɹ�����0 ʧ��
Author: Caibiao Lee
Date:	2019-09-28
*********************************************************/
int LinkListQueue_Destroy(LIST_QUEUE_S *pstListQueue)
{
	int i =0 ;
	LINK_DATA_S l_stLinkData;
	
	if(NULL==pstListQueue)
	{
		printf("%s %d input para error \n",__FUNCTION__,__LINE__);
		return -1;
	}

	if(0==gs_LinkListUseFlag[pstListQueue->s8QueueNum])
	{
		printf("%s %d Link List Num=%d is not in use \n",
			__FUNCTION__,__LINE__,pstListQueue->s8QueueNum);
		return -2;
	}

	for(i=0;i<pstListQueue->s32Length;i++)
	{
		LinkListQueue_Output(pstListQueue,&l_stLinkData);
	}
	
	gs_LinkListUseFlag[pstListQueue->s8QueueNum] = 0;
	
	return 0;
} 
 
/******************************************************** 
Function: LinkListQueue_Input	
Description: Ԫ�ؽ�������
Input:	
	*pstListQueue ������
	stLinkData	  ����
OutPut: *pstListQueue
Return:
Others: 
Author: Caibiao Lee
Date:	2019-09-28
*********************************************************/
int LinkListQueue_Input(LIST_QUEUE_S *pstListQueue, LINK_DATA_S stLinkData)
{
	QUEUE_NODE_S *l_pstQueueNode = NULL;
	
	if(NULL==pstListQueue)
	{
		printf("%s %d input para error \n",__FUNCTION__,__LINE__);
		return -1;
	}
	
	l_pstQueueNode=(QUEUE_NODE_S *)malloc(sizeof(QUEUE_NODE_S));
	if(NULL==l_pstQueueNode)
	{
		printf("%s %d malloc error \n",__FUNCTION__,__LINE__);
		return -2;
	}
	
	memcpy(&l_pstQueueNode->stData,&stLinkData,sizeof(LINK_DATA_S));

	l_pstQueueNode->stNext=NULL;
	
	LinkListQueue_Lock(pstListQueue->s8QueueNum);
	
	if(pstListQueue->stRear==NULL)
	{	
	    /**�����һ�β���������ͷָ���βָ��Ϊl_pstQueueNode**/
		pstListQueue->stFront=pstListQueue->stRear=l_pstQueueNode;
	}else
	{
		/**�����е�β������l_pstQueueNode**/
		pstListQueue->stRear->stNext=l_pstQueueNode;

		/**���������е�βָ��ָ��l_pstQueueNode**/
		pstListQueue->stRear=l_pstQueueNode;        
	}
	pstListQueue->s32Length++;
	
	LinkListQueue_Unlock(pstListQueue->s8QueueNum);
	
	return 0;
}
 
/******************************************************** 
Function: LinkListQueue_Output	
Description: Ԫ�س�������
Input:	
	*pstListQueue ������
	*stLinkData	  ����
OutPut: *pstListQueue��*pstLinkData
Return: 0 �ɹ�; ��0 ʧ��
Others: 
Author: Caibiao Lee
Date:	2019-09-28
*********************************************************/
int LinkListQueue_Output(LIST_QUEUE_S *pstListQueue,LINK_DATA_S *pstLinkData)
{
	QUEUE_NODE_S *l_pstQueueNode=NULL;
	
	if((NULL==pstListQueue)||(NULL==pstLinkData))
	{
		printf("%s %d input para error \n",__FUNCTION__,__LINE__);
		return -1;
	};

	l_pstQueueNode = pstListQueue->stFront;
	if((NULL==pstListQueue->stRear)||(NULL==pstListQueue->stFront))
	{
		printf("%s %d error\n",__FUNCTION__,__LINE__);
		return -2;
	}

	LinkListQueue_Lock(pstListQueue->s8QueueNum);
	if(pstListQueue->stFront==pstListQueue->stRear)
	{
		pstListQueue->stFront=pstListQueue->stRear=NULL;
	}else
	{
		pstListQueue->stFront=pstListQueue->stFront->stNext;
	}

	pstListQueue->s32Length--;

	LinkListQueue_Unlock(pstListQueue->s8QueueNum);

	*pstLinkData=l_pstQueueNode->stData;
	memcpy((unsigned char*)pstLinkData,&l_pstQueueNode->stData,sizeof(LINK_DATA_S));
	free(l_pstQueueNode);
	
	return 0;
	
}
 
/******************************************************** 
Function: LinkListQueue_Printf	
Description: ��ӡ���������
Input:	
	*pstListQueue ������
OutPut: *pstListQueue
Return:
Others: ���ڲ���������ʹ��
Author: Caibiao Lee
Date:	2019-09-28
*********************************************************/
int LinkListQueue_Printf(LIST_QUEUE_S *pstListQueue)
{
	LIST_QUEUE_S l_stQueueNode ;
 
 	if(NULL==pstListQueue)
	{
		printf("%s %d input para error \n",__FUNCTION__,__LINE__);
		return -1;
	};

	LinkListQueue_Lock(pstListQueue->s8QueueNum);
	l_stQueueNode.stFront = pstListQueue->stFront;
 
	if(pstListQueue->stFront==NULL || pstListQueue->stRear==NULL)
	{
		return -2;
	}
	
	while(pstListQueue->stFront!=NULL)
	{
		printf("%d\n",pstListQueue->stFront->stData.u32Len);
		pstListQueue->stFront=pstListQueue->stFront->stNext;
	}
	pstListQueue->stFront = l_stQueueNode.stFront;

	LinkListQueue_Unlock(pstListQueue->s8QueueNum);

	return 0;
}


int LinkListQueue_Debug(void)
{
	int l_s32Capacity = 0;
	unsigned char l_u8QueueNum = 0;
	LINK_DATA_S stTestData;	
	LIST_QUEUE_S* pstListQueue = NULL;

	l_s32Capacity = 20;
	l_u8QueueNum = 0;

	/**����������**/
	pstListQueue = LinkListQueue_Create(l_s32Capacity,l_u8QueueNum);
	if(NULL==pstListQueue)
	{
		printf("%s %d create link list error \n",__FUNCTION__,__LINE__);
		return -1;
	}

	/**����:�ն������**/
	LinkListQueue_Output(pstListQueue,&stTestData);	
	printf("Out Put Data = %d  s32Capacity=%d s32Length=%d \n",stTestData.u32Len,
		pstListQueue->s32Capacity,pstListQueue->s32Length);

	/**���Ԫ�ص�������ȥ**/
	for(int i=0;i<10;i++)
	{
		stTestData.u32Len = i;
		LinkListQueue_Input(pstListQueue,stTestData);
	}
	
	/**��ӡ��������������**/
	LinkListQueue_Printf(pstListQueue);

	/**������һ��Ԫ��**/
	LinkListQueue_Output(pstListQueue,&stTestData);	
	printf("Out Put Data = %d  s32Capacity=%d s32Length=%d \n",stTestData.u32Len,
		pstListQueue->s32Capacity,pstListQueue->s32Length);

	/**������һ��Ԫ��**/
	LinkListQueue_Output(pstListQueue,&stTestData);	
	printf("Out Put Data = %d  s32Capacity=%d s32Length=%d \n",stTestData.u32Len,
		pstListQueue->s32Capacity,pstListQueue->s32Length);

	/**������һ��Ԫ��**/
	LinkListQueue_Output(pstListQueue,&stTestData);	
	printf("Out Put Data = %d  s32Capacity=%d s32Length=%d \n",stTestData.u32Len,
		pstListQueue->s32Capacity,pstListQueue->s32Length);

	/**�������һ��Ԫ��**/
	stTestData.u32Len = 100;
	LinkListQueue_Input(pstListQueue,stTestData);

	/**��ӡ��������������**/
	LinkListQueue_Printf(pstListQueue);

	/**���Դ���һ���Ѿ����ڵĶ���**/
	LinkListQueue_Create(l_s32Capacity,l_u8QueueNum);

	/**�ͷŶ�����Դ**/
	LinkListQueue_Destroy(pstListQueue);
 
	return 0;
}

 
