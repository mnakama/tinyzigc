#pragma once

#include <stdint.h>

#include "enums.h"

struct __attribute__((__packed__)) Frame {
    uint8_t SOF;
	uint8_t dataLen;
	enum Subsystem subsystem : 5; // L.O. bits come first
	enum Type type : 3;
	uint8_t cmdID;
	uint8_t data[];
};

// adapter/z-stack/znp/definition.ts
struct __attribute__((__packed__)) AF_dataRequest { // 1
	uint16_t dstAddr; // network address
	uint8_t dstEndpoint;
	uint8_t srcEndpoint;
	uint16_t clusterID;
	uint8_t transID;
	uint8_t options;
	uint8_t radius;
	uint8_t len;
	uint8_t data[];
};

// adapter/z-stack/znp/definition.ts
struct __attribute__((__packed__)) AF_dataRequestExt { // 2
	uint8_t dstAddrMode; // AddressMode
	uint64_t dstAddr; // IEEE address
	uint8_t dstEndpoint;
	uint16_t dstPanID;
	uint8_t srcEndpoint;
	uint16_t clusterID;
	uint8_t transID;
	uint8_t options;
	uint8_t radius;
	uint16_t len;
	uint8_t data[];
};

// adapter/z-stack/znp/definition.ts
struct __attribute__((__packed__)) AF_incomingMsg { // 129
	uint16_t groupID;
	uint16_t clusterID;
	uint16_t srcAddr;
	uint8_t srcEndpoint;
	uint8_t dstEndpoint;
	uint8_t wasBroadcast;
	uint8_t linkQuality;
	uint8_t securityUse;
	uint32_t timestamp;
	uint8_t transSeqNumber;
	uint8_t len;
	uint8_t data[];
};

// zspec/zcl/zclHeader.ts
struct __attribute__((__packed__)) ZclHeader {
	enum FrameType frameType : 2;
	uint8_t manufacturerSpecific : 1;
	enum FrameDirection direction : 1;
	uint8_t disableDefaultResponse : 1;
	uint8_t reserved : 3;
};

struct __attribute__((__packed__)) CommandReport { // 0x0a
	uint16_t attrID;
	uint8_t dataType;
	uint8_t data[];
};

// adapter/z-stack/znp/definition.ts
struct __attribute__((__packed__)) ZDO_srcRtgInd { // 196
	uint16_t dstAddr;
	uint8_t relayCount;
	uint16_t relayList; // what's a LIST_UINT16?

};

struct Device {
	uint64_t macAddress;
	uint16_t netAddress;
	int modelID;
	const char* name;
};
