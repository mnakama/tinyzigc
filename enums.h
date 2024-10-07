#pragma once

// adapter/z-stack/constants/common.ts
enum AddressMode {
	AddrNotPresent = 0x00,
	AddrGroup      = 0x01,
	Addr16bit      = 0x02,
	Addr64bit      = 0x03,
	AddrBroadcast  = 0x0f,
};

// zspec/zcl/definition/cluster.ts
enum Cluster {
	genBasic = 0,
	genLevelCtrl = 8,
	genMultistateInput = 18,
	genOTA = 25,
	greenPower = 33,
	haMeterIdentification = 2817,
	haApplianceEventsAlerts = 2818,
	haApplianceStatistics = 2819,
	haElectricalMeasurement = 2820,
	haDiagnostic = 2821,
	touchlink = 4096,
};

// zspec/zcl/definition/enums.ts
enum DataType {
	UINT8 = 0x20,
	UINT16 = 0x21,
	CHAR_STR = 0x42,
};

// adapter/z-stack/unpi/constants.ts
enum Type {
	POLL = 0,
	SREQ = 1,
	AREQ = 2,
	SRSP = 3,
};

// adapter/z-stack/unpi/constants.ts
enum Subsystem {
	RESERVED = 0,
	SYS = 1,
	MAC = 2,
	NWK = 3,
	AF = 4,
	ZDO = 5,
	SAPI = 6,
	UTIL = 7,
	DEBUG = 8,
	APP = 9,
	APP_CNF = 15,
	GREENPOWER = 21,
};

// adapter/z-stack/znp/definition.ts
enum AF_Command {
	dataRequest = 1,
	dataRequestExt = 2,
	incomingMsg = 129,
};

// adapter/z-stack/znp/definition.ts
enum ZDO_Command {
	ieeeAddrRsp = 129,
	srcRtgInd = 196,
};

// zspec/zcl/definition/enums.ts
enum FrameType {
	Global = 0,
	Specific = 1,
};

// zspec/zcl/definition/enums.ts
enum FrameDirection {
	ClientToServer = 0,
	ServerToClient = 1,
};

// zspec/zcl/definition/foundation.js
enum FrameGlobalCommand {
	Read = 0x00,
	ReadRsp = 0x01,
	Write = 0x02,
	WriteUndiv = 0x03,
	WriteRsp = 0x04,
	WriteNoRsp = 0x05,
	ConfigReport = 0x06,
	ConfigReportResp = 0x07,
	ReadReportConfig = 0x08,
	ReadReportConfigRsp = 0x09,
	Report = 0x0a,
	DefaultRsp = 0x0b,
	Discover = 0x0c,
	DiscoverRsp = 0x0d,
	ReadStructured = 0x0e,
	WriteStructured = 0x0f,
	WriteStructuredRsp = 0x10,
	DiscoverCommands = 0x11,
	DiscoverCommandsRsp = 0x12,
	DiscoverCommandsGen = 0x13,
	DiscoverCommandsGenRsp = 0x14,
	DiscoverExt = 0x15,
	DiscoverExtRsp = 0x16,
};

// zspec/zcl/definition/cluster.ts
enum GenClusterCommand {
	queryNextImageRequest = 1,
};

// zspec/zcl/definition/cluster.ts
enum GenLevelCtrlCommand {
	moveToLevel = 0, // level (uint8), transtime (uint16)
	move = 1, // movemode (uint8), rate (uint8)
	step = 2, // stepmode (uint8), stepsize (uint8), transtime (uint16)
	stop = 3, // no params
	moveToLevelWithOnOff = 4, // level (uint8), transtime (uint16)
	moveWithOnOff = 5, // movemode (uint8), rate (uint8)
	stepWithOnOff = 6, // stepmode (uint8), stepsize (uint8), transtime (uint16)
	stopWithOnOff = 7, // no params
	moveToLevelTuya = 240, // level (uint16), transtime (uint16)
};

enum Model {
	m_WXKG11LM, // wireless switch
	m_MCCGQ11LM, // door/window sensor
	m_ZNCZ12LM, // outlet
	m_ZNCWWSQ01LM, // cat feeder
	m_929002294101, // hue lamp bulb
	m_9290024683, // hue 70w ambient bulb
	m_9290024687, // hue 70w color bulb
	m_046677577957, // hue 70w wide bulb
};
