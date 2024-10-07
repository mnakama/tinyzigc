#include <err.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/param.h>

#include "enums.h"
#include "structs.h"
#include "devices.h"

const uint8_t SOF = 0xfe; // start of frame
uint8_t TransactionID = 0;
uint8_t TransactionSeq = 0;

const char* clusterName(enum Cluster clusterID) {
	switch (clusterID) {
	case genBasic:
		return "genBasic";
	case genMultistateInput:
		return "genMultistateInput";
	case genOTA:
		return "genOTA";
	case greenPower:
		return "greenPower";
	case touchlink:
		return "touchlink";
	default:
		return "unknown";
	}
}

const char* dataTypeName(enum DataType dataType) {
	switch (dataType) {
	case UINT8:
		return "UINT8";
	case UINT16:
		return "UINT16";
	case CHAR_STR:
		return "CHAR_STR";
	default:
		return "unknown";
	}
}

int dataTypeSize(enum DataType dataType, const void* data) {
	switch (dataType) {
	case UINT8:
		return 1;
	case UINT16:
		return 2;
	case CHAR_STR:
		return *((uint8_t*)data);
	default:
		return -1;
	}
}

void dataTypePrint(enum DataType dataType, const void* data) {
	switch (dataType) {
	case UINT8:
		printf("0x%02hhx", *((uint8_t*)data));
		break;
	case UINT16:
		printf("0x%04hx", *((uint16_t*)data));
		break;
	case CHAR_STR: {
		uint8_t* ch = (uint8_t*)data;
		int len = *ch;
		ch++;
		uint8_t* end = ch + len - 1;
		while (ch <= end) {
			// should be utf8
			putchar(*(ch++));
		}
		break;
	}
	default:
		printf("?");
	}
}

uint8_t* writeUint8(uint8_t* dest, uint8_t value) {
	*(dest++) = value;
	return dest;
}
uint16_t* writeUint16(uint16_t* dest, uint16_t value) {
	*(dest++) = value;
	return dest;
}

struct Device* getDeviceByNetAddress(uint16_t netAddr) {
	for (size_t i=0; i < LENGTH(devices); i++) {
		if (devices[i].netAddress == netAddr) return &devices[i];
	}

	return NULL;
}

uint8_t checksumFrame(const struct Frame* frame) {
	uint8_t checksum = 0;
	for (const uint8_t *c = frame->data - 3; c < frame->data+frame->dataLen; c++) {
		checksum ^= *c;
	}

	return checksum;
}

void printFrame(const struct Frame* frame) {
	printf("SOF: 0x%hhx\ndataLen: %hhu\ntype: %hhu\nsubsystem: %hhu\ncmdID: %hhu\n",
		   frame->SOF, frame->dataLen, frame->type, frame->subsystem, frame->cmdID);
}

void sendFrame(struct Frame* frame, uint8_t* end) {
	frame->SOF = SOF;

	frame->dataLen = end - frame->data;

	*end = checksumFrame(frame);

	ssize_t fd = creat("/tmp/writeFrame", 0664);
	if (fd < 0) {
		warn("failed to create file");
		return;
	}

	ssize_t len = (void*)end - (void*)frame + 1;
	ssize_t ret = write(fd, frame, len);
	if (ret < len) {
		warn("failed to write %zd/%zd", ret, len);
	}

	if (close(fd) < 0) {
		warn("failed to close");
	}
}

void sendDataRequestExt(struct Frame* frame, uint8_t* end) {
	frame->subsystem = AF;
	frame->type = SREQ;
	frame->cmdID = dataRequestExt;

	struct AF_dataRequestExt* request = (struct AF_dataRequestExt*)frame->data;
	request->radius = 30;
	request->transID = TransactionID++;
	request->len = end - request->data;
	sendFrame(frame, end);
}

