#ifndef ROCKET_NET_CODER_TINYPB_CODER_H
#define ROCKET_NET_CODER_TINYPB_CODER_H

#include "rocket/net/coder/abstract_coder.h"
#include "rocket/net/coder/tiny_pb_protocol.h"
namespace rocket
{
class TinyPBCoder :public AbstracstCoder {

public:
TinyPBCoder() {}
~TinyPBCoder() {}
void encode(std::vector<AbstractProtocol::s_ptr>& messages, TcpBuffer::s_ptr out_buffer);

void decode(std::vector<AbstractProtocol::s_ptr>& out_messages, TcpBuffer::s_ptr buffer);

private:
  const char* encodeTinyPB(std::shared_ptr<TinyPBProtocol> message, int& len);

};



} // namespace rocket





#endif