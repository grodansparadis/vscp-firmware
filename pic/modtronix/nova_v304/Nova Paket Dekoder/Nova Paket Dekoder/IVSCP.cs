#region ---( Includes )---
using System;

//using Xedia.Serialization;
#endregion

namespace Xedia.Protocol.VSCP
{
	#region ---( Enumerations )---
	public enum LogLevel : byte 
	{
		Debug,
		Error,
		Fatal
	}
/*
#if (! NETCF)
	[System.ComponentModel.TypeConverter(typeof(ControlEventConverter))]
#endif
*/
	public enum ControlEvent : int 
	{
		Undefined = 0,

		MuteOnOff      = (EventClass.CONTROL << 16) + 1,
		AllLightsOnOff = (EventClass.CONTROL << 16) + 2,
		Open           = (EventClass.CONTROL << 16) + 3,
		Close          = (EventClass.CONTROL << 16) + 4,
		TurnOn         = (EventClass.CONTROL << 16) + 5,
		TurnOff        = (EventClass.CONTROL << 16) + 6,
		Start          = (EventClass.CONTROL << 16) + 7,
		Stop           = (EventClass.CONTROL << 16) + 8,
		Reset          = (EventClass.CONTROL << 16) + 9,
		Interrupt      = (EventClass.CONTROL << 16) + 10,
		Sleep          = (EventClass.CONTROL << 16) + 11,
		Wakeup         = (EventClass.CONTROL << 16) + 12,
		Resume         = (EventClass.CONTROL << 16) + 13,
		Pause          = (EventClass.CONTROL << 16) + 14,
		Activate       = (EventClass.CONTROL << 16) + 15,
		Deactivate     = (EventClass.CONTROL << 16) + 16,
		Dim            = (EventClass.CONTROL << 16) + 20,
		ChangeChannel  = (EventClass.CONTROL << 16) + 21
	}
/***
#if (! NETCF)
	[System.ComponentModel.TypeConverter(typeof(EventCodeConverter))]
#endif
 ***/
	public enum EventCode : int
	{
		Undefined = 0,
		StatusHeartbeat		= (EventClass.PROTOCOL << 16) + 1,
		NewNodeOnline		= (EventClass.PROTOCOL << 16) + 2,
		ProbeACK			= (EventClass.PROTOCOL << 16) + 3,
		NicknameFailure		= (EventClass.PROTOCOL << 16) + 4,
		SetNickname			= (EventClass.PROTOCOL << 16) + 6,
		NicknameAccepted	= (EventClass.PROTOCOL << 16) + 7,
		DropNickname		= (EventClass.PROTOCOL << 16) + 8,
		ReadRegister		= (EventClass.PROTOCOL << 16) + 9,
		ReadWriteResponse	= (EventClass.PROTOCOL << 16) + 10,
		WriteRegister		= (EventClass.PROTOCOL << 16) + 11,
		BootloaderMode		= (EventClass.PROTOCOL << 16) + 12,
		BootloaderACK		= (EventClass.PROTOCOL << 16) + 13,
		BootloaderNACK		= (EventClass.PROTOCOL << 16) + 14,
		StartBlockTransfer	= (EventClass.PROTOCOL << 16) + 15,
		BlockData			= (EventClass.PROTOCOL << 16) + 16,
		BlockDataACK		= (EventClass.PROTOCOL << 16) + 17,
		BlockDataNACK		= (EventClass.PROTOCOL << 16) + 18,
		ProgBlockData		= (EventClass.PROTOCOL << 16) + 19,
		ProgBlockDataACK	= (EventClass.PROTOCOL << 16) + 20,
		ProgBlockDataNACK	= (EventClass.PROTOCOL << 16) + 21,
		ActivateImage		= (EventClass.PROTOCOL << 16) + 22,
		ReadPage			= (EventClass.PROTOCOL << 16) + 24,
		WritePage			= (EventClass.PROTOCOL << 16) + 25,
		ReadPageResponse	= (EventClass.PROTOCOL << 16) + 26,
		GetEDAMatrix		= (EventClass.PROTOCOL << 16) + 32,
		EDAMatrixResponse	= (EventClass.PROTOCOL << 16) + 33,

        XBUS_Data           = (EventClass.DATA << 16) + 1,
        AD_Value            = (EventClass.DATA << 16) + 2,
        DA_Value            = (EventClass.DATA << 16) + 3,
        RelativeStringth    = (EventClass.DATA << 16) + 4,
        
