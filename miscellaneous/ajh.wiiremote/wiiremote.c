
/*
 *  wiiremote.c
*
 *	This code deals with the wiiremote, including connecting and disconnection and all communication.
 *	It provides a simple API for dealing with the wiiremote.
 *
 *  Modified by and Copyright Alex Harker 2010.
 *	Based on aka.wiiremote, written by and Copyright Masayuki Akamatsu.
 *	aka.wiiremote based on "DarwiinRemote" by Hiroaki Kimura
 *
 */

#include <wiiremote.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Boolean	requestUpdates(WiiRemoteRef wiiremote);
void wiiremote_listener(IOBluetoothL2CAPChannelRef channel, void *refCon, IOBluetoothL2CAPChannelEvent *event);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WiiRemoteRef wiiremote_create(WiiCallbackFunction notify_function, void *callback_ref);
void wiiremote_destroy(WiiRemoteRef wiiremote);
void wiiremote_init(WiiRemoteRef wiiremote);

Boolean wiiremote_search(WiiRemoteRef wiiremote, char use_address, char *address);
Boolean wiiremote_stopsearch(WiiRemoteRef wiiremote);
void wiiremote_search_complete(void *refCon, IOBluetoothDeviceInquiryRef inquiry, IOReturn post, Boolean aborted);

void device_found(void *refCon, IOBluetoothDeviceInquiryRef inquiry, IOBluetoothDeviceRef device);
void device_updated(void *refCon, IOBluetoothDeviceInquiryRef inquiry, IOBluetoothDeviceRef device, uint32_t devicesRemaining);
void checkDevice(WiiRemoteRef wiiremote, IOBluetoothDeviceRef device);

Boolean wiiremote_connect(WiiRemoteRef wiiremote);
void wiiremote_disconnect(WiiRemoteRef wiiremote);
void wiiremote_channel_close_notify (void *refCon, IOBluetoothUserNotificationRef inRef, IOBluetoothObjectRef objectRef);
void wiiremote_discconect_notify(void *refCon, IOBluetoothUserNotificationRef inRef, IOBluetoothObjectRef objectRef);

Boolean	openCChan(WiiRemoteRef wiiremote);
Boolean	openIChan(WiiRemoteRef wiiremote);

Boolean sendCommand(WiiRemoteRef wiiremote, unsigned char *data, unsigned short length);
Boolean	writeData(WiiRemoteRef wiiremote, const unsigned char *data, unsigned long address, unsigned short length);
Boolean readDataRequest(WiiRemoteRef wiiremote, unsigned long address, unsigned short length);


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//


#define BLUETOOTH_TRY_INIT	\
short		i;				

#define BLUETOOTH_TRY(try_call)			\
for (i = 0; i < kTrial; i++)			\
{										\
	if ((try_call) == kIOReturnSuccess)	\
		break;							\
	usleep(kWait);						\
}

#define RETURN_FALSE_ON_TEST(test, do_on_fail)	\
if (test) {do_on_fail; return false;}

#define TRY_FAILED (i == kTrial)


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//


