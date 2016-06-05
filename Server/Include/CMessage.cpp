#include "CMessage.h"

namespace IslandSurvival{

CMessage::CMessage()
{
	m_proto = NULL;
    m_head.m_message_hash = 0;
}

CMessage::~CMessage()
{
	if(NULL != m_proto)
	{
		delete m_proto;
        m_proto = NULL;
	}
}


CMessage::CMessage(const CMessage& mess)
{
    m_head = mess.m_head;
    m_proto = mess.m_proto->New();
    m_proto->CopyFrom(*(mess.m_proto));
}

CMessage& CMessage::operator=(const CMessage& mess)
{
    if(this == &mess)
    {
        return *this;
    }
    this->m_head = mess.m_head;
    this->m_proto = mess.m_proto->New();
    this->m_proto->CopyFrom(*(mess.m_proto));
    return *this;
}

#define Append(x) do{ \
	int32_t size = sizeof(x); \
    if(size == 2) \
    { \
        uint16_t v = htons(x); \
        memcpy(Outs, &(v), size); \
    } \
    else if(size == 4)\
    { \
        uint32_t v = htonl(x); \
        memcpy(Outs, &(v), size); \
    } \
    else \
    { \
        memcpy(Outs, &(x), size); \
    } \
	Outs += size; \
	OutLen += size; \
}while(0)

void CMessageHead::encode(char *Outs, int32_t &OutLen)
{
	OutLen = 0;
	Append(m_packetlen);
	Append(m_message_id);
    Append(m_framenum);
    Append(m_message_hash);
}
#undef Append

#define Append(x) do{ \
    int32_t size = sizeof(x); \
	if(size == 2) \
    { \
        uint16_t v; \
        memcpy(&v, Ins, size); \
        x = ntohs(v); \
    } \
    else if(size == 4)\
    { \
        uint32_t v; \
        memcpy(&v, Ins, size); \
        x = ntohl(v); \
    } \
    else \
    { \
        memcpy(&x, Ins, size); \
    } \
	Ins += size; \
}while(0)

int CMessageHead::decode(const char *Ins, const int32_t InLen)
{
	if(InLen != Size())
	{
		return fail;
	}
	Append(m_packetlen);
	Append(m_message_id);
    Append(m_framenum);
    Append(m_message_hash);
	return success;
}
#undef Append

void CMessage::encode(char *Outs, int32_t &OutLen)
{
    OutLen = 0;
    if(NULL == m_proto)
    {
        PRINT_LOG("CMessage encode error , m_proto is NULL\n");
        return;
    }
    m_head.m_packetlen = 0;
	std::string code = "";
    m_proto->SerializeToString(&code);
	m_head.m_packetlen += code.length() + m_head.Size();

	m_head.encode(Outs, OutLen);
	memcpy(&Outs[OutLen], code.c_str(), code.length());

	OutLen += code.length();
}

int32_t CMessage::decode(const char *Ins, const int32_t InLen)
{
	if(NULL == m_proto || NULL == Ins || InLen < m_head.Size()) 
	{
        PRINT_LOG("decode error, m_proto = %p, Ins = %p, InLen = %d\n", m_proto, Ins, InLen);
		return fail;
	}
	m_head.decode(Ins, m_head.Size());
	if(InLen != m_head.m_packetlen)
	{
        PRINT_LOG("InLen != m_head.m_packetlen\n");
		return fail;
	}
	std::string code = "";
	code.assign(&Ins[m_head.Size()], m_head.m_packetlen - m_head.Size());
    m_proto->ParseFromString(code);
	return success;
}


}