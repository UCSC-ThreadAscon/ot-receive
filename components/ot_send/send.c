#include "ot_send.h"

#include <openthread/ping_sender.h>

/**
 * @file
 *  This file defines the functions used for the sender to send
 *  packets to a given destination, at repeated intervals.
*/

#define MLEID "fdc2:53d3:bb7e:2437:114d:c3da:9918:d5e4"

void start_ping(otInstance *aInstance) {
  otDeviceRole currentRole;
  do {
    currentRole = otThreadGetDeviceRole(aInstance);
    vTaskDelay(DEFAULT_WAIT_TIME);
  } while(
    (currentRole == OT_DEVICE_ROLE_DISABLED) ||
    (currentRole == OT_DEVICE_ROLE_DETACHED)
  );

  ping(aInstance);
  return;
}

void ping(otInstance *aInstance) {
  otPingSenderConfig aConfig;

  aConfig.mSource = *otThreadGetMeshLocalEid(aInstance);

  otIp6Address *destAddr = &(aConfig.mDestination);
  otIp6AddressFromString(MLEID, destAddr);

  aConfig.mTimeout = 100; // ms
  aConfig.mMulticastLoop = true;

  // Default settings
  aConfig.mReplyCallback = NULL;
  aConfig.mStatisticsCallback = NULL;
  aConfig.mCallbackContext = NULL;
  aConfig.mSize = 0;
  aConfig.mCount = 0;
  aConfig.mInterval = 0;
  aConfig.mTimeout = 100; // ms
  aConfig.mHopLimit = 0;

  ESP_ERROR_CHECK(otPingSenderPing(aInstance, &aConfig));
  return;
};