        Warning = (EventClass.ALARM << 16) + 1,
		Alarm			= (EventClass.ALARM << 16) + 2,
		AlarmSound		= (EventClass.ALARM << 16) + 3,
		AlarmLight		= (EventClass.ALARM << 16) + 4,
		PowerOnOff		= (EventClass.ALARM << 16) + 5,
		EmergencyStop   = (EventClass.ALARM << 16) + 6,
		EmergencyPause  = (EventClass.ALARM << 16) + 7,
		EmergencyReset  = (EventClass.ALARM << 16) + 8,
		EmergencyResume = (EventClass.ALARM << 16) + 9,

		Count				= (EventClass.MEASUREMENT << 16) + 1,
		Length				= (EventClass.MEASUREMENT << 16) + 2,
		Mass				= (EventClass.MEASUREMENT << 16) + 3,
		Time				= (EventClass.MEASUREMENT << 16) + 4,
		ElectricCurrent		= (EventClass.MEASUREMENT << 16) + 5,
		Temperature			= (EventClass.MEASUREMENT << 16) + 6,
		AmountOfSubstance	= (EventClass.MEASUREMENT << 16) + 7,
		IntensityOfLight	= (EventClass.MEASUREMENT << 16) + 8,
		Frequency			= (EventClass.MEASUREMENT << 16) + 9,
		Force				= (EventClass.MEASUREMENT << 16) + 11,
		Pressure			= (EventClass.MEASUREMENT << 16) + 12,
		Energy				= (EventClass.MEASUREMENT << 16) + 13,
		Power				= (EventClass.MEASUREMENT << 16) + 14,
		ElectricalCharge	= (EventClass.MEASUREMENT << 16) + 15,
		ElectricalPotential	= (EventClass.MEASUREMENT << 16) + 16,
		ElectricalCapacitance	= (EventClass.MEASUREMENT << 16) + 17,
		ElectricalResistance	= (EventClass.MEASUREMENT << 16) + 18,
		ElectricalConductance	= (EventClass.MEASUREMENT << 16) + 19,
		MagneticFieldStrength	= (EventClass.MEASUREMENT << 16) + 20,
		MagneticFlux			= (EventClass.MEASUREMENT << 16) + 21,
		MagneticFluxDensity		= (EventClass.MEASUREMENT << 16) + 22,
		Inductance				= (EventClass.MEASUREMENT << 16) + 23,
		FluxOfLight				= (EventClass.MEASUREMENT << 16) + 24,
		lluminance				= (EventClass.MEASUREMENT << 16) + 25,
		RadiationDose			= (EventClass.MEASUREMENT << 16) + 26,
		CatalyticActivity		= (EventClass.MEASUREMENT << 16) + 27,
		Volume					= (EventClass.MEASUREMENT << 16) + 28,
		SoundIntensity			= (EventClass.MEASUREMENT << 16) + 29,
		Angle					= (EventClass.MEASUREMENT << 16) + 30,
		Position				= (EventClass.MEASUREMENT << 16) + 31,
		Speed					= (EventClass.MEASUREMENT << 16) + 32,
		Acceleration			= (EventClass.MEASUREMENT << 16) + 33,
		Tension					= (EventClass.MEASUREMENT << 16) + 34,
		DampMoist				= (EventClass.MEASUREMENT << 16) + 35,
		Flow					= (EventClass.MEASUREMENT << 16) + 36,
		TermalResistance		= (EventClass.MEASUREMENT << 16) + 37,
		RefractivePower			= (EventClass.MEASUREMENT << 16) + 38,
		DynamicViscosity		= (EventClass.MEASUREMENT << 16) + 39,
		SoundImpedance			= (EventClass.MEASUREMENT << 16) + 40,
		SoundResistance			= (EventClass.MEASUREMENT << 16) + 41,
		ElectircalElastance		= (EventClass.MEASUREMENT << 16) + 42,
		LuminousEnergy			= (EventClass.MEASUREMENT << 16) + 43,
		Luminance				= (EventClass.MEASUREMENT << 16) + 44,
		ChemicalConcentration	= (EventClass.MEASUREMENT << 16) + 45,
		AbsorbedIonizedRadiation= (EventClass.MEASUREMENT << 16) + 46,
		DoseEquivalent			= (EventClass.MEASUREMENT << 16) + 47,
		LightFlux				= (EventClass.MEASUREMENT << 16) + 48,
		DewPoint				= (EventClass.MEASUREMENT << 16) + 49,		     
       
