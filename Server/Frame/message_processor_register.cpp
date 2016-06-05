#include "message_processor_register.h"

namespace IslandSurvival {

MessageProcessorRegister* MessageProcessorRegister::m_pInstance = NULL;

MessageProcessorRegister* MessageProcessorRegister::Instance()
{
    if(NULL == m_pInstance)
    {
        m_pInstance = new MessageProcessorRegister;
    }
    return m_pInstance;
}

int32_t MessageProcessorRegister::RegistMessage(int32_t id, ::google::protobuf::Message* pb)
{
    if(NULL == pb || m_proto_type.find(id) != m_proto_type.end())
    {
        if(NULL == pb)
        {
            PRINT_LOG("regist message error, pb is NULL\n");
        }
        if(m_proto_type.find(id) != m_proto_type.end())
        {
            PRINT_LOG("regist message error, this id(%d) already exist\n", id);
        }
        return fail;
    }
    m_proto_type[id] = pb;
    return success;
}

int32_t MessageProcessorRegister::RegistProcessor(int32_t id, MessageProcessorBase * processor)
{
    if(NULL == processor || m_processor.find(id) != m_processor.end())
    {
        if(NULL == processor)
        {
            PRINT_LOG("RegistProcessor error, processor is NULL\n");
        }
        if(m_processor.find(id) != m_processor.end())
        {
            PRINT_LOG("RegistProcessor error, this id(%d) already exist\n", id);
        }
        return fail;
    }
    m_processor[id] = processor;
    return int32_t(success);
}

::google::protobuf::Message* MessageProcessorRegister::CreateMessage(int32_t id)
{
    if(m_proto_type.find(id) == m_proto_type.end())
    {
        PRINT_LOG("CreateMessage error , message id(%d) not registed!\n", id);
        return NULL;
    }
    return m_proto_type[id]->New();
}

int32_t MessageProcessorRegister::ProcessRequest(int32_t user_id, CMessage * message)
{
    if(NULL == message)
    {
        PRINT_LOG("ProcessRequest error , message is NULL\n");
        return fail;
    }
    int32_t ret = m_processor[message->m_head.m_message_id]->ProcessRequest(user_id, *message);
    return int32_t(ret);
}


}