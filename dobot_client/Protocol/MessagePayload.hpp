
#pragma once
#include <memory>

#include <QObject>

class MessagePayload : public QObject
{
    Q_OBJECT
public:
    explicit MessagePayload(QObject *parent = nullptr);

    void setPayload(const std::vector<uint8_t>& payload);

    std::vector<uint8_t> getPayload() const;


private:

    std::vector<uint8_t> m_payload;

};

typedef std::shared_ptr<MessagePayload> MessagePayloadSmartPtr;