void lightOnOff(uint8_t addrMode, uint64_t dstAddr, uint8_t brightness, uint8_t transtime) {
	uint8_t buffer[0x200] = {0};

	struct Frame* frame = (struct Frame*)buffer;

	struct AF_dataRequestExt* request = (struct AF_dataRequestExt*)frame->data;
	request->dstAddrMode = addrMode;
	request->dstAddr = dstAddr;
	request->dstEndpoint = (addrMode == AddrGroup) ? 0xff : 11;
	//request->dstPanID = 0;
	request->srcEndpoint = 1;
	request->clusterID = genLevelCtrl;
	//request->options = 0;

	uint8_t* p = request->data;
	struct ZclHeader* header = (struct ZclHeader*)(p++);
	header->frameType = Specific;
	//header->manufacturerSpecific = 0;
	header->direction = ClientToServer;
	header->disableDefaultResponse = 1;
	//header->reserved = 0;

	*(p++) = TransactionSeq++; // transactionSequence
	*(p++) = moveToLevelWithOnOff; // commandID
	p = writeUint8(p, brightness);
	p = (uint8_t*)writeUint16((uint16_t*)p, transtime);


	sendDataRequestExt(frame, p);
}

// movemode:
//   0 - normal?
void lightMoveWithOnOff(uint8_t addrMode, uint64_t dstAddr, uint8_t movemode, uint8_t rate) {
	uint8_t buffer[0x200] = {0};

	struct Frame* frame = (struct Frame*)buffer;

	struct AF_dataRequestExt* request = (struct AF_dataRequestExt*)frame->data;
	request->dstAddrMode = addrMode;
	request->dstAddr = dstAddr;
	request->dstEndpoint = (addrMode == AddrGroup) ? 0xff : 11;
	//request->dstPanID = 0;
	request->srcEndpoint = 1;
	request->clusterID = genLevelCtrl;
	//request->options = 0;

	uint8_t* p = request->data;
	struct ZclHeader* header = (struct ZclHeader*)(p++);
	header->frameType = Specific;
	//header->manufacturerSpecific = 0;
	header->direction = ClientToServer;
	header->disableDefaultResponse = 1;
	//header->reserved = 0;

	*(p++) = TransactionSeq++; // transactionSequence
	*(p++) = moveWithOnOff; // commandID
	p = writeUint8(p, movemode);
	p = writeUint8(p, rate);


	sendDataRequestExt(frame, p);
}

void lightStop(uint8_t addrMode, uint64_t dstAddr) {
	uint8_t buffer[0x200] = {0};

	struct Frame* frame = (struct Frame*)buffer;

	struct AF_dataRequestExt* request = (struct AF_dataRequestExt*)frame->data;
	request->dstAddrMode = addrMode;
	request->dstAddr = dstAddr;
	request->dstEndpoint = (addrMode == AddrGroup) ? 0xff : 11;
	//request->dstPanID = 0;
	request->srcEndpoint = 1;
	request->clusterID = genLevelCtrl;
	//request->options = 0;

	uint8_t* p = request->data;
	struct ZclHeader* header = (struct ZclHeader*)(p++);
	header->frameType = Specific;
	//header->manufacturerSpecific = 0;
	header->direction = ClientToServer;
	header->disableDefaultResponse = 1;
	//header->reserved = 0;

	*(p++) = TransactionSeq++; // transactionSequence
	*(p++) = stop; // commandID


	sendDataRequestExt(frame, p);
}