		Button             = (EventClass.INFO << 16) + 1,
		Mouse              = (EventClass.INFO << 16) + 2,
		On                 = (EventClass.INFO << 16) + 3,
		Off                = (EventClass.INFO << 16) + 4,
		Alive			   = (EventClass.INFO << 16) + 5,
		Terminating        = (EventClass.INFO << 16) + 6,
		Opened             = (EventClass.INFO << 16) + 7,
		Closed             = (EventClass.INFO << 16) + 8,
		NodeHeartbeat      = (EventClass.INFO << 16) + 9,
		BelowLimit         = (EventClass.INFO << 16) + 10,
		AboveLimit         = (EventClass.INFO << 16) + 11,
		Pulse              = (EventClass.INFO << 16) + 12,
		Error              = (EventClass.INFO << 16) + 13,
		Resumed            = (EventClass.INFO << 16) + 14,
		Paused             = (EventClass.INFO << 16) + 15,
		GoodMorning        = (EventClass.INFO << 16) + 17,
		GoodDay            = (EventClass.INFO << 16) + 18,
		GoodAfternoon      = (EventClass.INFO << 16) + 19,
		GoodEvening        = (EventClass.INFO << 16) + 20,
		GoodNight          = (EventClass.INFO << 16) + 21,
		SeeYouSoon         = (EventClass.INFO << 16) + 22,
		Goodbye            = (EventClass.INFO << 16) + 23,
		Stopped            = (EventClass.INFO << 16) + 24,
		Started            = (EventClass.INFO << 16) + 25,
		ResetCompleted     = (EventClass.INFO << 16) + 26,
		Interrupted        = (EventClass.INFO << 16) + 27,
		PreparingToSleep   = (EventClass.INFO << 16) + 28,
		WokenUp            = (EventClass.INFO << 16) + 29,
		Dusk               = (EventClass.INFO << 16) + 30,
		Dawn               = (EventClass.INFO << 16) + 32,
		Active             = (EventClass.INFO << 16) + 33,
		Inactive           = (EventClass.INFO << 16) + 34,
		Busy               = (EventClass.INFO << 16) + 35,
		Idle               = (EventClass.INFO << 16) + 36,
		LevelChanged       = (EventClass.INFO << 16) + 40,

		IncomingCall     = (EventClass.PHONE << 16) + 1,
		OutgoingCall     = (EventClass.PHONE << 16) + 2,
		Ring             = (EventClass.PHONE << 16) + 3,
		CallAnswer       = (EventClass.PHONE << 16) + 4,
		CallHungup       = (EventClass.PHONE << 16) + 5,
		CallAbort        = (EventClass.PHONE << 16) + 6,
		CallTransfer     = (EventClass.PHONE << 16) + 7,
		CallDatabaseInfo = (EventClass.PHONE << 16) + 8,

		MuteOnOff      = (EventClass.CONTROL << 16) + 1,
		AllLightsOnOff = (EventClass.CONTROL << 16) + 2,
		Open           = (EventClass.CONTROL << 16) + 3,
		Close          = (EventClass.CONTROL << 16) + 4,
		TurnOn         = (EventClass.CONTROL << 16) + 5,
		TurnOff        = (EventClass.CONTROL << 16) + 6,
		Start          = (EventClass.CONTROL << 16) + 7,
		Stop           = (EventClass.CONTROL << 16) + 8,
		Reset          = (EventClass.CONTROL << 16) + 9,
		Interrupt      = (EventClass.CONTROL << 16) + 10,
		Sleep          = (EventClass.CONTROL << 16) + 11,
		Wakeup         = (EventClass.CONTROL << 16) + 12,
		Resume         = (EventClass.CONTROL << 16) + 13,
		Pause          = (EventClass.CONTROL << 16) + 14,
		Activate       = (EventClass.CONTROL << 16) + 15,
		Deactivate     = (EventClass.CONTROL << 16) + 16,
		Dim            = (EventClass.CONTROL << 16) + 20,
		ChangeChannel  = (EventClass.CONTROL << 16) + 21,

		LogMessage					= (EventClass.LOG << 16) + 1,
		StartLogging				= (EventClass.LOG << 16) + 2,
		StopLogging					= (EventClass.LOG << 16) + 3,
		SetLogLevel					= (EventClass.LOG << 16) + 4,

		RC5Receive					= (EventClass.IR << 16) + 1,
		RC5Transmit					= (EventClass.IR << 16) + 2,

		OneWireNewID				= (EventClass.ONE_WIRE << 16) + 1,
		OneWireConvert				= (EventClass.ONE_WIRE << 16) + 2,
		OneWireReadROM				= (EventClass.ONE_WIRE << 16) + 3,
		OneWireMatchROM				= (EventClass.ONE_WIRE << 16) + 4,
		OneWireSkipROM				= (EventClass.ONE_WIRE << 16) + 5,
		OneWireSearchROM			= (EventClass.ONE_WIRE << 16) + 6,
		OneWireConditionalSearchROM	= (EventClass.ONE_WIRE << 16) + 7,
		OneWireProgram				= (EventClass.ONE_WIRE << 16) + 8,
		OneWireOverdriveSkipROM		= (EventClass.ONE_WIRE << 16) + 9,
		OneWireOverdriveMatchROM	= (EventClass.ONE_WIRE << 16) + 10,
		OneWireReadMemory			= (EventClass.ONE_WIRE << 16) + 11,
		OneWireWriteMemory			= (EventClass.ONE_WIRE << 16) + 12,

