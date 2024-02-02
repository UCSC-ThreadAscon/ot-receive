#include <stdio.h>

#include "ot_receive.h"
#include "common.c"

#define MAX_CHARS 22

otError udpAttachPayload(otMessage *aMessage) {
  static int count = 0;

  char* payload = calloc(MAX_CHARS, sizeof(char));
  sprintf(payload, "Packet Number %d", count);
  count += 1;

  otError error = otMessageAppend(aMessage, payload, MAX_CHARS * sizeof(char));
  free(payload);
  return error;
}

void udpSend(otInstance *aInstance,
             uint16_t port,
             uint16_t destPort,
             otUdpSocket *aSocket,
             otMessageInfo *aMessageInfo) {
  otMessage *aMessage = otUdpNewMessage(aInstance, NULL);

  otError error = udpAttachPayload(aMessage);
  handleMessageError(aMessage, error);

  error = otUdpSend(aInstance, aSocket, aMessage, aMessageInfo);
  handleMessageError(aMessage, error);

  DEBUG_PRINT(otLogNotePlat("UDP packet successfully sent."));
  vTaskDelay(PACKET_SEND_DELAY);
  return;
}

void udpSendInfinite(otInstance *aInstance, uint16_t port, uint16_t destPort) {
  checkConnection(aInstance);

  otSockAddr aSockName;
  aSockName.mAddress = *otThreadGetMeshLocalEid(aInstance);
  aSockName.mPort = port;
  otUdpSocket *aSocket = udpCreateSocket(aInstance, port, &aSockName);

  otMessageInfo aMessageInfo;
  aMessageInfo.mSockAddr = *otThreadGetMeshLocalEid(aInstance);
  aMessageInfo.mSockPort = port;
  aMessageInfo.mPeerPort = destPort;
  aMessageInfo.mLinkInfo = NULL;
  aMessageInfo.mHopLimit = 0;  // default
  otIp6Address *peerAddr = &(aMessageInfo.mPeerAddr);
  handleError(otIp6AddressFromString(MLEID_MULTICAST, peerAddr));

  while (true) {
    udpSend(aInstance, port, destPort, aSocket, &aMessageInfo);
  }
  return;
}