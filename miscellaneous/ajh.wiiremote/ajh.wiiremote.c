
/*
 *  ajh.wiiremote
 *
 *	ajh.wiiremote is an object for communicating with a nintendo wiiremote directly via bluetooth. It supports most known extension controllers and the motion plus extension.
 *
 *	ajh.wiiremote is based on aka.wiiremote.c - Written by and Copyright Masayuki Akamatsu.
 *
 *  Copyright 2010 Alex Harker.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <wiiremote.h>

typedef struct _ajhwiiremote
{
	t_object        x_obj;
	
	WiiRemoteRef	wiiremote;
	char			address[32];
	char			use_address;
	
	long			poll;
	
	long			last_buttonData;
	long			last_eButtonData;
	
	long			last_nStickX;
	long			last_nStickY;
	long			last_eStickX1;
	long			last_eStickY1;
	long			last_eStickX2;
	long			last_eStickY2;
	long			last_eCont1;
	long			last_eCont2;
		
	void			*statusOut;
	void			*dataOut;
	
	void			*data_clock;
	
} t_ajhwiiremote;


void *this_class;	


void ajhwiiremote_report_timed(t_ajhwiiremote *x);
void ajhwiiremote_report(t_ajhwiiremote *x);

void ajhwiiremote_address(t_ajhwiiremote *x, t_symbol *s);
void ajhwiiremote_connect(t_ajhwiiremote *x);
void ajhwiiremote_disconnect(t_ajhwiiremote *x);

void ajhwiiremote_poll(t_ajhwiiremote *x, long poll);
void ajhwiiremote_motionsensor(t_ajhwiiremote *x, long mode);
void ajhwiiremote_irsensor(t_ajhwiiremote *x, long enable);
void ajhwiiremote_vibration(t_ajhwiiremote *x, long enable);
void ajhwiiremote_led(t_ajhwiiremote *x, long enable1, long enable2, long enable3, long enable4);
void ajhwiiremote_expansion(t_ajhwiiremote *x, long enable);
void ajhwiiremote_motionplusmode(t_ajhwiiremote *x, long mode);

void ajhwiiremote_getbattery(t_ajhwiiremote *x);
void ajhwiiremote_getexpansion(t_ajhwiiremote *x);
void ajhwiiremote_getled(t_ajhwiiremote *x);
void ajhwiiremote_getaddress(t_ajhwiiremote *x);
void ajhwiiremote_getcalibration(t_ajhwiiremote *x);

void ajhwiiremote_connection(t_ajhwiiremote *x, t_symbol *s, short argc, t_atom *argv);
void ajhwiiremotre_notify (void *x, void *wiiremote, WiimoteCallbackType Type, char *String);

void ajhwiiremote_init(t_ajhwiiremote *x);
void ajhwiiremote_exp_init(t_ajhwiiremote *x);
void *ajhwiiremote_new(t_symbol *s, short ac, t_atom *av);
void ajhwiiremote_free(t_ajhwiiremote *x);
void ajhwiiremote_assist(t_ajhwiiremote *x, void *b, long m, long a, char *s);


t_symbol *ps_remote;
t_symbol *ps_classic;
t_symbol *ps_nunchuk;
t_symbol *ps_guitar;
t_symbol *ps_drums;
t_symbol *ps_motionplus;
t_symbol *ps_unknown;

t_symbol *ps_buttons;
t_symbol *ps_motion;
t_symbol *ps_stick;
t_symbol *ps_stick1;
t_symbol *ps_stick2;
t_symbol *ps_analog;
t_symbol *ps_ir;
t_symbol *ps_irraw;
t_symbol *ps_tap;
t_symbol *ps_whammy;
t_symbol *ps_softness;

t_symbol *ps_battery;
t_symbol *ps_led;
t_symbol *ps_expansion;
t_symbol *ps_address;

t_symbol *ps_connected;
t_symbol *ps_disconnected;

t_symbol *ps_calibration;
t_symbol *ps_stick_calibration;
t_symbol *ps_motion_calibration;


int C74_EXPORT main()
{
	NumVersion				outSoftwareVersion;
	BluetoothHCIVersionInfo	outHardwareVersion;
	
	post("ajh.wiiremote 1.0 by Alex Harker (based on aka.wiiremote by Masayuki Akamatsu)");

	if (IOBluetoothGetVersion(&outSoftwareVersion, &outHardwareVersion)==kIOReturnSuccess)	
	{
		if (outSoftwareVersion.majorRev < 1 && outSoftwareVersion.minorAndBugRev < 0x63)
		{
			error("requires Bluetooth version 1.6.3 or later.");
			return 0;
		}
	}
	else
	{
		error("can't get Bluetooth version.");
		return 0;
	}

	this_class = class_new ("ajh.wiiremote",
							(method) ajhwiiremote_new, 
							(method)ajhwiiremote_free, 
							sizeof(t_ajhwiiremote), 
							0L, 
							A_GIMME,
							0);
	
	class_addmethod (this_class, (method)ajhwiiremote_report, "bang", 0);
	
	class_addmethod (this_class, (method)ajhwiiremote_address, "address", A_DEFSYM, 0L);
	class_addmethod (this_class, (method)ajhwiiremote_connect,"connect", 0);
	class_addmethod (this_class, (method)ajhwiiremote_disconnect,"disconnect",0);
	class_addmethod (this_class, (method)ajhwiiremote_poll,"poll", A_DEFLONG, 0);
	class_addmethod (this_class, (method)ajhwiiremote_motionsensor,"motion", A_DEFLONG, 0);
	class_addmethod (this_class, (method)ajhwiiremote_irsensor,"ir", A_DEFLONG, 0);
	class_addmethod (this_class, (method)ajhwiiremote_vibration,"vibration", A_DEFLONG, 0);
	class_addmethod (this_class, (method)ajhwiiremote_led,"led", A_DEFLONG, A_DEFLONG, A_DEFLONG, A_DEFLONG, 0);
	class_addmethod (this_class, (method)ajhwiiremote_expansion,"expansion", A_DEFLONG, 0);
	class_addmethod (this_class, (method)ajhwiiremote_motionplusmode,"motionplus", A_DEFLONG, 0);

	class_addmethod (this_class, (method)ajhwiiremote_getbattery, "getbattery",0);
	class_addmethod (this_class, (method)ajhwiiremote_getexpansion, "getexpansion",0);
	class_addmethod (this_class, (method)ajhwiiremote_getled,"getled",0);
	class_addmethod (this_class, (method)ajhwiiremote_getaddress, "getaddress",0);
	class_addmethod (this_class, (method)ajhwiiremote_getcalibration, "getcalibration", 0);
			
	class_addmethod(this_class, (method)ajhwiiremote_assist,"assist", A_CANT, 0);

	class_register(CLASS_BOX, this_class);

	ps_remote = gensym("remote");
	ps_classic = gensym("classic");
	ps_nunchuk = gensym("nunchuk");
	ps_guitar = gensym("guitar");
	ps_drums = gensym("drums");
	ps_motionplus = gensym("motionplus");
	ps_unknown = gensym("unknown");

	ps_buttons = gensym("buttons");
	ps_motion = gensym("motion");
	ps_stick = gensym("stick");
	ps_stick1 = gensym("stick1");
	ps_stick2 = gensym("stick2");
	ps_analog = gensym("analog");
	ps_irraw = gensym("irraw");
	ps_ir = gensym("ir");
	ps_tap = gensym("tap");
	ps_whammy = gensym("whammy");
	ps_softness = gensym("softness");
	
	
	ps_stick2 = gensym("stick2");
	ps_stick2 = gensym("stick2");
	
	ps_battery = gensym("battery");
	ps_led = gensym("led");
	ps_expansion = gensym("expansion");
	ps_address = gensym("address");

	ps_calibration = gensym("calibration");
	ps_stick_calibration = gensym("stick_calibration");
	ps_motion_calibration = gensym("motion_calibration");
	
	ps_connected = gensym("connected");
	ps_disconnected = gensym("disconnected");
	
	return 0;
}


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------


void ajhwiiremote_report_timed(t_ajhwiiremote *x)
{
	long poll = x->poll;
	ajhwiiremote_report(x);
	
	if (poll) clock_delay(x->data_clock, poll);
}


void ajhwiiremote_report(t_ajhwiiremote *x)
{
	long eButtonData, eStickX1, eStickY1, eStickX2, eStickY2, eCont1, eCont2, nStickX, nStickY, buttonData;
	
	t_atom av[17]; 
	
	if (x->wiiremote->device == nil)			// if there is no device do nothing
		return;								

	if (x->wiiremote->isExpansionPortEnabled && x->wiiremote->ExpBogus == false)
	{
		// Motion Plus
			
		if (WiiMotionPlusTest(x->wiiremote->expType))
		{
			atom_setfloat(av, x->wiiremote->mScaleGyroY);
			atom_setfloat(av + 1, x->wiiremote->mScaleGyroR);				
			atom_setfloat(av + 2, x->wiiremote->mScaleGyroP);
			atom_setfloat(av + 3, x->wiiremote->mGyroY);
			atom_setfloat(av + 4, x->wiiremote->mGyroR);				
			atom_setfloat(av + 5, x->wiiremote->mGyroP);
			atom_setlong(av + 6, x->wiiremote->mFastY);
			atom_setlong(av + 7, x->wiiremote->mFastR);
			atom_setlong(av + 8, x->wiiremote->mFastP);
			outlet_anything(x->dataOut, ps_motionplus, 9, av);
		}
			
		// Nunchuk
		
		if (x->wiiremote->expType == WiiNunchuk || x->wiiremote->expType == WiiMotionPlusNunchuk)
		{
			// Buttons
			
			eButtonData = x->wiiremote->eButtonData;
			
			if (eButtonData != x->last_eButtonData)
			{
				atom_setlong(av, ps_buttons);
				atom_setlong(av + 1, (0x2 - (eButtonData & 0x2)) >> 0x1);		// Button C
				atom_setlong(av + 2, (0x1 - (eButtonData & 0x1)));				// Button Z
				outlet_anything(x->dataOut, ps_nunchuk, 3, av);
			}
			x->last_eButtonData = eButtonData;
			
			// Joystick
			
			nStickX = x->wiiremote->eStickX1;
			nStickY = x->wiiremote->eStickY1;
			
			if (nStickX != x->last_nStickX || nStickY != x->last_nStickY)
			{
				atom_setsym(av, ps_stick);
				atom_setfloat(av + 1, x->wiiremote->eScaleStickX1);
				atom_setfloat(av + 2, x->wiiremote->eScaleStickY1);
				atom_setlong(av + 3, x->wiiremote->eStickX1);
				atom_setlong(av + 4, x->wiiremote->eStickY1);
				outlet_anything(x->dataOut, ps_nunchuk, 5, av);
			}
			
			x->last_nStickX = nStickX;
			x->last_nStickY = nStickY;
			
			// Motion Sensor
			
			if (x->wiiremote->isMotionSensorEnabled)
			{
				eButtonData = x->wiiremote->eButtonData;
				
				atom_setsym(av, ps_motion);
				atom_setfloat(av + 1, x->wiiremote->eScaleAccX);
				atom_setfloat(av + 2, x->wiiremote->eScaleAccY);
				atom_setfloat(av + 3, x->wiiremote->eScaleAccZ);
				atom_setlong(av + 4, x->wiiremote->eAccX);
				atom_setlong(av + 5, x->wiiremote->eAccY);
				atom_setlong(av + 6, x->wiiremote->eAccZ);
				atom_setlong(av + 7, x->wiiremote->eOrientation);
				outlet_anything(x->dataOut, ps_nunchuk, 8, av);
			}
		}
		
		// Classic Controller
		
		if (x->wiiremote->expType == WiiClassicController || x->wiiremote->expType == WiiMotionPlusClassic)
		{
			// Buttons
			
			eButtonData = x->wiiremote->eButtonData;
			
			if (eButtonData != x->last_eButtonData)
			{
				atom_setsym(av, ps_buttons);
				atom_setlong(av + 1, eButtonData & 0x0001);					// Button Up
				atom_setlong(av + 2, (eButtonData & 0x4000) >> 0xE);			// Button Down
				atom_setlong(av + 3, (eButtonData & 0x0002) >> 0x1);			// Button Left
				atom_setlong(av + 4, (eButtonData & 0x8000) >> 0xF);			// Button Right
				atom_setlong(av + 5, (eButtonData & 0x1000) >> 0xC);			// Button Minus
				atom_setlong(av + 6, (eButtonData & 0x0800) >> 0xB);			// Button Home
				atom_setlong(av + 7, (eButtonData & 0x0400) >> 0xA);			// Button Plus
				atom_setlong(av + 8, (eButtonData & 0x0008) >> 0x3);			// Button x
				atom_setlong(av + 9, (eButtonData & 0x0020) >> 0x5);			// Button y
				atom_setlong(av + 10, (eButtonData & 0x0010) >> 0x4);		// Button a
				atom_setlong(av + 11, (eButtonData & 0x0040) >> 0x6);		// Button b
				atom_setlong(av + 12, (eButtonData & 0x0080) >> 0x7);		// Button ZL
				atom_setlong(av + 13, (eButtonData & 0x0004) >> 0x2);		// Button ZR
				atom_setlong(av + 14, (eButtonData & 0x2000) >> 0xD);		// Button L Trigger
				atom_setlong(av + 15, (eButtonData & 0x0200) >> 0x9);		// Button R Trigger
				outlet_anything(x->dataOut, ps_classic, 16, av);			
			}
			x->last_eButtonData = eButtonData;

			eStickX1 = x->wiiremote->eStickX1;
			eStickY1 = x->wiiremote->eStickY1;
			eStickX2 = x->wiiremote->eStickX2;
			eStickY2 = x->wiiremote->eStickY2;
			eCont1 = x->wiiremote->eCont1;
			eCont2 = x->wiiremote->eCont2;
			
			// Joystick 1
			
			if (eStickX1 != x->last_eStickX1 || eStickY1 != x->last_eStickY1)
			{
				atom_setsym(av, ps_stick1);
				atom_setfloat(av + 1, x->wiiremote->eScaleStickX1);
				atom_setfloat(av + 2, x->wiiremote->eScaleStickY1);
				atom_setlong(av + 3, x->wiiremote->eStickX1);
				atom_setlong(av + 4, x->wiiremote->eStickY1);
				outlet_anything(x->dataOut, ps_classic, 5, av);
			}
			
			// Joystick 2
			
			if (eStickX2 != x->last_eStickX2 || eStickY2 != x->last_eStickY2)
			{
				atom_setsym(av, ps_stick2);
				atom_setfloat(av + 1, x->wiiremote->eScaleStickX2);
				atom_setfloat(av + 2, x->wiiremote->eScaleStickY2);
				atom_setlong(av + 3, x->wiiremote->eStickX2);
				atom_setlong(av + 4, x->wiiremote->eStickY2);
				outlet_anything(x->dataOut, ps_classic, 5, av);
			}
			
			// Analog
			
			if (eCont1 != x->last_eCont1 || eCont2 != x->last_eCont2)
			{
				atom_setsym(av, ps_analog);
				atom_setlong(av + 1, eCont1);
				atom_setlong(av + 2, eCont2);
				outlet_anything(x->dataOut, ps_classic, 3, av);
			}
			
			x->last_eStickX1 = eStickX1;
			x->last_eStickY1 = eStickY1;
			x->last_eStickX2 = eStickX2;
			x->last_eStickY2 = eStickY2;
			x->last_eCont1 = eCont1;
			x->last_eCont2 = eCont2;
		}
		
		// Guitar Hero Guitar
		
		if (x->wiiremote->expType == WiiGuitarHeroGuitar)
		{
			// Buttons
			
			eButtonData = x->wiiremote->eButtonData;
			
			if (eButtonData != x->last_eButtonData)
			{
				atom_setsym(av, ps_buttons);
				atom_setlong(av + 1, (eButtonData & 0x0004) >> 0x2);
				atom_setlong(av + 2, (eButtonData & 0x0010) >> 0x4);
				atom_setlong(av + 3, (eButtonData & 0x0040) >> 0x6);
				atom_setlong(av + 4, (eButtonData & 0x0080) >> 0x7);
				atom_setlong(av + 5, (eButtonData & 0x0100) >> 0x8);
				atom_setlong(av + 6, (eButtonData & 0x0800) >> 0xB);
				atom_setlong(av + 7, (eButtonData & 0x1000) >> 0xC);
				atom_setlong(av + 8, (eButtonData & 0x2000) >> 0xD);
				atom_setlong(av + 9, (eButtonData & 0x4000) >> 0xE);
				atom_setlong(av + 10, (eButtonData & 0x8000) >> 0xF);
				outlet_anything(x->dataOut, ps_guitar, 11, av);
			}
			x->last_eButtonData = eButtonData;

			eStickX1 = x->wiiremote->eStickX1;
			eStickY1 = x->wiiremote->eStickY1;
			eStickY2 = x->wiiremote->eStickY2;
			eCont2 = x->wiiremote->eCont2;
			
			// Joystick
			
			if (eStickX1 != x->last_eStickX1 || eStickY1 != x->last_eStickY1)
			{
				atom_setsym(av, ps_stick);
				atom_setlong(av + 1, eStickX1);
				atom_setlong(av + 2, eStickY1);
				outlet_anything(x->dataOut, ps_guitar, 3, av);
			}
			
			// Tap Bar
			
			if (eStickY2 != x->last_eStickY2)
			{
				atom_setsym(av, ps_tap);
				atom_setlong(av + 1, eStickY2);
				outlet_anything(x->dataOut, ps_guitar, 2, av);
			}
			
			// Whammy Bar
			
			if (eCont2 != x->last_eCont2)
			{
				atom_setsym(av, ps_whammy);
				atom_setlong(av + 1, eCont2);
				outlet_anything(x->dataOut, ps_guitar, 2, av);
			}
			
			x->last_eStickX1 = eStickX1;
			x->last_eStickY1 = eStickY1;
			x->last_eStickY2 = eStickY2;
			x->last_eCont2 = eCont2;
		}
		
		// Guitar Hero Drums
		
		if (x->wiiremote->expType == WiiGuitarHeroDrums)
		{
			// Buttons
			
			eButtonData = x->wiiremote->eButtonData;
			
			if (eButtonData != x->last_eButtonData)
			{
				atom_setsym(av, ps_buttons);
				atom_setlong(av + 1, (eButtonData & 0x0004) >> 0x2);
				atom_setlong(av + 2, (eButtonData & 0x0010) >> 0x4);
				atom_setlong(av + 3, (eButtonData & 0x0080) >> 0x7);
				atom_setlong(av + 4, (eButtonData & 0x0400) >> 0xA);
				atom_setlong(av + 5, (eButtonData & 0x0800) >> 0xB);
				atom_setlong(av + 6, (eButtonData & 0x1000) >> 0xC);
				atom_setlong(av + 7, (eButtonData & 0x2000) >> 0xD);
				atom_setlong(av + 8, (eButtonData & 0x4000) >> 0xE);
				atom_setlong(av + 9, (eButtonData & 0x8000) >> 0xF);
				outlet_anything(x->dataOut, ps_drums, 10, av);
			}
			x->last_eButtonData = eButtonData;

			eStickX1 = x->wiiremote->eStickX1;
			eStickY1 = x->wiiremote->eStickY1;
			eCont1 = x->wiiremote->eCont1;
			eCont2 = x->wiiremote->eCont2;
			
			// Joystick
			
			if (eStickX1 != x->last_eStickX1 || eStickY1 != x->last_eStickY1)
			{
				atom_setsym(av, ps_stick);
				atom_setlong(av + 1, eStickX1);
				atom_setlong(av + 2, eStickY1);
				outlet_anything(x->dataOut, ps_drums, 3, av);
			}
			
			// Softness
			
			if (eCont2)
			{
				atom_setsym(av, ps_softness);
				atom_setlong(av + 1, eCont2);
				atom_setlong(av + 1, eCont1);
				outlet_anything(x->dataOut, ps_drums, 3, av);
			}
			
			x->last_eStickX1 = eStickX1;
			x->last_eStickY1 = eStickY1;
		}
		
		// Unknown Controller
		
		if (WiiUnknownTest(x->wiiremote->expType))
		{
			// Data
			
			atom_setlong(av + 0, x->wiiremote->uData1);
			atom_setlong(av + 1, x->wiiremote->uData2);
			atom_setlong(av + 2, x->wiiremote->uData3);
			atom_setlong(av + 3, x->wiiremote->uData4);
			atom_setlong(av + 4, x->wiiremote->uData5);
			atom_setlong(av + 5, x->wiiremote->uData6);
			outlet_anything(x->dataOut, ps_unknown, 6, av);
		}
	}
	
	// Wii Remote

	// Buttons
	
	if (x->wiiremote->WiiButtonBogus == false) 
	{
		buttonData = x->wiiremote->buttonData;
			
		if (buttonData != x->last_buttonData)
		{
			atom_setsym(av, ps_buttons);
	
			atom_setlong(av + 1, (buttonData & 0x0800) >> 0xB);				// Button Up
			atom_setlong(av + 2, (buttonData & 0x0400) >> 0xA);				// Button Down
			atom_setlong(av + 3, (buttonData & 0x0100) >> 0x8);				// Button Left
			atom_setlong(av + 4, (buttonData & 0x0200) >> 0x9);				// Button Right
			atom_setlong(av + 5, (buttonData & 0x0008) >> 0x3);				// Button A
			atom_setlong(av + 6, (buttonData & 0x0004) >> 0x2);				// Button B
			atom_setlong(av + 7, (buttonData & 0x0010) >> 0x4);				// Button Minus
			atom_setlong(av + 8, (buttonData & 0x0080) >> 0x7);				// Button Home
			atom_setlong(av + 9, (buttonData & 0x1000) >> 0xC);				// Button Plus
			atom_setlong(av + 10, (buttonData & 0x0002) >> 0x1);				// Button 1
			atom_setlong(av + 11, (buttonData & 0x0001));					// Button 2
	
			outlet_anything(x->dataOut, ps_remote, 12, av);				
		}
		x->last_buttonData = buttonData;
	}

	// IR Sensor
	
	if (x->wiiremote->isIRSensorEnabled && x->wiiremote->WiiMotionBogus == false)
	{
		atom_setsym(av, ps_ir);
		atom_setfloat(av + 1, x->wiiremote->posX);			
		atom_setfloat(av + 2, x->wiiremote->posY);
		atom_setfloat(av + 3, x->wiiremote->angle);
		atom_setlong (av + 4, x->wiiremote->tracking);
		outlet_anything(x->dataOut, ps_remote, 5, av);
		
		// Raw Data
		
		atom_setsym(av, ps_irraw);
		atom_setlong(av + 1, 0);
		atom_setlong(av + 2, x->wiiremote->irData[0].x);
		atom_setlong(av + 3, x->wiiremote->irData[0].y);
		atom_setlong(av + 4, x->wiiremote->irData[0].s);
		outlet_anything(x->dataOut, ps_remote, 5, av);
		atom_setlong(av + 1, 1);
		atom_setlong(av + 2, x->wiiremote->irData[1].x);
		atom_setlong(av + 3, x->wiiremote->irData[1].y);
		atom_setlong(av + 4, x->wiiremote->irData[1].s);
		outlet_anything(x->dataOut, ps_remote, 5, av);
		atom_setlong(av + 1, 2);
		atom_setlong(av + 2, x->wiiremote->irData[2].x);
		atom_setlong(av + 3, x->wiiremote->irData[2].y);
		atom_setlong(av + 4, x->wiiremote->irData[2].s);
		outlet_anything(x->dataOut, ps_remote, 5, av);
		atom_setlong(av + 1, 3);
		atom_setlong(av + 2, x->wiiremote->irData[3].x);
		atom_setlong(av + 3, x->wiiremote->irData[3].y);
		atom_setlong(av + 4, x->wiiremote->irData[3].s);
		outlet_anything(x->dataOut, ps_remote, 5, av);
	}

	// Motion Sensor
	
	if (x->wiiremote->isMotionSensorEnabled && x->wiiremote->WiiMotionBogus == false && x->wiiremote->WiiCallibrationBogus == false)
	{
		buttonData = x->wiiremote->buttonData;
		
		atom_setsym(av, ps_motion);
		atom_setfloat(av + 1, x->wiiremote->accScaleX);
		atom_setfloat(av + 2, x->wiiremote->accScaleY);
		atom_setfloat(av + 3, x->wiiremote->accScaleZ);
		atom_setlong(av + 4, x->wiiremote->accX);
		atom_setlong(av + 5, x->wiiremote->accY);
		atom_setlong(av + 6, x->wiiremote->accZ);
		atom_setlong(av + 7, x->wiiremote->orientation);
		outlet_anything(x->dataOut, ps_remote, 8, av);
	}
}


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------


void ajhwiiremote_address(t_ajhwiiremote *x, t_symbol *s)
{
	if (*(s->s_name) == 0)	// if null string
	{
		*(x->address) = 0;
		x->use_address = 0;
	}
	else
	{
		strcpy(x->address, s->s_name);
		x->use_address = 1;
	}
}


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------


void ajhwiiremote_connect(t_ajhwiiremote *x)
{
	if (wiiremote_isconnected(x->wiiremote))
	{
		post ("Already Connected!!");
		outlet_anything(x->statusOut, ps_connected, 0, 0);		
	}
	else
	{
		ajhwiiremote_init(x);
		
		// Start searching the device
		
		wiiremote_search(x->wiiremote, x->use_address, x->address);					
	}
}


void ajhwiiremote_disconnect(t_ajhwiiremote *x)
{	
	clock_unset(x->data_clock);
	wiiremote_stopsearch(x->wiiremote);
	wiiremote_disconnect(x->wiiremote);
}


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------


void ajhwiiremote_poll(t_ajhwiiremote *x, long poll)
{
	x->poll = abs(poll);	
	if (poll)
	{
		clock_unset(x->data_clock);
		clock_delay(x->data_clock, 0);
	}
}


void ajhwiiremote_motionsensor(t_ajhwiiremote *x, long mode)
{
	Boolean enabled = mode ? true : false;		
	wiiremote_motionsensor(x->wiiremote, enabled);	
}


void ajhwiiremote_irsensor(t_ajhwiiremote *x, long enable)
{
	wiiremote_irsensor(x->wiiremote, enable);	
}


void ajhwiiremote_vibration(t_ajhwiiremote *x, long enable)
{
	wiiremote_vibration(x->wiiremote, enable);		
}


void ajhwiiremote_led(t_ajhwiiremote *x, long enable1, long enable2, long enable3, long enable4)
{
	wiiremote_led(x->wiiremote, enable1, enable2, enable3, enable4);	
}


void ajhwiiremote_expansion(t_ajhwiiremote *x, long enable)
{
	wiiremote_expansion(x->wiiremote, enable);
}


void ajhwiiremote_motionplusmode(t_ajhwiiremote *x, long mode)
{
	if (mode < 0) 
		mode = 0;
	if (mode >= NumOfMPModes) 
		mode = NumOfMPModes - 1;
		
	wiiremote_prefermotionplus(x->wiiremote, mode);
}


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------


void ajhwiiremote_getbattery(t_ajhwiiremote *x)
{
	if (x->wiiremote->device != nil)
	{
		t_atom	status;
		
		atom_setfloat(&status, x->wiiremote->batteryLevel);
		outlet_anything(x->statusOut, ps_battery, 1, &status);		
	}
}


void ajhwiiremote_getexpansion(t_ajhwiiremote *x)
{
	if (x->wiiremote->device != nil)
	{
		t_atom	status;
		
		if (x->wiiremote->expType != WiiExpNotAttached || x->wiiremote->expType != WiiExpUninitialised)
			atom_setlong(&status, x->wiiremote->expType - 1);
		else
			atom_setlong(&status, 0);
		outlet_anything(x->statusOut, ps_expansion, 1, &status);
	}
}


void ajhwiiremote_getled(t_ajhwiiremote *x)
{
	if (x->wiiremote->device != nil)
	{
		t_atom list[4]; 
		
		atom_setlong(list,     x->wiiremote->isLED1Illuminated);
		atom_setlong(list + 1, x->wiiremote->isLED2Illuminated);
		atom_setlong(list + 2, x->wiiremote->isLED3Illuminated);
		atom_setlong(list + 3, x->wiiremote->isLED4Illuminated);
		outlet_anything(x->statusOut, ps_led, 4, list);
	}
}


void ajhwiiremote_getcalibration(t_ajhwiiremote *x)
{
	if (x->wiiremote->device != nil)
	{
		t_atom list[8]; 
		
		switch (x->wiiremote->expType)
		{
			case WiiNunchuk:
		
				atom_setsym(list, ps_nunchuk);
				atom_setsym(list + 1, ps_stick);
				atom_setlong(list + 2, x->wiiremote->eStick1CalibData.x_max);
				atom_setlong(list + 3, x->wiiremote->eStick1CalibData.x_min);
				atom_setlong(list + 4, x->wiiremote->eStick1CalibData.x_center);
				atom_setlong(list + 5, x->wiiremote->eStick1CalibData.y_max);
				atom_setlong(list + 6, x->wiiremote->eStick1CalibData.y_min);
				atom_setlong(list + 7, x->wiiremote->eStick1CalibData.y_center);
				outlet_anything(x->statusOut, ps_calibration, 8, list);
				
				atom_setsym(list + 1, ps_motion);
				atom_setlong(list + 2, x->wiiremote->eAccCalibData.accX_zero);
				atom_setlong(list + 3, x->wiiremote->eAccCalibData.accY_zero);
				atom_setlong(list + 4, x->wiiremote->eAccCalibData.accZ_zero);
				atom_setlong(list + 5, x->wiiremote->eAccCalibData.accX_1g);
				atom_setlong(list + 6, x->wiiremote->eAccCalibData.accY_1g);
				atom_setlong(list + 7, x->wiiremote->eAccCalibData.accZ_1g);
				outlet_anything(x->statusOut, ps_calibration, 8, list);
				
				break;
				
			case WiiClassicController:
		
				atom_setsym(list, ps_classic);
				atom_setsym(list + 1, ps_stick1);
				atom_setlong(list + 2, x->wiiremote->eStick1CalibData.x_max);
				atom_setlong(list + 3, x->wiiremote->eStick1CalibData.x_min);
				atom_setlong(list + 4, x->wiiremote->eStick1CalibData.x_center);
				atom_setlong(list + 5, x->wiiremote->eStick1CalibData.y_max);
				atom_setlong(list + 6, x->wiiremote->eStick1CalibData.y_min);
				atom_setlong(list + 7, x->wiiremote->eStick1CalibData.y_center);
				outlet_anything(x->statusOut, ps_calibration, 8, list);
				
				atom_setsym(list, ps_classic);
				atom_setsym(list + 1, ps_stick2);
				atom_setlong(list + 2, x->wiiremote->eStick2CalibData.x_max);
				atom_setlong(list + 3, x->wiiremote->eStick2CalibData.x_min);
				atom_setlong(list + 4, x->wiiremote->eStick2CalibData.x_center);
				atom_setlong(list + 5, x->wiiremote->eStick2CalibData.y_max);
				atom_setlong(list + 6, x->wiiremote->eStick2CalibData.y_min);
				atom_setlong(list + 7, x->wiiremote->eStick2CalibData.y_center);
				outlet_anything(x->statusOut, ps_calibration, 8, list);
				break;
			
			default:
				break;
		}
		
		atom_setsym(list, ps_remote);
		atom_setsym(list +  1, ps_motion);
		atom_setlong(list + 2, x->wiiremote->wiiCalibData.accX_zero);
		atom_setlong(list + 3, x->wiiremote->wiiCalibData.accY_zero);
		atom_setlong(list + 4, x->wiiremote->wiiCalibData.accZ_zero);
		atom_setlong(list + 5, x->wiiremote->wiiCalibData.accX_1g);
		atom_setlong(list + 6, x->wiiremote->wiiCalibData.accY_1g);
		atom_setlong(list + 7, x->wiiremote->wiiCalibData.accZ_1g);
		outlet_anything(x->statusOut, ps_calibration, 8, list);
	}
}


void ajhwiiremote_getaddress(t_ajhwiiremote *x)
{
	if (x->wiiremote->device != nil)
	{
		char	str[32];
		t_atom	address;
		
		wiiremote_getaddress(x->wiiremote, str);
		atom_setsym(&address, gensym(str));
		outlet_anything(x->statusOut, ps_address, 1, &address);		
	}
}


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------


void ajhwiiremote_connection(t_ajhwiiremote *x, t_symbol *s, short argc, t_atom *argv)
{	
	Boolean connection = wiiremote_isconnected(x->wiiremote);
	
	clock_unset(x->data_clock);
	
	if (connection == true)																	
	{
		// If the device is connected...
		
		if (x->poll) clock_delay(x->data_clock, 0);
		outlet_anything(x->statusOut, ps_connected, 0L, 0);
	}
	else
		outlet_anything(x->statusOut, ps_disconnected, 0L, 0);
}


void ajhwiiremotre_notify (void *x, void *wiiremote, WiimoteCallbackType Type, char *String)
{
	switch (Type)
	{
		case WiiPost:
			
			post (String);
			break;
			
		case WiiErr:
		
			error (String);
			break;
			
		case WiiDebug:
		
			#ifdef WII_DEBUG 
			post (String);
			#endif
			break;
			
		case WiiConnection:
			schedule_delay(x, (method) ajhwiiremote_connection, 0 , 0, 0, 0L);
			break;
			
		case WiiExpansionChange:
			
			switch (((WiiRemoteRef) wiiremote)->expType)
			{
				case WiiExpNotAttached:			post ("expansion change - not attached");							break;
				case WiiExpUninitialised:		post ("expansion change - not intialised");							break;
				case WiiNunchuk:				post ("expansion change - nunchuk");								break;
				case WiiClassicController:		post ("expansion change - classic controller");						break;
				case WiiGuitarHeroGuitar:		post ("expansion change - guitar hero guitar");						break;
				case WiiGuitarHeroDrums:		post ("expansion change - guitar hero drums");						break;
				case WiiMotionPlus:				post ("expansion change - motion plus");							break;
				case WiiMotionPlusNunchuk:		post ("expansion change - motion plus with nunchuk");				break;
				case WiiMotionPlusClassic:		post ("expansion change - motion plus with classic controller");	break;
				case WiiExpNotTested:			post ("expansion change - not tested");								break;
				default:						post ("expansion change - unknown device %x", ((WiiRemoteRef) wiiremote)->expType);
			}
			break;
	}
}


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------


void ajhwiiremote_init(t_ajhwiiremote *x)
{
	x->last_buttonData = 0xFFFFFFFF;
	ajhwiiremote_exp_init(x);
}


void ajhwiiremote_exp_init(t_ajhwiiremote *x)
{
	x->last_eButtonData = 0xFFFFFFFF;
	x->last_eButtonData = 0xFFFFFFFF;
	
	x->last_nStickX = 0xFFFF;
	x->last_nStickY = 0xFFFF;
	x->last_eStickX1 = 0xFFFF;
	x->last_eStickY1 = 0xFFFF;
	x->last_eStickX2 = 0xFFFF;
	x->last_eStickY2 = 0xFFFF;
	x->last_eCont1 = 0xFFFF;
	x->last_eCont2 = 0xFFFF;
}


void *ajhwiiremote_new(t_symbol *s, short ac, t_atom *av)
{
	t_ajhwiiremote *x;
	
	x = (t_ajhwiiremote *)object_alloc(this_class);
	
	x->wiiremote = wiiremote_create(ajhwiiremotre_notify, x);
	x->data_clock = clock_new(x, (method)ajhwiiremote_report_timed);
	
	ajhwiiremote_motionsensor(x, 1);
	
	if (ac > 0 && atom_gettype(av) == A_SYM)
	{
		strcpy(x->address, atom_getsym(av)->s_name);
		x->use_address = 1;
	}
	else
	{
		x->address[0] = 0;
		x->use_address = 0;
	}
	
	ajhwiiremote_init(x);
	
	x->statusOut = outlet_new(x, 0);
	x->dataOut = outlet_new(x, 0);
	
	x->poll = 10;

	return x;
}


void ajhwiiremote_free(t_ajhwiiremote *x)
{
	freeobject((t_object *)x->data_clock);
	wiiremote_destroy(x->wiiremote);
}


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------


void ajhwiiremote_assist(t_ajhwiiremote *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET)
	{
		sprintf(s,"connect, bang, disconnect....");
	}
	else  
	{
		switch(a)
		{
			case 0: sprintf(s,"data messages"); break;
			case 1: sprintf(s,"status messages"); break;
		}
	}
}