		X10StandardMessageReceive	= (EventClass.X10 << 16) + 1,
		X10ExtendedMessageReceive	= (EventClass.X10 << 16) + 2,
		X10StandardMessageSend		= (EventClass.X10 << 16) + 3,
		X10ExtendedMessageSend		= (EventClass.X10 << 16) + 4,
		X10SimpleMessage			= (EventClass.X10 << 16) + 5,
/*
		Level2ReadRegister			= (EventClass.L2CONTROL << 16) + 1,
		Level2WriteRegister			= (EventClass.L2CONTROL << 16) + 2,
		Level2ReadWriteRespose		= (EventClass.L2CONTROL << 16) + 3,		
		ConfigReadRequest			= (EventClass.L2CONTROL << 16) + 4,
		ConfigReadResponse			= (EventClass.L2CONTROL << 16) + 5,
		ConfigChanged				= (EventClass.L2CONTROL << 16) + 6,
		ConfigUpdateRequest			= (EventClass.L2CONTROL << 16) + 7,
		ConfigUpdateResponse		= (EventClass.L2CONTROL << 16) + 8,

		Talk						= (EventClass.TTS << 16) + 1,
*/
	}       
	public enum EventClass : short
	{
		//-----------------------------------------------------------------------------
		//                   Types for CLASS --- VSCP Protocol Functionality
		//-----------------------------------------------------------------------------
		PROTOCOL = 0,
		//-----------------------------------------------------------------------------
		//                   CLASS --- Alarm
		//-----------------------------------------------------------------------------
		ALARM = 1,
		//-----------------------------------------------------------------------------
		//                   CLASS --- Security
		//-----------------------------------------------------------------------------
		SECURITY = 2,
		//-----------------------------------------------------------------------------
		//                   CLASS --- Measurement
		//-----------------------------------------------------------------------------
		MEASUREMENT = 10,
		//-----------------------------------------------------------------------------
		//                   CLASS --- Data
		//-----------------------------------------------------------------------------
		DATA = 15,
		//-----------------------------------------------------------------------------
		//                   CLASS --- Event
		//-----------------------------------------------------------------------------
		INFO = 20,
		//-----------------------------------------------------------------------------
		//                   CLASS --- Control
		//-----------------------------------------------------------------------------
		CONTROL = 30,
		//-----------------------------------------------------------------------------
		//                   CLASS --- Phone
		//-----------------------------------------------------------------------------
		PHONE = 100,
		//-----------------------------------------------------------------------------
		//                   CLASS --- LIN interface
		//-----------------------------------------------------------------------------
		LIN = 101,
		//-----------------------------------------------------------------------------
		//                   CLASS --- IR interface
		//-----------------------------------------------------------------------------
		IR = 110,
		//-----------------------------------------------------------------------------
		//                   CLASS --- 1-wire protocol i/f
		//-----------------------------------------------------------------------------
		ONE_WIRE = 200,
		//-----------------------------------------------------------------------------
		//                   CLASS --- X10 i/f
		//-----------------------------------------------------------------------------
		X10 = 201,
		//-----------------------------------------------------------------------------
		//                   CLASS --- LON
		//-----------------------------------------------------------------------------
		LON = 202,
		//-----------------------------------------------------------------------------
		//                   CLASS --- EIB
		//-----------------------------------------------------------------------------
		EIB = 203,
		//-----------------------------------------------------------------------------
		//                   CLASS --- SNAP
		//-----------------------------------------------------------------------------
		SNAP = 204,
		//-----------------------------------------------------------------------------
		//                   CLASS --- MUMIN
		//-----------------------------------------------------------------------------
		MUMIN = 205,
		//-----------------------------------------------------------------------------
		//                   CLASS --- Log
		//-----------------------------------------------------------------------------
		LOG = 509,
		//-----------------------------------------------------------------------------
		//                   CLASS --- Laboratory
		//-----------------------------------------------------------------------------
		LABORATORY = 510,
		//-----------------------------------------------------------------------------
		//                   CLASS --- Local
		//-----------------------------------------------------------------------------
		//L2CONTROL = 1024,

		//-----------------------------------------------------------------------------
		//                   CLASS --- Text To Speech
		//-----------------------------------------------------------------------------
		//TTS = 1028
	}
	public enum EVENT_PRIORITY : byte
	{
		Lowest = 0x00,
		Low = 0x01,
		BelowNormal = 0x02,
		Normal = 0x03,
		AboveNormal = 0x04,		
		High = 0x05,
		Urgent = 0x06,
		Critical = 0x07
	}
	#endregion
}