void wiiremote_callback(WiiRemoteRef wiiremote, WiimoteCallbackType Type, char *String)
{
	if (wiiremote->callback_function) wiiremote->callback_function(wiiremote->callback_ref, wiiremote, Type, String);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//


WiiRemoteRef wiiremote_create(WiiCallbackFunction notify_function, void *callback_ref)
{
	WiiRemoteRef wiiremote = (WiiRemoteRef) malloc (sizeof(WiiRemoteRec));
	if (!wiiremote) return 0;
	
	wiiremote_init(wiiremote);
	
	wiiremote->callback_function = notify_function;
	wiiremote->isBeingDestroyed = false;
	wiiremote->callback_ref = callback_ref;
	
	wiiremote->isMotionSensorEnabled = true;
	wiiremote->isIRSensorEnabled = false;
	wiiremote->isExpansionPortEnabled = true;
	
	wiiremote->MotionPlusMode = MotionPlusPassThrough;
	
	return wiiremote;
}


void wiiremote_destroy(WiiRemoteRef wiiremote)
{
	if (wiiremote == nil) return;
	
	wiiremote->isBeingDestroyed = true;
	
	wiiremote_stopsearch(wiiremote);
	if (wiiremote->connected == true)
		wiiremote_disconnect(wiiremote);
	
	free (wiiremote);
}


void wiiremote_init(WiiRemoteRef wiiremote)
{
	// Bluetooth 
	
	wiiremote->connected = false;
	
	wiiremote->inquiry = nil;
	wiiremote->device = nil;
	wiiremote->ichan = nil;
	wiiremote->cchan = nil;
	wiiremote->disconnectNotification = nil;

	wiiremote->search_address = nil;
	wiiremote->use_address = 0;

	// Read queue
	
	wiiremote->QueueLength = 0;
	wiiremote->ReadWait = 0;

	wiiremote->QueueWritePtr = 0;
	wiiremote->QueueReadPtr = 0;
	
	// Data bogus flags
	
	wiiremote->WiiButtonBogus = true;
	wiiremote->WiiIRBogus = true;
	wiiremote->WiiMotionBogus = true;
	wiiremote->WiiCallibrationBogus = true;
	wiiremote->ExpBogus = true;
	wiiremote->ExpCalibrationBogus = true;
	
	// Remote
	
	wiiremote->buttonData = 0;
	
	wiiremote->accX = 0;
	wiiremote->accY = 0;
	wiiremote->accZ = 0;
	
	wiiremote->accScaleX = 0;
	wiiremote->accScaleY = 0;
	wiiremote->accScaleZ = 0;
	
	wiiremote->lowZ = 0;
	wiiremote->lowX = 0;	
	wiiremote->orientation = 0;
	wiiremote->leftPoint = -1;
	
	wiiremote->tracking = false;
	
	wiiremote->batteryLevel = 0;
	
	wiiremote->wiiIRMode = kWiiIRModeExtended;
		
	wiiremote->isVibrationEnabled = false;
	
	wiiremote->isLED1Illuminated = false;
	wiiremote->isLED2Illuminated = false;
	wiiremote->isLED3Illuminated = false;
	wiiremote->isLED4Illuminated = false;
			
	// Extension controllers
	
	wiiremote->expType = WiiExpNotTested;
	wiiremote->ExpansionConnect = 0;
	wiiremote->ExpansionInit = 0;
	wiiremote->ConnectionSafety = 0;
	
	// Motion Plus
	
	wiiremote->mGyroY = 0;
	wiiremote->mGyroR = 0;
	wiiremote->mGyroP = 0;
	
	wiiremote->mFastY = 0;
	wiiremote->mFastR = 0;
	wiiremote->mFastP = 0;
	
	wiiremote->mScaleGyroY = 0;
	wiiremote->mScaleGyroR = 0;
	wiiremote->mScaleGyroP = 0;
		
	// Extension data
	
	wiiremote->eButtonData = 0;
	
	wiiremote->eStickX1 = 0;
	wiiremote->eStickY1 = 0;
	wiiremote->eStickX2 = 0;
	wiiremote->eStickY2 = 0;
	
	wiiremote->eCont1 = 0;
	wiiremote->eCont2 = 0;
	wiiremote->eCont3 = 0;
	wiiremote->eCont4 = 0;
	
	wiiremote->eAccX = 0;
	wiiremote->eAccY = 0;
	wiiremote->eAccZ = 0;
	
	wiiremote->eScaleStickX1 = 0;
	wiiremote->eScaleStickY1 = 0;
	wiiremote->eScaleStickX2 = 0;
	wiiremote->eScaleStickY2 = 0;
	
	wiiremote->eScaleCont1 = 0;
	wiiremote->eScaleCont2 = 0;
	wiiremote->eScaleCont3 = 0;
	wiiremote->eScaleCont4 = 0;
	
	wiiremote->eScaleAccX = 0;
	wiiremote->eScaleAccY = 0;
	wiiremote->eScaleAccZ = 0;
	
	wiiremote->eLowZ = 0;
	wiiremote->eLowX = 0;
	
	wiiremote->eOrientation = 0;	
			
	// Raw data (unknown controller)
	
	wiiremote->uData1 = 0;
	wiiremote->uData2 = 0;
	wiiremote->uData3 = 0;
	wiiremote->uData4 = 0;
	wiiremote->uData5 = 0;
	wiiremote->uData6 = 0;
	
}


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//


Boolean wiiremote_search(WiiRemoteRef wiiremote, char use_address, char *address)
{
	IOReturn ret;
	
	if (wiiremote->inquiry != nil)
	{
		wiiremote_callback(wiiremote, WiiPost, "Still Searching....");
		return true;
	}
	
	wiiremote_init(wiiremote);
	
	wiiremote->inquiry = IOBluetoothDeviceInquiryCreateWithCallbackRefCon((void *)wiiremote);
	IOBluetoothDeviceInquirySetDeviceFoundCallback(wiiremote->inquiry, device_found);
	IOBluetoothDeviceInquirySetDeviceNameUpdatedCallback(wiiremote->inquiry, device_updated);
	IOBluetoothDeviceInquirySetCompleteCallback(wiiremote->inquiry, wiiremote_search_complete);
	
	wiiremote->search_address = CFStringCreateWithCString(nil, address, kCFStringEncodingMacRoman);
	wiiremote->use_address = use_address;

	ret = IOBluetoothDeviceInquiryStart(wiiremote->inquiry);
	if (ret != kIOReturnSuccess)
	{
		wiiremote_callback(wiiremote, WiiPost, "Could Not Search.... (check that bluetooth is on)");
		IOBluetoothDeviceInquiryDelete(wiiremote->inquiry);
		if (wiiremote->search_address != nil) CFRelease(wiiremote->search_address);
		wiiremote->search_address = nil;
		wiiremote->inquiry = nil;
		return false;
	}
	
	wiiremote_callback(wiiremote, WiiPost, "Starting Search....");
	return true;
}


Boolean wiiremote_stopsearch(WiiRemoteRef wiiremote)
{	
	if (wiiremote->inquiry == nil)	return true;
	
	if (wiiremote->connected == false) 
		wiiremote_callback(wiiremote, WiiPost, "Search Finished.... wiiremote not found");
	
	IOBluetoothDeviceInquiryStop(wiiremote->inquiry);
	IOBluetoothDeviceInquiryDelete(wiiremote->inquiry);
	if (wiiremote->search_address != nil) CFRelease(wiiremote->search_address);
	wiiremote->search_address = nil;
	wiiremote->inquiry = nil;
	
	return true;
}


void wiiremote_search_complete(void *refCon, IOBluetoothDeviceInquiryRef inquiry, IOReturn post, Boolean aborted)
{
	if (aborted) return;
	
	wiiremote_stopsearch((WiiRemoteRef)refCon);
	return;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//


void device_found(void *refCon, IOBluetoothDeviceInquiryRef inquiry, IOBluetoothDeviceRef device)
{
	checkDevice((WiiRemoteRef)refCon, device);
}


void device_updated(void *refCon, IOBluetoothDeviceInquiryRef inquiry, IOBluetoothDeviceRef device, uint32_t devicesRemaining)
{
	checkDevice((WiiRemoteRef)refCon, device);
}


void checkDevice(WiiRemoteRef wiiremote, IOBluetoothDeviceRef device)
{
	CFStringRef	name;
	CFStringRef	address;
	
	if (wiiremote->connected == true) 
		return;
			
	name = IOBluetoothDeviceGetName(device);
	address = IOBluetoothDeviceGetAddressString(device);
	
	if (name != NULL && address != NULL)
	{
		if (CFStringCompare(name, CFSTR("Nintendo RVL-CNT-01"), 0) == kCFCompareEqualTo || CFStringCompare(name, CFSTR("Nintendo RVL-CNT-01-TR"), 0) == kCFCompareEqualTo )
		{
			if (!wiiremote->use_address || CFStringCompare(address, wiiremote->search_address, kCFCompareCaseInsensitive) == kCFCompareEqualTo)
			{				
				wiiremote->device = IOBluetoothObjectRetain(device);
				if (wiiremote_connect(wiiremote) == false)
				{
					wiiremote_callback(wiiremote, WiiErr, "Found wiimote.... Connection Failed");
					wiiremote_disconnect(wiiremote);
				}
				else 
				{
					// N.B. wiiremote->connected is set in connect function
					
					wiiremote_callback(wiiremote, WiiPost, "Found wiimote.... Connection Successful!!");
					wiiremote_callback(wiiremote, WiiConnection, 0);
					wiiremote_stopsearch(wiiremote);
				}
			}
			else 
				wiiremote_callback(wiiremote, WiiPost, "Found wiimote.... Wrong Address"); 		
		}
	}
}


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//


Boolean wiiremote_connect(WiiRemoteRef wiiremote)
{
	BLUETOOTH_TRY_INIT
	
	if (wiiremote->device == nil)
	{
		wiiremote_callback(wiiremote, WiiDebug, "Found wiimote device is NULL");
		return false;
	}
	
	// Connect the device and register for disconnection notification
	
	BLUETOOTH_TRY(IOBluetoothDeviceOpenConnection(wiiremote->device, nil, nil))
	RETURN_FALSE_ON_TEST(TRY_FAILED, wiiremote_callback(wiiremote, WiiDebug, "Timed out on opening connection"))
	wiiremote->disconnectNotification = IOBluetoothDeviceRegisterForDisconnectNotification(wiiremote->device, wiiremote_discconect_notify, (void *)wiiremote);

	// Open C and I channels
	
	RETURN_FALSE_ON_TEST(openCChan(wiiremote) == false, wiiremote_callback(wiiremote, WiiDebug, "Opening C chan failed"))
	RETURN_FALSE_ON_TEST(openIChan(wiiremote) == false, wiiremote_callback(wiiremote, WiiDebug, "Opening C chan failed"))
			
	// Set LEDs and get accelerometer calibration data - callback intitiates status / expansion port check etc.
	// We are connected - must set the below before we try to write to the device
	
	wiiremote->connected = true;						
	
	readDataRequest(wiiremote, 0x001E, 16);	
	wiiremote_led(wiiremote, wiiremote->isLED1Illuminated, wiiremote->isLED2Illuminated, wiiremote->isLED3Illuminated, wiiremote->isLED4Illuminated);
	
	return true;
}


void wiiremote_disconnect(WiiRemoteRef wiiremote)
{
	BLUETOOTH_TRY_INIT
	
	// Close cchan  - release and cleanup
	
	if (wiiremote->cchan != nil && IOBluetoothDeviceIsConnected(wiiremote->device))
	{
		BLUETOOTH_TRY(IOBluetoothL2CAPChannelCloseChannel(wiiremote->cchan))
		if (TRY_FAILED)
			wiiremote_callback(wiiremote, WiiDebug, "Failed to properly disconnect C Chan");
		IOBluetoothObjectRelease(wiiremote->cchan);
		wiiremote->cchan = nil;

	}
	
	// Close ichan - release and cleanup
	
	if (wiiremote->ichan != nil && IOBluetoothDeviceIsConnected(wiiremote->device))
	{
		BLUETOOTH_TRY(IOBluetoothL2CAPChannelCloseChannel(wiiremote->ichan))
		if (TRY_FAILED) 
			wiiremote_callback(wiiremote, WiiDebug, "Failed to properly disconnect I Chan");
		IOBluetoothObjectRelease(wiiremote->ichan);
		wiiremote->cchan = nil;
	}

	if (wiiremote->device != nil && IOBluetoothDeviceIsConnected(wiiremote->device))
	{
		BLUETOOTH_TRY(IOBluetoothDeviceCloseConnection(wiiremote->device))
		if (TRY_FAILED) wiiremote_callback(wiiremote, WiiErr, "Failed to properly disconnect device - hold power button on remote to disconnect");
	}
	
	if (wiiremote->device != nil) IOBluetoothObjectRelease(wiiremote->device);
	wiiremote->device = nil;
	
	if (wiiremote->disconnectNotification != nil) IOBluetoothUserNotificationUnregister(wiiremote->disconnectNotification);
	wiiremote->disconnectNotification = nil;

	// Notify client
	
	wiiremote->connected = false;
	if (wiiremote->isBeingDestroyed == false)
		wiiremote_callback(wiiremote, WiiConnection, 0);
}


void wiiremote_discconect_notify(void *refCon, IOBluetoothUserNotificationRef inRef, IOBluetoothObjectRef objectRef)
{
	CFStringRef	itsAddress, myAddress;
		
	itsAddress = IOBluetoothDeviceGetAddressString(objectRef);
	if (itsAddress != nil)
	{
		myAddress = IOBluetoothDeviceGetAddressString(((WiiRemoteRef)refCon)->device);
		if (myAddress != nil)
		{
			if (CFStringCompare(itsAddress, myAddress, 0) == kCFCompareEqualTo)
			{
				wiiremote_callback((WiiRemoteRef)refCon, WiiPost, "wiiremote initiated disconnection");
				wiiremote_disconnect((WiiRemoteRef)refCon);
			}
			CFRelease(myAddress);
		}
		CFRelease(itsAddress);
	}
}


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//


Boolean	openCChan(WiiRemoteRef wiiremote)
{
	// Open L2CAPChannel : BluetoothL2CAPPSM = 17
	
	BLUETOOTH_TRY_INIT

	BLUETOOTH_TRY(IOBluetoothDeviceOpenL2CAPChannelSync(wiiremote->device, &(wiiremote->cchan), 17, wiiremote_listener, (void *)wiiremote))
	RETURN_FALSE_ON_TEST(TRY_FAILED, wiiremote->cchan = nil)
	wiiremote->cchan = IOBluetoothObjectRetain(wiiremote->cchan);

	return true;
}


Boolean	openIChan(WiiRemoteRef wiiremote)
{
	// Open L2CAPChannel : BluetoothL2CAPPSM = 19
	
	BLUETOOTH_TRY_INIT

	BLUETOOTH_TRY(IOBluetoothDeviceOpenL2CAPChannelSync(wiiremote->device, &(wiiremote->ichan), 19, wiiremote_listener, (void *)wiiremote))
	RETURN_FALSE_ON_TEST(TRY_FAILED, wiiremote->ichan = nil)
	wiiremote->ichan = IOBluetoothObjectRetain(wiiremote->ichan);

	return true;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//


Boolean sendCommand(WiiRemoteRef wiiremote, unsigned char *data, unsigned short length)
{
	unsigned char buf[40];
	IOReturn	ret;
	int			i;
	
	if (wiiremote->connected == false)
		return false;
		
	memset(buf, 0, 40);
	buf[0] = 0x52;
	memcpy(buf + 1, data, length);
	
	if (buf[1] == 0x16)
		length = 23;
	else
		length++;
	
	// If we have issues sending the command then assume the C chan has gone bad and get a new one
	
	for (i = 0; i < kTrial; i++)
	{
		ret = IOBluetoothL2CAPChannelWriteSync(wiiremote->cchan, buf, length);
		if (ret == kIOReturnSuccess) break;
		
		IOBluetoothObjectRelease(wiiremote->cchan);
		wiiremote->cchan = nil;
		openCChan(wiiremote);
	}
	
	if (i == kTrial)
	{
		wiiremote_callback(wiiremote, WiiErr, "Failed to write to wiiremote....");
		return false;
	}
	
	return true;
}


Boolean	writeData(WiiRemoteRef wiiremote, const unsigned char *data, unsigned long address, unsigned short length)
{
	unsigned char cmd[22];
	int i;
	unsigned long addr = address;
	
	for (i = 0 ; i < length ; i++)
		cmd[i + 6] = data[i];
	
	for(; i < 16 ; i++)
		cmd[i + 6]= 0;
	
	cmd[0] = 0x16;
	cmd[1] = (addr >> 24) & 0xFF;
	cmd[2] = (addr >> 16) & 0xFF;
	cmd[3] = (addr >> 8) & 0xFF;
	cmd[4] = (addr >> 0) & 0xFF;
	cmd[5] = length;
		
	if (wiiremote->isVibrationEnabled) cmd[1] |= 0x01;
	
	data = cmd;
	
	return sendCommand(wiiremote, cmd, 22);
}


Boolean readData(WiiRemoteRef wiiremote, unsigned long address, unsigned short length)
{
	unsigned char cmd[7];
	unsigned long addr = address;
	unsigned short len = length;
	
	cmd[0] = 0x17;
	cmd[1] = (addr >> 24) & 0xFF;
	cmd[2] = (addr >> 16) & 0xFF;
	cmd[3] = (addr >> 8) & 0xFF;
	cmd[4] = (addr >> 0) & 0xFF;
	
	cmd[5] = (len >> 8) & 0xFF;
	cmd[6] = (len >> 0) & 0xFF;
	
	if (wiiremote->isVibrationEnabled)	cmd[1] |= 0x01;
	
	return sendCommand(wiiremote, cmd, 7);
}


Boolean serviceReadQueue(WiiRemoteRef wiiremote, Boolean force)
{
	int32_t CurrentPtr;
	
	//if (!force && OSAtomicCompareAndSwap32Barrier(0, 1, &wiiremote->ReadWait) == false)
	//{
	//	post ("FAIL READ");
	//	return true;
	//}
		
	if (OSAtomicCompareAndSwap32Barrier(0, 0, &wiiremote->QueueLength) == true)
	{
	//	OSAtomicCompareAndSwap32Barrier(1, 0, &wiiremote->ReadWait);
		return true;
	}

	CurrentPtr = OSAtomicIncrement32(&wiiremote->QueueReadPtr);
	post ("DO READ %ld %x", wiiremote->QueueLength, wiiremote->ReadAddressQueue[CurrentPtr % READ_QUEUE_LENGTH]);

	if (!(CurrentPtr % READ_QUEUE_LENGTH))
		OSAtomicAdd32(-READ_QUEUE_LENGTH, &wiiremote->QueueReadPtr);
			
	if (wiiremote->ReadAddressQueue[CurrentPtr % READ_QUEUE_LENGTH] == 0x04A600F0)
		wiiremote->MPSearch = 1;
	else
		wiiremote->MPSearch = 0;
	
	//if (wiiremote->MPSearch == 0 || ((wiiremote->expType == WiiExpUninitialised || wiiremote->expType == WiiExpNotAttached) || wiiremote->MotionPlusMode != MotionPlusPassThrough)) 
		readData(wiiremote, wiiremote->ReadAddressQueue[CurrentPtr % READ_QUEUE_LENGTH], wiiremote->ReadLengthQueue[CurrentPtr % READ_QUEUE_LENGTH]);
	//else
	//{
	//	wiiremote->MPSearch = 0;
	//	post ("no");
	//}
	
	OSAtomicDecrement32(&wiiremote->QueueLength);
	
	return true;
}


Boolean readDataRequest(WiiRemoteRef wiiremote, unsigned long address, unsigned short length)
{
	if (OSAtomicAdd32(0, &wiiremote->QueueLength) < READ_QUEUE_LENGTH)
	{		
		int32_t CurrentPtr = OSAtomicIncrement32(&wiiremote->QueueWritePtr);
		
		if (!(CurrentPtr % READ_QUEUE_LENGTH))
			OSAtomicAdd32(-READ_QUEUE_LENGTH, &wiiremote->QueueWritePtr);
			
		wiiremote->ReadAddressQueue[CurrentPtr % READ_QUEUE_LENGTH] = address;
		wiiremote->ReadLengthQueue[CurrentPtr % READ_QUEUE_LENGTH] = length;
		OSAtomicIncrement32(&wiiremote->QueueLength);
	}
	else {
		post("FULL");
	}

	
	return serviceReadQueue(wiiremote, false);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//


/*

* Handle report 0x21 (Read Data) from wiimote.

 * dp[0] = Bluetooth header
 * dp[1] = (0x21) Report/Channel ID
 * dp[2] = Wiimote Buttons
 * dp[3] = Wiimote Buttons
 * dp[4] = High 4 bits = payload size; Low 4 bits = post flag (0 = all good)	 
 * dp[5] = Offset of memory read
 * dp[6] = Offset of memory read
 * dp[7+] = the Data.

 */

 
void handlereadData(WiiRemoteRef wiiremote, unsigned char *dp, size_t dataLength);
void handleStatusReport(WiiRemoteRef wiiremote, unsigned char *dp, size_t dataLength);
void handleExpansionAttachment (WiiRemoteRef wiiremote, WiiExpansionConnectType Type, Boolean MustAttach);

void handlereadData(WiiRemoteRef wiiremote, unsigned char *dp, size_t dataLength)
{

	long read_address = dp[6] + (dp[5] << 8);
	WiiExpansionPortType expType = wiiremote->expType;
	WiiExpansionPortType testExpType;
	WiiMotionPlusModeType MotionPlusMode = wiiremote->MotionPlusMode;
	WiiExpansionConnectType connectType = WiiConnectReset;
	
	// Sanity check
	
	if (dp[4] & 0x0F && read_address != 0x00F0)			// 0x00F0 - will sort out this error later
	{
		post ("INSANE");
		serviceReadQueue(wiiremote, true);
		return;					
	}
		
	if (wiiremote->MPSearch)
		read_address = 0x0000;
		
	switch (read_address)
	{
		// Detect motion plus device
		
		case 0x0000:							
 					
			post ("HERE YOU GO");
			if (!(dp[4] & 0x0F) && MotionPlusMode != MotionPlusIgnore && (dp[21] == 0x04 || dp[21] == 0x05 || dp[21] == 0x07 || dp[21] == 0x00) && dp[22] == 0x05)
			{
				if (expType == WiiExpNotAttached || MotionPlusMode == MotionPlusPreferMP)
					connectType = WiiConnectMotionPlus;
				else
				{
					if (MotionPlusMode == MotionPlusPassThrough)
					{
						switch (expType)
						{
							case WiiNunchuk:
								connectType = WiiConnectMotionPlusNunchuk;
								break;
							case WiiClassicController:
								connectType = WiiConnectMotionPlusClassic;
								break;
						}
					}
				}
			}
			
			if (connectType == WiiConnectReset) // && expType == WiiExpUninitialised) 
				connectType = WiiConnectStandardExpansion;
			
			post ("MP CHECK");
			handleExpansionAttachment (wiiremote, connectType, true);
		
			serviceReadQueue(wiiremote, true);
			return;
		
		// Identify active attached expansion device
	
		case 0x00F0:
	
			if (dp[4] & 0x0F)
				expType = WiiExpNotAttached;
			else
				expType = (dp[17] << 16) + (dp[21] << 8) + dp[22];		// see header file for enum values

			// This is a fix for newer or possibly black nunchuks
			
			if (expType == 0xFF0000)																						
				expType = WiiNunchuk;
			
			// This is a fix for WiiMotionMplus INSIDE
			
			testExpType = expType ^ 0x010000;
			
			if (testExpType == WiiMotionPlus)																						
				expType = testExpType;
			if (testExpType == WiiMotionPlusClassic)																						
				expType = testExpType;
			if (testExpType == WiiMotionPlusNunchuk)																						
				expType = testExpType;
			
			if (expType != wiiremote->expType)																				
			{
				// The expansion status has changed
				
				wiiremote->expType = expType;
				post ("ID CHECK");
				if (expType != WiiExpNotAttached && expType != WiiExpUninitialised)
				{
					// Reset flags
					
					handleExpansionAttachment (wiiremote, WiiConnectReset, true);	
					
					// Get calbration data for new active extension (will check for mp if in passthrough after calibration
					
					readDataRequest(wiiremote, 0x04A40020, 32);																	
					
					// If in pass through mode check for motion plus
					
					if (MotionPlusMode == MotionPlusPassThrough && !WiiMotionPlusTest(expType))								
						handleExpansionAttachment (wiiremote, WiiSearchMotionPlus, true);
				}
				
				// Notify owner of expansion change and request updates
				
				wiiremote_callback(wiiremote, WiiExpansionChange, 0);														
				requestUpdates(wiiremote);																 
			}
			
			// Check to see if we're still waiting for the extension device to initialise
			
			if (OSAtomicCompareAndSwap32Barrier(1, 1, &wiiremote->ExpansionInit) == true)									
			{
				post ("TRY AGAIN %x %x", expType, dp[4]);
				if (++wiiremote->ConnectionSafety >= 5)
					handleExpansionAttachment (wiiremote, WiiConnectReset, true);
				else
				{
					// Read expansion device type again
					
					readDataRequest(wiiremote, 0x04A400F0, 16);																
				}
			}
			else
			{
				// Reset flags
				post ("WE ARE DONE");
				handleExpansionAttachment (wiiremote, WiiConnectReset, true);												
			}

			// If we no longer have an connected extension or the mp is preferred look to see if there is a motion plus
			
			if ((expType == WiiExpNotAttached && MotionPlusMode != MotionPlusIgnore) || (expType == WiiExpUninitialised && MotionPlusMode == MotionPlusPreferMP))	
				handleExpansionAttachment (wiiremote, WiiSearchMotionPlus, false);											
				
			// If the extension is uninitialised and the mp is not preferred initialise it

			if (expType == WiiExpUninitialised && MotionPlusMode != MotionPlusPreferMP && !(dp[4] & 0x0F))									
			{
				post ("INIT %ld", expType == WiiExpUninitialised && MotionPlusMode != MotionPlusPreferMP);
				handleExpansionAttachment (wiiremote, WiiConnectStandardExpansion, false);
			}
			
			serviceReadQueue(wiiremote, true);
			return;
		
		// Wiimote calibration data (and initial status)
		
		case 0x001E:
		
			// We read two bytes early so we can tell apart from a extension calibration read 
			
			dp += 2;																					
			
			wiiremote->wiiCalibData.accX_zero = (dp[7] << 2) + ((dp[10] & 0x30) >> 4);
			wiiremote->wiiCalibData.accY_zero = (dp[8] << 2) + ((dp[10] & 0xC) >> 2);
			wiiremote->wiiCalibData.accZ_zero = (dp[9] << 2) + (dp[10] & 0x3);
			
			wiiremote->wiiCalibData.accX_1g = (dp[11] << 2) + ((dp[14] & 0x30) >> 4);
			wiiremote->wiiCalibData.accY_1g = (dp[12] << 2) + ((dp[14] & 0xC) >> 2);
			wiiremote->wiiCalibData.accZ_1g = (dp[13] << 2) + (dp[14] & 0x3);
			
			wiiremote->WiiCallibrationBogus = false;
			wiiremote_getstatus(wiiremote);
						
			serviceReadQueue(wiiremote, true);
			return;
			
		// Expansion device calibration data
		
		case 0x0020:
			
			switch (expType)
			{
				
				// Nunchuk calibration data
				
				case WiiNunchuk:
				
					wiiremote->eAccCalibData.accX_zero = (dp[7] << 2) + ((dp[10] & 0x30) >> 4);
					wiiremote->eAccCalibData.accY_zero = (dp[8] << 2) + ((dp[10] & 0xC) >> 2);
					wiiremote->eAccCalibData.accZ_zero = (dp[9] << 2) + (dp[10] & 0x3);
					
					wiiremote->eAccCalibData.accX_1g = (dp[11] << 2) + ((dp[14] & 0x30) >> 4);
					wiiremote->eAccCalibData.accY_1g = (dp[12] << 2) + ((dp[14] & 0xC) >> 2);
					wiiremote->eAccCalibData.accZ_1g = (dp[13] << 2) + (dp[14] & 0x3);
					
					wiiremote->eStick1CalibData.x_max = dp[15];
					wiiremote->eStick1CalibData.x_min = dp[16];
					wiiremote->eStick1CalibData.x_center = dp[17];
					
					wiiremote->eStick1CalibData.y_max = dp[18];
					wiiremote->eStick1CalibData.y_min = dp[19];
					wiiremote->eStick1CalibData.y_center = dp[20];
					
					break;
					
				case WiiClassicController:
				
					wiiremote->eStick1CalibData.x_max = dp[7] >> 2;
					wiiremote->eStick1CalibData.x_min = dp[8] >> 2;
					wiiremote->eStick1CalibData.x_center = dp[9] >> 2;
					
					wiiremote->eStick1CalibData.y_max = dp[10] >> 2;
					wiiremote->eStick1CalibData.y_min = dp[11] >> 2;
					wiiremote->eStick1CalibData.y_center = dp[12] >> 2;	
					
					wiiremote->eStick2CalibData.x_max = dp[13] >> 3; 
					wiiremote->eStick2CalibData.x_min = dp[14] >> 3;
					wiiremote->eStick2CalibData.x_center = dp[15] >> 3;
					
					wiiremote->eStick2CalibData.y_max = dp[16] >> 3;
					wiiremote->eStick2CalibData.y_min = dp[17] >> 3;
					wiiremote->eStick2CalibData.y_center = dp[18] >> 3;
					
					break;	
			}
			return;
			
		case 0x0030:
			wiiremote->ExpCalibrationBogus = false;
			
			serviceReadQueue(wiiremote, true);
			return;
			
		// Wii remote buttons	
		
		default:
			wiiremote->buttonData = ((short) dp[2] << 8) + dp[3];

			serviceReadQueue(wiiremote, true);
	}
}


void handleStatusReport(WiiRemoteRef wiiremote, unsigned char *dp, size_t dataLength)
{
	// N.B. passed attachment status may not be accurate here, so we ignore it and explicity test it below
		
	wiiremote->batteryLevel = (double) dp[7];
	wiiremote->batteryLevel /= (double) 0xC0;		// C0 = fully charged.
	
	if (dp[4] & 0x10)
		wiiremote->isLED1Illuminated = true;
	else
		wiiremote->isLED1Illuminated = false;
	
	if (dp[4] & 0x20)
		wiiremote->isLED2Illuminated = true;
	else
		wiiremote->isLED2Illuminated = false;
	
	if (dp[4] & 0x40)
		wiiremote->isLED3Illuminated = true;
	else
		wiiremote->isLED3Illuminated = false;
	
	if (dp[4] & 0x80)
		wiiremote->isLED4Illuminated = true;
	else
		wiiremote->isLED4Illuminated = false;
			
	// Test for attachment of extensions including Motion Plus
	
	readDataRequest(wiiremote, 0x04A400F0, 16);
}

void handleExpansionAttachment (WiiRemoteRef wiiremote, WiiExpansionConnectType Type, Boolean MustAttach)
{
	bool Test;
	WiiExpansionPortType expType;
	
	if (wiiremote->connected == false || wiiremote->isExpansionPortEnabled == false)
		return;					
	
	Test = OSAtomicCompareAndSwap32Barrier(0, 1, &wiiremote->ExpansionConnect);
	
	if (MustAttach == false && Test == false) 
	{
		post ("FAIL EXT");
		return;
	}
	
	// If we're trying to change the extension
	
	if (Type != WiiConnectReset && Type != WiiSearchMotionPlus)												
	{
		wiiremote->ExpBogus = true;
		wiiremote->ExpCalibrationBogus = true;
		wiiremote->ConnectionSafety = 0;
	}
	
	expType = wiiremote->expType;
	
	switch (Type)
	{
		case WiiConnectReset:
		
			// Reset Flag
			post("reset");
			OSAtomicCompareAndSwap32Barrier(1, 0, &wiiremote->ExpansionConnect);
			OSAtomicCompareAndSwap32Barrier(1, 0, &wiiremote->ExpansionInit);			// don't keep trying to get the new device
			return;																		// do NOT read expansion device type
			
		case WiiConnectStandardExpansion:
		
			// Standard Expansion
			post("standard");
			writeData(wiiremote, (darr){0x55}, 0x04A400F0, 1);							// intialise device - unencrypted
			if (!WiiMotionPlusTest(expType))				
				writeData(wiiremote, (darr){0x00}, 0x04A400FB, 1);						// make device active
			break;
			
		case WiiConnectMotionPlus:
		
			// Motion Plus
			post ("MP");
			if (expType == WiiMotionPlusNunchuk || expType == WiiMotionPlusClassic)
				writeData(wiiremote, (darr){0x04}, 0x04A400FE, 1);						// make MotionPlus active alone (it's currently in passthorough mode)
			else
			{
				writeData(wiiremote, (darr){0x55}, 0x04A600F0, 1);						// intialise Motion Plus - unencrypted
				writeData(wiiremote, (darr){0x04}, 0x04A600FE, 1);						// make Motion Plus active
			}
			break;
			
		case WiiConnectMotionPlusNunchuk:
			post ("MP+NK");
			// Motion Plus + Nunchuk
		
			writeData(wiiremote, (darr){0x55}, 0x04A600F0, 1);							// intialise Motion Plus - unencrypted
			writeData(wiiremote, (darr){0x05}, 0x04A600FE, 1);		
			break;
			
		case WiiConnectMotionPlusClassic:
			post ("MP+CC");
			// Motion Plus + Classic Controller
					
			writeData(wiiremote, (darr){0x55}, 0x04A600F0, 1);							// intialise Motion Plus - unencrypted
			writeData(wiiremote, (darr){0x07}, 0x04A600FE, 1);
			break;
			
		case WiiSearchMotionPlus:
		
			// Search Motion Plus
			
			// do NOT read expansion device type
			
			if (WiiMotionPlusTest(expType))
			{
				post ("MP CONNECTED");
				handleExpansionAttachment(wiiremote, WiiConnectReset, true);								
			}
			post("search");
			// Search For Motion Plus (read high address)
			
			readDataRequest(wiiremote, 0x04A600F0, 16);
			
			// Do NOT read expansion device type (we haven't changed it yet!)
			
			return;																							
	}
		
	OSAtomicCompareAndSwap32Barrier(0, 1, &wiiremote->ExpansionInit);	
	readDataRequest(wiiremote, 0x04A400F0, 16);																	// read expansion device type
}

void handleExtensionData(WiiRemoteRef wiiremote, unsigned char *dp, size_t dataLength)
{
	WiiExpansionPortType expType = wiiremote->expType;
	WiiMotionPlusModeType MotionPlusMode;
	
	float absx;
	float absz;
	
	long mGyroY, mGyroR, mGyroP;
	char mFastY, mFastR, mFastP;
	char PassThrough = 0;
	
	unsigned char startByte;
	unsigned char bitmask;
	
	unsigned char *data_test;
	long i;
	
	switch (dp[1]) 
	{
		case 0x34:	startByte = 4;		break;
		case 0x35:	startByte = 7;		break;
		case 0x36:	startByte = 14;		break;
		case 0x37:	startByte = 17;		break;
			
		// This shouldn't ever happen
			
		default:	return;																
	}
	
	data_test = dp + startByte;
	
	// Data Validity
	
	for (i = 0; i < 8; i++)
		if (*data_test++ != 0xFF) break;
	
	// The former probably won't happen, but if it does it means the data is bogus
	
	if (i == 8 || wiiremote->ExpCalibrationBogus == true)								
		return;										
	
	// Motion Plus Pass Through
	
	if (expType == WiiMotionPlusNunchuk || expType == WiiMotionPlusClassic)
	{
		PassThrough = 1;
		if (dp[startByte + 5] & 0x2)
			expType = WiiMotionPlus;
		else
		{
			if (wiiremote->expType == WiiMotionPlusNunchuk)
				expType = WiiNunchuk;
			else
				expType = WiiClassicController;
		}
	}
	
	// Handle Data
	
	switch (expType)
	{
		case WiiMotionPlus:
	
			mGyroY = dp[startByte + 0] + ((dp[startByte + 3] & 0xFC) << 6);
			mGyroR = dp[startByte + 1] + ((dp[startByte + 4] & 0xFC) << 6);
			mGyroP = dp[startByte + 2] + ((dp[startByte + 5] & 0xFC) << 6);
			
			wiiremote->mFastY = mFastY = 1 - ((dp[startByte + 3] & 2) == 2);
			wiiremote->mFastR = mFastR = 1 - ((dp[startByte + 4] & 2) == 2);
			wiiremote->mFastP = mFastP = 1 - (dp[startByte + 3] & 1);
			
			wiiremote->mGyroY = mGyroY;		
			wiiremote->mGyroR = mGyroR;		
			wiiremote->mGyroP = mGyroP;		
			
			wiiremote->mScaleGyroY = mFastY ? (mGyroY - 8063) / 4.0 : (mGyroY - 8063) / 20.0;
			wiiremote->mScaleGyroR = mFastR ? (mGyroR - 8063) / 4.0 : (mGyroR - 8063) / 20.0;
			wiiremote->mScaleGyroP =  mFastP ? (mGyroP - 8063) / 4.0 : (mGyroP - 8063) / 20.0;
			
			// Check to see expansion status and act accordingly 
			
			MotionPlusMode = wiiremote->MotionPlusMode;
	
			if (MotionPlusMode == MotionPlusIgnore)
				handleExpansionAttachment(wiiremote, WiiConnectStandardExpansion, false);
			
			// Check to see if an extension attchment is plugged in
			
			if (MotionPlusMode == MotionPlusPreferStandard && dp[startByte + 4] & 1)									
				handleExpansionAttachment(wiiremote, WiiConnectStandardExpansion, false);
					
			if ((MotionPlusMode == MotionPlusPreferMP || (!(dp[startByte + 4] & 1))) && PassThrough)
				handleExpansionAttachment (wiiremote, WiiConnectMotionPlus, false);
				
			if (MotionPlusMode == MotionPlusPassThrough && !PassThrough && (dp[startByte + 4] & 1))
				handleExpansionAttachment (wiiremote, WiiConnectStandardExpansion, false);
		
			break;
			
		case WiiNunchuk:
		
			if (!PassThrough)
			{
				wiiremote->eStickX1 =		dp[startByte];
				wiiremote->eStickY1 =		dp[startByte + 1];
				wiiremote->eAccX   =		((unsigned short) dp[startByte + 2] << 2) | ((dp[startByte + 5] & 0x0C) >> 2);
				wiiremote->eAccY   =		((unsigned short) dp[startByte + 3] << 2) | ((dp[startByte + 5] & 0x30) >> 4);
				wiiremote->eAccZ   =		((unsigned short) dp[startByte + 4] << 2) | ((dp[startByte + 5] & 0xC0) >> 6);
				wiiremote->eButtonData =	dp[startByte + 5]  & 0x3;
			}
			else
			{
				wiiremote->eStickX1 =		dp[startByte];
				wiiremote->eStickY1 =		dp[startByte + 1];
				wiiremote->eAccX   =		((unsigned short) dp[startByte + 2] << 2) | ((dp[startByte + 5] & 0x10) >> 3);
				wiiremote->eAccY   =		((unsigned short) dp[startByte + 3] << 2) | ((dp[startByte + 5] & 0x20) >> 4);
				wiiremote->eAccZ   =		((unsigned short) (dp[startByte + 4] & 0xFE) << 2) | ((dp[startByte + 5] & 0xC0) >> 5);	
				wiiremote->eButtonData =	(dp[startByte + 5]  & 0xC) >> 2;
			}
			
			wiiremote->eScaleAccX = (double) (wiiremote->eAccX - wiiremote->eAccCalibData.accX_zero) / (double) (wiiremote->eAccCalibData.accX_1g - wiiremote->eAccCalibData.accX_zero);
			wiiremote->eScaleAccY = (double) (wiiremote->eAccY - wiiremote->eAccCalibData.accY_zero) / (double) (wiiremote->eAccCalibData.accY_1g - wiiremote->eAccCalibData.accY_zero);
			wiiremote->eScaleAccZ = (double) (wiiremote->eAccZ - wiiremote->eAccCalibData.accZ_zero) / (double) (wiiremote->eAccCalibData.accZ_1g - wiiremote->eAccCalibData.accZ_zero);
			
			if (wiiremote->eStickX1 > wiiremote->eStick1CalibData.x_max)
				wiiremote->eStick1CalibData.x_max = wiiremote->eStickX1;
			if (wiiremote->eStickY1 > wiiremote->eStick1CalibData.y_max)
				wiiremote->eStick1CalibData.y_max = wiiremote->eStickY1;
			if (wiiremote->eStickX1 < wiiremote->eStick1CalibData.x_min)
				wiiremote->eStick1CalibData.x_min = wiiremote->eStickX1;
			if (wiiremote->eStickY1 < wiiremote->eStick1CalibData.y_min)
				wiiremote->eStick1CalibData.y_min = wiiremote->eStickY1;
			
			if (wiiremote->eStickX1 > wiiremote->eStick1CalibData.x_center)
				wiiremote->eScaleStickX1 = (double) (wiiremote->eStickX1 - wiiremote->eStick1CalibData.x_center) / (double) (wiiremote->eStick1CalibData.x_max - wiiremote->eStick1CalibData.x_center);
			else
				wiiremote->eScaleStickX1 = (double) (wiiremote->eStickX1 - wiiremote->eStick1CalibData.x_center) / (double) (wiiremote->eStick1CalibData.x_center - wiiremote->eStick1CalibData.x_min);
			
			if (wiiremote->eStickY1 > wiiremote->eStick1CalibData.x_center)
				wiiremote->eScaleStickY1 = (double) (wiiremote->eStickY1 - wiiremote->eStick1CalibData.y_center) / (double) (wiiremote->eStick1CalibData.y_max - wiiremote->eStick1CalibData.y_center);
			else
				wiiremote->eScaleStickY1 = (double) (wiiremote->eStickY1 - wiiremote->eStick1CalibData.y_center) / (double) (wiiremote->eStick1CalibData.y_center - wiiremote->eStick1CalibData.y_min);
			
			wiiremote->eLowZ = wiiremote->eLowZ * .9 + wiiremote->eAccZ * .1;
			wiiremote->eLowX = wiiremote->eLowX * .9 + wiiremote->eAccX * .1;
			
			absx = abs(wiiremote->eLowX - 512);
			absz = abs(wiiremote->eLowZ - 512);
			
			if (wiiremote->eOrientation == 0 || wiiremote->eOrientation == 2) absx -= 5;
			if (wiiremote->eOrientation == 1 || wiiremote->eOrientation == 3) absz -= 5;
			
			if (absz >= absx)
			{
				if (absz > 5)
					wiiremote->eOrientation = (wiiremote->eLowZ > 512) ? 0 : 2;
			}
			else
			{
				if (absx > 5)
					wiiremote->eOrientation = (wiiremote->eLowX > 512) ? 3 : 1;
			}
		
			break;
			
		case WiiClassicController:
		case WiiGuitarHeroGuitar:																			
		case WiiGuitarHeroDrums:
		
			// N.B. Save space here by reusing classic controller space for guitar hero controllers (they are very similar)
			
			wiiremote->eButtonData = ((unsigned short)(dp[startByte + 4] << 8) + dp[startByte + 5]);
			
			if (PassThrough)
			{
				wiiremote->eButtonData = (wiiremote->eButtonData & 0xFFFE) | (dp[startByte + 0] & 0x1) | ((dp[startByte + 1] & 0x1) << 0x1);
				bitmask = 0x3E;
			}
			else bitmask = 0x3F;
			
			// N.B. bit reverse
			
			wiiremote->eButtonData = ~wiiremote->eButtonData;												
			
			wiiremote->eStickX1 =	dp[startByte + 0] & bitmask;
			wiiremote->eStickY1 =	dp[startByte + 1] & bitmask;
			
			wiiremote->eStickX2 =	(((dp[startByte + 0] & 0xC0) >> 3) |
									((dp[startByte + 1] & 0xC0) >> 5) |
									((dp[startByte + 2] & 0x80) >> 7)) & 0x1F;
			wiiremote->eStickY2 =	dp[startByte + 2] & 0x1F;
			
			wiiremote->eCont1 =		(((dp[startByte + 2] & 0x60) >> 2) |
									((dp[startByte + 3] & 0xE0) >> 5)) & 0x1F;
			wiiremote->eCont2 =		dp[startByte + 3] & 0x1F;
			
			if (wiiremote->eStickX1 > wiiremote->eStick1CalibData.x_max)
				wiiremote->eStick1CalibData.x_max = wiiremote->eStickX1;
			if (wiiremote->eStickY1 > wiiremote->eStick1CalibData.y_max)
				wiiremote->eStick1CalibData.y_max = wiiremote->eStickY1;
			if (wiiremote->eStickX1 < wiiremote->eStick1CalibData.x_min)
				wiiremote->eStick1CalibData.x_min = wiiremote->eStickX1;
			if (wiiremote->eStickY1 < wiiremote->eStick1CalibData.y_min)
				wiiremote->eStick1CalibData.y_min = wiiremote->eStickY1;
				
			if (wiiremote->eStickX2 > wiiremote->eStick2CalibData.x_max)
				wiiremote->eStick2CalibData.x_max = wiiremote->eStickX2;
			if (wiiremote->eStickY2 > wiiremote->eStick2CalibData.y_max)
				wiiremote->eStick2CalibData.y_max = wiiremote->eStickY2;
			if (wiiremote->eStickX2 < wiiremote->eStick2CalibData.x_min)
				wiiremote->eStick2CalibData.x_min = wiiremote->eStickX2;
			if (wiiremote->eStickY2 < wiiremote->eStick2CalibData.y_min)
				wiiremote->eStick2CalibData.y_min = wiiremote->eStickY2;
				
			if (wiiremote->eStickX1 > wiiremote->eStick1CalibData.x_center)
				wiiremote->eScaleStickX1 = (double) (wiiremote->eStickX1 - wiiremote->eStick1CalibData.x_center) / (double) (wiiremote->eStick1CalibData.x_max - wiiremote->eStick1CalibData.x_center);
			else
				wiiremote->eScaleStickX1 = (double) (wiiremote->eStickX1 - wiiremote->eStick1CalibData.x_center) / (double) (wiiremote->eStick1CalibData.x_center - wiiremote->eStick1CalibData.x_min);
			
			if (wiiremote->eStickY1 > wiiremote->eStick1CalibData.x_center)
				wiiremote->eScaleStickY1 = (double) (wiiremote->eStickY1 - wiiremote->eStick1CalibData.y_center) / (double) (wiiremote->eStick1CalibData.y_max - wiiremote->eStick1CalibData.y_center);
			else
				wiiremote->eScaleStickY1 = (double) (wiiremote->eStickY1 - wiiremote->eStick1CalibData.y_center) / (double) (wiiremote->eStick1CalibData.y_center - wiiremote->eStick1CalibData.y_min);
				
			if (wiiremote->eStickX2 > wiiremote->eStick2CalibData.x_center)
				wiiremote->eScaleStickX2 = (double) (wiiremote->eStickX2 - wiiremote->eStick2CalibData.x_center) / (double) (wiiremote->eStick2CalibData.x_max - wiiremote->eStick2CalibData.x_center);
			else
				wiiremote->eScaleStickX2 = (double) (wiiremote->eStickX2 - wiiremote->eStick2CalibData.x_center) / (double) (wiiremote->eStick2CalibData.x_center - wiiremote->eStick2CalibData.x_min);
			
			if (wiiremote->eStickY2 > wiiremote->eStick2CalibData.x_center)
				wiiremote->eScaleStickY2 = (double) (wiiremote->eStickY2 - wiiremote->eStick2CalibData.y_center) / (double) (wiiremote->eStick2CalibData.y_max - wiiremote->eStick2CalibData.y_center);
			else
				wiiremote->eScaleStickY2 = (double) (wiiremote->eStickY2 - wiiremote->eStick2CalibData.y_center) / (double) (wiiremote->eStick2CalibData.y_center - wiiremote->eStick2CalibData.y_min);
				
			if (expType == WiiGuitarHeroDrums)
			{
				// Determine velocity info for which pad
				
				if (wiiremote->eCont1 & 0x10)																	
				{
					switch ((wiiremote->eStickX2 & 0x1) | (wiiremote->eStickY2 & 0x1E) | ((wiiremote->eCont1 & 0x4) << 0x2))
					{				
						case 0x37:		wiiremote->eCont2 = 0x1;		break;
						case 0x36:		wiiremote->eCont2 = 0x2;		break;
						case 0x33:		wiiremote->eCont2 = 0x3;		break;
						case 0x23:		wiiremote->eCont2 = 0x4;		break;
						case 0x1F:		wiiremote->eCont2 = 0x5;		break;
						case 0x1D:		wiiremote->eCont2 = 0x6;		break;
						case 0x15:		wiiremote->eCont2 = 0x7;		break;
						default:		wiiremote->eCont2 = 0;
					}
				}
				else
					wiiremote->eCont2 = 0;
					
				wiiremote->eButtonData &= 0x7E;			// Mask Button Data	
				wiiremote->eCont1 &= 0x7;				// Mask Velocity Info
			}
			
		break;
		
	}
	
	wiiremote->uData1 = dp[startByte + 0];
	wiiremote->uData2 = dp[startByte + 1];
	wiiremote->uData3 = dp[startByte + 2];
	wiiremote->uData4 = dp[startByte + 3];
	wiiremote->uData5 = dp[startByte + 4];
	wiiremote->uData6 = dp[startByte + 5];
	
	wiiremote->ExpBogus = false;
}


void handleIRData(WiiRemoteRef wiiremote, unsigned char *dp, size_t dataLength)
{
	double d, dx, dy, cx, cy, ox, oy, gain;

	int p1, p2, r;
	
	long i;
	
	if (dp[1] == 0x33)
	{ 
		// 12 IR bytes
		
		int startByte = 0;
		for(i=0 ; i < 4 ; i++)
		{
			startByte = 7 + 3 * i;
			wiiremote->irData[i].x = (dp[startByte +0] | ((dp[startByte +2] & 0x30) << 4)) & 0x3FF;
			wiiremote->irData[i].y = (dp[startByte +1] | ((dp[startByte +2] & 0xC0) << 2)) & 0x3FF;
			wiiremote->irData[i].s =  dp[startByte +2] & 0x0F;
		}
 	}
	else
	{	
		// 10 IR bytes
	
		int shift = (dp[1] == 0x36) ? 4 : 7;
		int startByte = 0;
		for (i=0; i < 2; i++) {
			startByte = shift + 5 * i;
			wiiremote->irData[2*i].x = (dp[startByte +0] | ((dp[startByte +2] & 0x30) << 4)) & 0x3FF;
			wiiremote->irData[2*i].y = (dp[startByte +1] | ((dp[startByte +2] & 0xC0) << 2)) & 0x3FF;
			wiiremote->irData[2*i].s = ((wiiremote->irData[2*i].x == wiiremote->irData[2*i].y) && (wiiremote->irData[2*i].x == 0x3FF)) ? 0x0F : 0x05;  // No size is given in 10 byte report.
			
			wiiremote->irData[(2*i)+1].x = (dp[startByte +3] | ((dp[startByte +2] & 0x03) << 8)) & 0x3FF;
			wiiremote->irData[(2*i)+1].y = (dp[startByte +4] | ((dp[startByte +2] & 0x0C) << 6)) & 0x3FF;
			wiiremote->irData[(2*i)+1].s = ((wiiremote->irData[(2*i)+1].x == wiiremote->irData[(2*i)+1].y) && (wiiremote->irData[(2*i)+1].x == 0x3FF)) ? 0x0F : 0x05;  // No size is given in 10 byte report.
		}
	}
	
	p1 = -1;
	p2 = -1;
	
	// We should modify this loop to take the points with the lowest s (the brightest ones)
	
	for (i=0 ; i<4 ; i++) {
		if (p1 == -1) {
			if (wiiremote->irData [i].s < 0x0F)
				p1 = i;
		} else {
			if (wiiremote->irData [i].s < 0x0F) {
				p2 = i;
				break;
			}
		}
	}

	if ((p1 > -1) && (p2 > -1))
	{
		int l = wiiremote->leftPoint;
		if (wiiremote->leftPoint == -1)
		{
			switch (wiiremote->orientation)
			{
				case 0: l = (wiiremote->irData[p1].x < wiiremote->irData[p2].x) ? 0 : 1; break;
				case 1: l = (wiiremote->irData[p1].y > wiiremote->irData[p2].y) ? 0 : 1; break;
				case 2: l = (wiiremote->irData[p1].x > wiiremote->irData[p2].x) ? 0 : 1; break;
				case 3: l = (wiiremote->irData[p1].y < wiiremote->irData[p2].y) ? 0 : 1; break;
			}
			
			wiiremote->leftPoint = l;
		}
		
		r = 1-l;
		
		dx = wiiremote->irData[r].x - wiiremote->irData[l].x;
		dy = wiiremote->irData[r].y - wiiremote->irData[l].y;
		d = hypot (dx, dy);
		
		dx /= d;
		dy /= d;
		
		cx = (wiiremote->irData[l].x + wiiremote->irData[r].x)/kWiiIRPixelsWidth - 1;
		cy = (wiiremote->irData[l].y + wiiremote->irData[r].y)/kWiiIRPixelsHeight - 1;
		
		ox = -dy*cy-dx*cx;
		oy = -dx*cy+dy*cx;
		
		// Cam:
		//
		// Compensate for distance. There must be fewer than 0.75*768 pixels between the spots for this to work.
		// In other words, you have to be far enough away from the sensor bar for the two spots to have enough space on the image sensor to travel without one of the points going off the image.
		// Note: it is working very well ...
		
		gain = 4;
		if (d < (0.75 * kWiiIRPixelsHeight)) 
			gain = 1 / (1 - d/kWiiIRPixelsHeight);
		
		ox *= gain;
		oy *= gain;		

		wiiremote->angle = atan2(dy, dx);
		wiiremote->tracking = true;
	}
	else
	{
		ox = oy = -100;
		wiiremote->leftPoint = -1;	// not tracking
		wiiremote->angle = -100;
		wiiremote->tracking = false;
	}

	wiiremote->posX = ox;	
	wiiremote->posY = oy;
}


void handleDataReport(WiiRemoteRef wiiremote, unsigned char *dp, size_t dataLength)
{	
	float absx;
	float absz;
	
	// Wiimote buttons
	
	wiiremote->buttonData = (((short) dp[2] << 8) + dp[3]) & 0x1F9F;
	wiiremote->WiiButtonBogus = false;
	
	// Report contains extension data		
	
	switch (dp[1])
	{
		case 0x34:
		case 0x35:
		case 0x36:
		case 0x37: 
			handleExtensionData(wiiremote, dp, dataLength);
		break;
	}
	
	// Report contains IR data
	
	if (dp[1] & 0x02) handleIRData(wiiremote, dp, dataLength);
	wiiremote->WiiIRBogus = false;
	
	// Report contains motion sensor data
	
	if (dp[1] & 0x01)
	{
		wiiremote->accX = ((unsigned short) dp[4] << 2) | ((dp[3] & 0x60) >> 5);
		wiiremote->accY = ((unsigned short) dp[5] << 2) | ((dp[2] & 0x20) >> 4);
		wiiremote->accZ = ((unsigned short) dp[6] << 2) | ((dp[2] & 0x40) >> 5);
		
		wiiremote->accScaleX = (double) (wiiremote->accX - wiiremote->wiiCalibData.accX_zero) / (double) (wiiremote->wiiCalibData.accX_1g - wiiremote->wiiCalibData.accX_zero);
		wiiremote->accScaleY = (double) (wiiremote->accY - wiiremote->wiiCalibData.accY_zero) / (double) (wiiremote->wiiCalibData.accY_1g - wiiremote->wiiCalibData.accY_zero);
		wiiremote->accScaleZ = (double) (wiiremote->accZ - wiiremote->wiiCalibData.accZ_zero) / (double) (wiiremote->wiiCalibData.accZ_1g - wiiremote->wiiCalibData.accZ_zero);
			
		wiiremote->lowZ = wiiremote->lowZ * 0.9 + wiiremote->accZ * 0.1;
		wiiremote->lowX = wiiremote->lowX * 0.9 + wiiremote->accX * 0.1;
		
		absx = abs(wiiremote->lowX - 512);
		absz = abs(wiiremote->lowZ - 512);
		
		if (wiiremote->orientation == 0 || wiiremote->orientation == 2) absx -= 5;
		if (wiiremote->orientation == 1 || wiiremote->orientation == 3) absz -= 5;
		
		if (absz >= absx)
		{
			if (absz > 5)
				wiiremote->orientation = (wiiremote->lowZ > 512)?0:2;
		}
		else
		{
			if (absx > 5)
				wiiremote->orientation = (wiiremote->lowX > 512)?3:1;
		}
		wiiremote->WiiMotionBogus = false;
	}
	
}


//--------------------------------------------------------------------------------------------


void wiiremote_listener(IOBluetoothL2CAPChannelRef channel, void *refCon, IOBluetoothL2CAPChannelEvent *event)
{
	void *dataPointer = event->u.data.dataPtr;
	UInt16 dataLength = event->u.data.dataSize;
	
	if (event->eventType == kIOBluetoothL2CAPChannelEventTypeData)
	{
		WiiRemoteRef wiiremote = (WiiRemoteRef) refCon;
		unsigned char *dp = (unsigned char *) dataPointer;

		if (!wiiremote->device)
			return;
	
		switch (dp[1])
		{
			// Status report 
			
			case 0x20:
				if (dataLength < 8)
					return;
				handleStatusReport(wiiremote, dp, dataLength);
				
				// Make sure we keep getting state change report.
				
				requestUpdates(wiiremote);						
				
				return;
				
			// Read data
			
			case 0x21:
				handlereadData(wiiremote, dp, dataLength);
				return;
			
			// Write data response
			
			case  0x22:
				return;
			
			// Data report	
			
			default:
				if ((dp[1] & 0xF0) == 0x30)
					handleDataReport(wiiremote, dp, dataLength);
		}
	}
}


//--------------------------------------------------------------------------------------------


Boolean	requestUpdates(WiiRemoteRef wiiremote)
{	
	/*
	
	 Set the report type the Wiimote should send.
	 
	 The IR reports must be matched with the data format set when initializing the IR camera:
	 0x36, 0x37	- 10 IR bytes go with Basic mode
	 0x33		- 12 IR bytes go with Extended mode
	 0x3e/0x3f	- 36 IR bytes go with Full mode
	 
	 The Nunchuk and Classic controller use 6 bytes to report their state - reports that give more extension bytes don't provide any more info.
	 
	 Buttons			|	Accelerometer	|	IR		|	Extension
	 -------------------+-------------------+-----------+-------------		
	 0x30: Core Buttons	|					|			|
	 0x31: Core Buttons	|	Accelerometer	|			|
	 0x32: Core Buttons	|					|			|	 8 bytes
	 0x33: Core Buttons	|	Accelerometer	| 12 bytes	|
	 0x34: Core Buttons	|					|			|	19 bytes
	 0x35: Core Buttons	|	Accelerometer	|			|	16 bytes
	 0x36: Core Buttons	|					| 10 bytes	|	 9 bytes
	 0x37: Core Buttons	|	Accelerometer	| 10 bytes	|	 6 bytes
	 0x3d: ------------	|					|			|	21 bytes
	 
	 0x3e / 0x3f:		Interleaved Core Buttons and Accelerometer with 16/36 IR bytes
	 
	 */
	
	char requestExpansion = wiiremote->isExpansionPortEnabled && wiiremote->expType != WiiExpNotAttached  && wiiremote->expType != WiiExpUninitialised;
	
	// Just buttons
	
	unsigned char cmd[] = {0x12, 0x02, 0x30};						
	
	if (wiiremote->isIRSensorEnabled)
	{
		if (requestExpansion)
		{
			// Buttons, 10 IR Bytes, 9 Extension Bytes
			
			cmd[2] = 0x36;											
			wiiremote->wiiIRMode = kWiiIRModeBasic;
		}
		else
		{
			// Buttons, Accelerometer, and 12 IR Bytes
			
			cmd[2] = 0x33;											
			wiiremote->wiiIRMode = kWiiIRModeExtended;		
		}
		
		// Set IR Mode
		
		writeData(wiiremote, (darr){ wiiremote->wiiIRMode }, 0x04B00033, 1);
		usleep(kWait);															
	}
	else
	{		
		if (requestExpansion)
		{
			// Buttons, 19 Extension Bytes
			
			cmd[2] = 0x34;												 
		}
		else
		{
			// Buttons
			
			cmd[2] = 0x30;											
		}
	}
	
	// Add Accelerometer
	
	if (wiiremote->isMotionSensorEnabled)	
		cmd[2] |= 0x01;			
	
	// Vibration Flag
	
	if (wiiremote->isVibrationEnabled)	
		cmd[1] |= 0x01;				
	
	return sendCommand(wiiremote, cmd, 3);
}

//--------------------------------------------------------------------------------------------


Boolean wiiremote_isconnected(WiiRemoteRef wiiremote)
{
	return wiiremote->connected;
}


void wiiremote_getaddress(WiiRemoteRef wiiremote, char *address)
{
	CFStringRef	cfstring;
	
	cfstring = IOBluetoothDeviceGetAddressString(wiiremote->device);
	CFStringGetCString(cfstring, address, 32, kCFStringEncodingMacRoman);
	CFRelease(cfstring);	
}


Boolean wiiremote_motionsensor(WiiRemoteRef wiiremote, Boolean enabled)
{
	wiiremote->isMotionSensorEnabled = enabled;
	return requestUpdates(wiiremote);
}


Boolean wiiremote_vibration(WiiRemoteRef wiiremote, Boolean enabled)
{	
	wiiremote->isVibrationEnabled = enabled;
	return requestUpdates(wiiremote);
}


Boolean wiiremote_expansion(WiiRemoteRef wiiremote, Boolean enabled)
{	
	wiiremote->isExpansionPortEnabled = enabled;
	wiiremote_prefermotionplus(wiiremote, wiiremote->MotionPlusMode);
	return requestUpdates(wiiremote);
}


void wiiremote_prefermotionplus(WiiRemoteRef wiiremote, WiiMotionPlusModeType Mode)
{	
	wiiremote->MotionPlusMode = Mode;	
	
	if (wiiremote->connected == true && wiiremote->isExpansionPortEnabled == true)
	{
		// Initiate search for motion plus if appropriate to mode	
		
		if (Mode == MotionPlusPreferMP || Mode == MotionPlusPassThrough)												
			handleExpansionAttachment (wiiremote, WiiSearchMotionPlus, true);
		else
		{
			// Read expansion device type in case there is a valid extension there
			
			if (!WiiValidExpTest(wiiremote->expType))
				readDataRequest(wiiremote, 0x04A400F0, 16);									
		}
	}
}


Boolean wiiremote_getstatus(WiiRemoteRef wiiremote)
{
	unsigned char cmd[] = {0x15, 0x00};
	return sendCommand(wiiremote, cmd, 2);
}


Boolean wiiremote_led(WiiRemoteRef wiiremote, Boolean enabled1, Boolean enabled2, Boolean enabled3, Boolean enabled4)
{
	unsigned char cmd[] = {0x11, 0x00};
	
	if (enabled1) cmd[1] |= 0x10;
	if (enabled2) cmd[1] |= 0x20;
	if (enabled3) cmd[1] |= 0x40;
	if (enabled4) cmd[1] |= 0x80;
	
	if (wiiremote->isVibrationEnabled) cmd[1] |= 0x01;
	
	wiiremote->isLED1Illuminated = enabled1;
	wiiremote->isLED2Illuminated = enabled2;
	wiiremote->isLED3Illuminated = enabled3;
	wiiremote->isLED4Illuminated = enabled4;
	
	return sendCommand(wiiremote, cmd, 2);
}


Boolean wiiremote_irsensor(WiiRemoteRef wiiremote, Boolean enabled)
{
	Boolean	ret;
	unsigned char cmd[] = {0x13, 0x00};
	unsigned char cmd2[] = {0x1a, 0x00};

	wiiremote->isIRSensorEnabled = enabled;
	
	// IR enable 1
	
	if (wiiremote->isVibrationEnabled)	cmd[1] |= 0x01;
	if (wiiremote->isIRSensorEnabled)	cmd[1] |= 0x04;
	if ((ret = sendCommand(wiiremote, cmd, 2)) == false)
		return ret;
	usleep(kWait);
	
	// Set register 0x1a (IR enable 2)
	
	if (enabled)
		cmd2[1] |= 0x04;
	if ((ret = sendCommand(wiiremote, cmd2, 2)) == false)
		return ret;
	
	usleep(kWait);
	
	if(enabled)
	{
		// Based on marcan's method, found on wiili wiki:
		// Tweaked to include some aspects of cliff's setup procedure in the hopes of it actually turning on 100% of the time
		// Previously - was seeing 30-40% failure rate before
		// The sleeps help it it seems
		
		usleep(kWait);
		if ((ret = writeData(wiiremote, (darr){0x01}, 0x04B00030, 1)) == false)	return ret;
		usleep(kWait);
		if ((ret = writeData(wiiremote, (darr){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0x00, 0xC0}, 0x04B00000, 9)) == false)	return ret;
		usleep(kWait);
		if ((ret = writeData(wiiremote, (darr){0x40, 0x00}, 0x04B0001A, 2)) == false)	return ret;
		usleep(kWait);
		if ((ret = writeData(wiiremote, (darr){0x08}, 0x04B00030, 1)) == false)	return ret;
		usleep(kWait);
		
		requestUpdates(wiiremote);
	}
	else
	{
		// Probably should do some writes to power down the camera, save battery but don't know how yet.
		
		ret = wiiremote_motionsensor(wiiremote, wiiremote->isMotionSensorEnabled);
		ret = wiiremote_vibration(wiiremote, wiiremote->isVibrationEnabled);
		ret = wiiremote_expansion(wiiremote, wiiremote->isExpansionPortEnabled);
	}
	
	return ret;
}
