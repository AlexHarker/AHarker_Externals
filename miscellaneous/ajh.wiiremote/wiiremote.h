
/*
 *  wiiremote.h
 *
 *	Header file for wiiremote.c
 *
 *	This code deals with the wiiremote, including connecting and disconnection and all communication.
 *	It provides a simple API for dealing with the wiiremote.
 *
 *  Modified by and Copyright Alex Harker 2010.
 *	Based on aka.wiiremote, written by and Copyright Masayuki Akamatsu.
 *	aka.wiiremote based on "DarwiinRemote" by Hiroaki Kimura
 *
 */

#include <IOBluetooth/IOBluetoothUserLib.h>
#include <libkern/OSAtomic.h>
#include <unistd.h>


/*------------------------------------- Trivial Type Defines ------------------------------------*/

// Trial defines and wait time
// the unit of kWait is microseconds, thus 10000 means 10ms

#define	kTrial	10
#define kWait	10000		

// IR dimension defines

#define kWiiIRPixelsWidth 1024.0
#define kWiiIRPixelsHeight 768.0

// data array type

typedef unsigned char darr[];

/*-------------------------------------- Main Type Defines --------------------------------------*/


typedef unsigned long WiimoteCallbackType;
enum 
{
	WiiPost,
	WiiErr,
	WiiDebug,
	WiiConnection,
	WiiExpansionChange
};


typedef void (*WiiCallbackFunction)(void *, void *, WiimoteCallbackType, char *);


typedef unsigned char WiiIRModeType;
enum 
{
	kWiiIRModeBasic		= 0x01,
	kWiiIRModeExtended	= 0x03,
	kWiiIRModeFull		= 0x05
};


typedef struct 
{
	int x;
	int y;
	int s;
	
} IRData;


typedef struct 
{
	unsigned short accX_zero;
	unsigned short accY_zero; 
	unsigned short accZ_zero;
	
	unsigned short accX_1g;
	unsigned short accY_1g;
	unsigned short accZ_1g; 
	
} WiiAccCalibData;


typedef struct 
{
	unsigned char x_min; 
	unsigned char x_max;
	unsigned char x_center;
	 
	unsigned char y_min; 
	unsigned char y_max; 
	unsigned char y_center; 
	
} WiiJoyStickCalibData;


typedef UInt32 WiiExpansionPortType;
enum
{
	WiiExpNotAttached		= 0xFFFFFE,
	WiiExpUninitialised		= 0xFFFFFF,
	WiiNunchuk				= 0x000000,
	WiiClassicController	= 0x000101,
	WiiGuitarHeroGuitar		= 0x000103,
	WiiGuitarHeroDrums		= 0x010103,
	WiiMotionPlus			= 0x000405,
	WiiMotionPlusNunchuk	= 0x000505,
	WiiMotionPlusClassic	= 0x000705,
	WiiExpNotTested			= 0x00FFFF
};


#define WiiUnknownTest(x)	((x != WiiExpNotAttached) && (x != WiiExpUninitialised) && (x != WiiNunchuk) && (x != WiiClassicController) && (x != WiiGuitarHeroGuitar) && \
							(x != WiiGuitarHeroDrums) && (x != WiiMotionPlus) && (x != WiiMotionPlusNunchuk) && (x != WiiMotionPlusClassic) && (x != WiiExpNotTested))
#define WiiMotionPlusTest(x) ((x == WiiMotionPlus) || (x == WiiMotionPlusNunchuk) || (x == WiiMotionPlusClassic))
#define WiiValidExpTest(x) (!(x & 0xF0))


typedef UInt16 WiiExpansionConnectType;
enum
{
	WiiConnectReset,
	WiiConnectStandardExpansion,
	WiiConnectMotionPlus,
	WiiConnectMotionPlusNunchuk,
	WiiConnectMotionPlusClassic,
	WiiSearchMotionPlus,
};


typedef UInt16 WiiMotionPlusModeType;
enum
{
	MotionPlusIgnore,
	MotionPlusPreferStandard,
	MotionPlusPreferMP,
	MotionPlusPassThrough,
	NumOfMPModes
};


#define READ_QUEUE_LENGTH 20


