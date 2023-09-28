#include "MessagePayload.hpp"

MessagePayload::MessagePayload(QObject *parent)
    : QObject{parent}
{

}

void MessagePayload::setPayload(const std::vector<uint8_t> &payload)
{
    m_payload = payload;
}

std::vector<uint8_t> MessagePayload::getPayload() const
{
    return m_payload;
}