void handle_AF_incomingMsg(const struct AF_incomingMsg* msg, const uint8_t* payloadEnd) {
	printf("AF_incomingMsg:\n  groupID: %hu\n  cluster: %s (%hu)\n  src:  0x%hx 0x%hhx\n  dstEndpoint: 0x%hhx\n  wasBroadcast: %hhu\n  linkQuality: %hhu\n  securityUse: %hhu\n  timestamp: %u\n  transSeqNumber: %hhu\n  len: %hhd\n",
		   msg->groupID, clusterName(msg->clusterID), msg->clusterID, msg->srcAddr, msg->srcEndpoint,
		   msg->dstEndpoint, msg->wasBroadcast, msg->linkQuality, msg->securityUse,
		   msg->timestamp, msg->transSeqNumber, msg->len);

	const uint8_t* msgEnd = msg->data + msg->len - 1;

	// frameEnd is the checksum, so stop before we get to it
	const uint8_t* end = MIN(payloadEnd, msgEnd);

	if (payloadEnd < msgEnd) {
		printf("warning: message payload extends %zd bytes past frame.\n", msgEnd - payloadEnd);
	} /*else if (payloadEnd > msgEnd) {
		printf("note: last %zd bytes of frame extend past message payload.\n", payloadEnd-msgEnd);
		}*/

	// controller/controller.ts :: onZclPayload
	switch (msg->clusterID) {
	case touchlink:
		printf("touchlink; nothing to do\n");
		break;
	case greenPower:
		printf("greenPower; not implemented\n");
		break;
	default: {
		// TODO: lookup device by source address
		struct Device* device = getDeviceByNetAddress(msg->srcAddr);
		if (device != NULL) {
			printf("Device: 0x%016lx %s\n",
				   device->macAddress, device->name);
		} else {
			printf("Device: unknown\n");
		}

		// TODO: handle re-transmitted Xiomi packets

		// parse header
		// zspec/zcl/zclHeader.ts

		const uint8_t* p = msg->data;

		struct ZclHeader* header = (struct ZclHeader*)(p++);
		printf("header:\n  frameType: %hhd\n  manuSpec: %hhd\n  direction: %hhd\n  disableDefaultResponse: %hhd\n  reserved: %hhd\n",
			   header->frameType, header->manufacturerSpecific, header->direction,
			   header->disableDefaultResponse, header->reserved);

		uint16_t manufacturerCode = 0;
		uint8_t transactionSeq;
		uint8_t commandID;
		if (header->manufacturerSpecific) {
			manufacturerCode = *((uint16_t*)(p));
			//printf("  debug: 0x%02hhx%02hhx\n", msg->data[2], msg->data[1]);
			printf("  manufacturerCode: 0x%02hx\n", manufacturerCode);
			p+=2;
		}
		transactionSeq = *(p++);
		commandID = *(p++);

		const uint8_t* payload = p;

		printf("  transactionSeq: %hhu\n  commandID: %hhu\n", transactionSeq, commandID);
		printf("  payload: ");
		for (; p <= end; p++) {
			printf("%02hhx ", *p);
		}
		printf("\n");

		if (header->frameType == Global) {
			switch (commandID) {
			case Report: {
				struct CommandReport* report = (struct CommandReport*)payload;
				int dataSize = dataTypeSize(report->dataType, report->data);
				printf("Report\n  attrID: 0x%02hx\n  dataType: %s (0x%02hhx)\n  size: %d\n  value: ",
					   report->attrID, dataTypeName(report->dataType), report->dataType, dataSize);
				dataTypePrint(report->dataType, report->data);
				printf("\n");

				switch (device->modelID) {
				case m_WXKG11LM: // wireless switch
					switch (report->attrID) {
					case 0x55: {
						uint16_t value = *((uint16_t*)report->data);
						char* action = "null";
						char actionNum[0x100];
						switch (value) {
						case 0x0:
							action = "hold";
							break;
						case 0x1:
							action = "single";
							break;
						case 0x2:
							action = "double";
							break;
						case 0x3:
							action = "triple";
							break;
						case 0x4:
							action = "quadrupal";
							break;
						case 0xff:
							action = "release";
							break;
						default:
							sprintf(actionNum, "%d", value);
							action = actionNum;
						}

						printf("topic:   zigbee2mqtt/0x%016lx\ntopic:   zigbee2mqtt/%s\nmessage: {\"action\":\"%s\"}\n",
							   device->macAddress, device->name, action);
						break;
					}}
					break;
				}
				break;
			}
			default:
				printf("Global command not implemented: %d\n", commandID);
			}
		} else {
			switch (msg->clusterID) {
			case genOTA:
				switch (commandID) {
				case queryNextImageRequest:
					printf("genOTA.queryNextImageRequest\n");
					break;
				default:
					printf("cluster %s (%d) command %d not implemented", clusterName(msg->clusterID), msg->clusterID, commandID);
				}
				break;
			default:
				printf("cluster %s (%d) commands not implemented\n", clusterName(msg->clusterID), msg->clusterID);
			}
		}
	}}
}