typedef struct _WiiRemoteRec
{
	// Wiiremote callback
	
	WiiCallbackFunction callback_function;
	Boolean	isBeingDestroyed;
	void *callback_ref;
	
	// Bluetooth
	
	Boolean connected;
	
	IOBluetoothDeviceInquiryRef		inquiry;
	IOBluetoothDeviceRef			device;
	IOBluetoothL2CAPChannelRef		ichan;
	IOBluetoothL2CAPChannelRef		cchan;
	IOBluetoothUserNotificationRef	disconnectNotification;

	CFStringRef		search_address;
	char			use_address;
	
	// Read queue
	
	unsigned long ReadAddressQueue[READ_QUEUE_LENGTH];
	unsigned long ReadLengthQueue[READ_QUEUE_LENGTH];

	long MPSearch;
	
	int32_t QueueLength;
	int32_t ReadWait;

	int32_t QueueWritePtr;
	int32_t QueueReadPtr;
	
	// Data bogus flags
	
	Boolean WiiButtonBogus;
	Boolean WiiIRBogus;
	Boolean WiiMotionBogus;
	Boolean WiiCallibrationBogus;
	Boolean ExpBogus;
	Boolean ExpCalibrationBogus;
	
	// User options
	
	Boolean	isMotionSensorEnabled;
	Boolean	isIRSensorEnabled;
	Boolean	isExpansionPortEnabled;

	WiiMotionPlusModeType MotionPlusMode;
	
	// Remote
	
	unsigned short buttonData;
	
	unsigned short accX;
	unsigned short accY;
	unsigned short accZ;
	
	float accScaleX;
	float accScaleY;
	float accScaleZ;
	
	float lowZ;
	float lowX;
	char orientation;
	int leftPoint;				// is point 0 or 1 on the left. -1 when not tracking.
	
	float posX;
	float posY;
	float angle;
	Boolean tracking;
	
	double batteryLevel;
	
	WiiIRModeType wiiIRMode;
	IRData irData[4];
	
	Boolean	isLED1Illuminated;
	Boolean	isLED2Illuminated;
	Boolean	isLED3Illuminated;
	Boolean	isLED4Illuminated;
		
	Boolean	isVibrationEnabled;
		
	WiiAccCalibData wiiCalibData;
	
	// Extension controllers
	
	WiiExpansionPortType expType;
	int32_t ExpansionConnect;				// Thread flag for atomic compare/swap operations to determine whether to issue expansion change instructions
	int32_t ExpansionInit;					// Thread flag for atomic compare/swap operations to determine wheteher to continue reading for updates on the expansion port attachment
	long ConnectionSafety;					// For preventing the lock staying locked forever if the initialisation has failed (happens going between mp and standard sometimes)
	
	// Motion Plus
	
	long mGyroY;
	long mGyroR;		// Roll
	long mGyroP;		// Pitch
	
	char mFastY;
	char mFastR;
	char mFastP;
	
	float mScaleGyroY;		// Yaw
	float mScaleGyroR;		// Roll
	float mScaleGyroP;		// Pitch
	
	long mExpCount;
		
	// Extension data
	
	unsigned short eButtonData;

	unsigned char eStickX1;
	unsigned char eStickY1;
	unsigned char eStickX2;
	unsigned char eStickY2;
	
	unsigned char eCont1;
	unsigned char eCont2;
	unsigned char eCont3;
	unsigned char eCont4;
	
	unsigned short eAccX;
	unsigned short eAccY;
	unsigned short eAccZ;
	
	float eScaleStickX1;
	float eScaleStickY1;
	float eScaleStickX2;
	float eScaleStickY2;
	
	float eScaleCont1;
	float eScaleCont2;
	float eScaleCont3;
	float eScaleCont4;
	
	float eScaleAccX;
	float eScaleAccY;
	float eScaleAccZ;
	
	float eLowZ;
	float eLowX;
	char eOrientation;

	WiiAccCalibData	eAccCalibData;
	WiiJoyStickCalibData eStick1CalibData;
	WiiJoyStickCalibData eStick2CalibData;
	
	// Raw extension data (unknown type)
	
	unsigned char uData1;
	unsigned char uData2;
	unsigned char uData3;
	unsigned char uData4;
	unsigned char uData5;
	unsigned char uData6;

}	WiiRemoteRec, *WiiRemoteRef;


/*-------------------------------------- Function Prototypes --------------------------------------*/


WiiRemoteRef	wiiremote_create(WiiCallbackFunction notify_function, void *callback_ref);
void			wiiremote_destroy(WiiRemoteRef wiiremote);

Boolean			wiiremote_search(WiiRemoteRef wiiremote, char use_address, char *address);
Boolean			wiiremote_stopsearch(WiiRemoteRef wiiremote);
Boolean			wiiremote_connect(WiiRemoteRef wiiremote);
void			wiiremote_disconnect(WiiRemoteRef wiiremote);

Boolean			wiiremote_isconnected(WiiRemoteRef wiiremote);
void			wiiremote_getaddress(WiiRemoteRef wiiremote, char *address);
Boolean			wiiremote_motionsensor(WiiRemoteRef wiiremote, Boolean enabled);
Boolean			wiiremote_irsensor(WiiRemoteRef wiiremote, Boolean enabled);
Boolean			wiiremote_vibration(WiiRemoteRef wiiremote, Boolean enabled);
Boolean			wiiremote_led(WiiRemoteRef wiiremote, Boolean enabled1, Boolean enabled2, Boolean enabled3, Boolean enabled4);
Boolean			wiiremote_expansion(WiiRemoteRef wiiremote, Boolean enabled);
void			wiiremote_prefermotionplus(WiiRemoteRef wiiremote, WiiMotionPlusModeType preferred);
Boolean			wiiremote_getstatus(WiiRemoteRef wiiremote);

