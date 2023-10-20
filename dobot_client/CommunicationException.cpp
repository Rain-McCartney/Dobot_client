
#include "CommunicationException.hpp"

CommunicationException::CommunicationException(const QString &message)
    :m_message(message)

{

}

void CommunicationException::raise() const
{
    throw* this;
}

CommunicationException *CommunicationException::clone() const
{
    return new CommunicationException(*this);
}

QString CommunicationException::getMessage() const
{
    return m_message;
}