void handleFrame(const struct Frame* frame, const uint8_t* frameEnd) {
	switch (frame->type) {
	case AREQ:
		switch (frame->subsystem) {
		case AF:
			switch (frame->cmdID) {
			case incomingMsg:
				handle_AF_incomingMsg((const struct AF_incomingMsg*)frame->data, frameEnd-1);
				break;
			default:
				printf("Unhandled AREQ AF command ID: %d\n", frame->cmdID);
				printFrame(frame);
			}
			break;

		case ZDO:
			switch (frame->cmdID) {
			case ieeeAddrRsp:
				printf("ieeeAddrRsp\n");
				break;
			case srcRtgInd:
				struct ZDO_srcRtgInd* msg = (struct ZDO_srcRtgInd*)frame->data;

				printf("ZDO_srcRtgInd:\n  dstAddr: 0x%hx\n  relayCount: %hhd\n  relayList: 0x%hx\n",
					   msg->dstAddr, msg->relayCount, msg->relayList);
				break;
			default:
				printf("Unhandled AREQ ZDO command ID: %d\n", frame->cmdID);
				printFrame(frame);
			}
			break;

		default:
			printf("Unhandled AREQ frame subsystem: %d\n", frame->subsystem);
			printFrame(frame);
		}
		break;

	default:
		printf("Unhandled frame type: %d\n", frame->type);
		printFrame(frame);
	}
}

int main(int argc, char** argv) {
	if (argc<=1) {
		printf("missing filename\n");
		return 1;
	}

	char* filename = argv[1];
	int fh = open(filename, O_RDONLY);
	if (fh < 0) {
		err(EXIT_FAILURE, "could not open");
	}

	lightOnOff(AddrGroup, 0x2, 0xfe, 5);

	static uint8_t buffer[0x400]; // 1 kiB
	uintptr_t bufferLen = 0;

	while (1) {
		{
			ssize_t ret = read(fh, buffer + bufferLen, sizeof(buffer) - bufferLen);
			if (ret < 0) {
				err(EXIT_FAILURE, "could not read");
			} else if (ret == 0) {
				printf("end of file\n");
				return 0;
			}

			bufferLen += ret;
		}

		uint8_t* end = buffer+bufferLen-1;

		bool movePartialFrame = false;

		uint8_t *start = buffer;
		for (; start <= end;) {
			// advance to next SOF
			while (*start != SOF && start < end) start++;

			if (*start != SOF) {
				// reached end of buffer
				printf("SOF not found: 0x%02hhx\n", *start);
				break;
			}

			if (end - start < 2) {
				printf("frame too short\n");
				movePartialFrame = true;
				break;
			}

			struct Frame *frame = (struct Frame*)start;
			uint8_t *frameEnd = frame->data + frame->dataLen;

			if (frameEnd > end) {
				printf("partial frame\n\n");
				movePartialFrame = true;
				break;
			}

			uint8_t frameChecksum = *(frameEnd);

			uint8_t checksum = checksumFrame(frame);

			if (frameChecksum == checksum) {
				//printf("checksum match\n\n");
				handleFrame(frame, frameEnd);
			} else {
				printf("checksum mismatch\n\n");
			}

			printf("\n\n");

			start = frameEnd+1;
		}

		if (movePartialFrame) {
			// copy partial frame to beginning of buffer
			size_t len = end - start + 1;
			memmove(buffer, start, len);
			bufferLen = len;
		} else {
			bufferLen = 0;
		}
	}

	if (close(fh) < 0) {
		err(EXIT_FAILURE, "could not close");
	}

	return 0;
}
