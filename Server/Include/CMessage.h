#pragma once
#ifndef _hello_CMESSAGE_H__
#define _hello_CMESSAGE_H__

#include "google/protobuf/message.h"
#include "server_base.h"

namespace IslandSurvival{

enum
{
	not_defined_messagetype = 0,
	not_complete_message = 1,
	decode_success = 2,
};

// 消息头结构体
struct CMessageHead
{
    // 消息头的编码
	void encode(char *Outs, int32_t &OutLen);
    // 消息头的解码
	int32_t decode(const char *Ins, const int32_t InLen);
    //返回消息头的大小
	int32_t Size() 
	{
        static int32_t head_size = sizeof(m_packetlen)
                                 + sizeof(m_message_id)
                                 + sizeof(m_framenum)
                                 + sizeof(m_message_hash);
        return head_size;
	}

	int32_t m_packetlen;       // 消息长度，这个一定放第一位
	int32_t m_message_id;      // 协议编号，这个放第二位， 否则解码会出错
    int32_t m_framenum;        // 帧序号
    uint32_t m_message_hash;   // 哈希验证码
};

// 消息类
struct CMessage
{

	CMessage();
	~CMessage();

    CMessage(const CMessage& mess);
    // 重载拷贝函数，实现深拷贝
    CMessage& operator=(const CMessage& mess);  
    // 编码
	void encode(char *Outs, int32_t &OutLen);
    // 解码
	int32_t decode(const char *Ins, const int32_t InLen);

	CMessageHead m_head;
	::google::protobuf::Message* m_proto;  
    // m_proto的析构由CMessage管理，所以最好用new出来的proto给m_proto赋值，否则会出问题
};

}

#endif
