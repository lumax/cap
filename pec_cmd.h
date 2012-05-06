
// Kommandofile V1.1 07.09.2011 bru

#ifndef __PEC_CMD_h__
#define __PEC_CMD_h__

#define nPEC_ASEND_ON	0x20	// Autosend der Daten einschalten
#define nPEC_ASEND_OFF	0x21	// Autosend der Daten ausschalten

#define nPEC_RESET_Q1	0x30	// Reset X1
#define nPEC_RESET_Q2	0x31	// Reset X2
#define nPEC_RESET_Z1	0x32	// Reset Z1
#define nPEC_RESET_Z2	0x33	// Reset Z2

#define nPEC_GET_Q1		0x34	// Position X1 abfragen int16 0-0xFFFF
#define nPEC_GET_Q2		0x35	// Position X2 abfragen int16 0-0xFFFF
#define nPEC_GET_Z1		0x36	// Position Z1 abfragen int16 0-0xFFFF
#define nPEC_GET_Z2		0x37	// Position Z2 abfragen int16 0-0xFFFF

#define nPEC_GET_FP1	0x40	// Position F1 abfragen int16 0-0x3FF
#define nPEC_GET_FP2	0x41	// Position F2 abfragen int16 0-0x3FF

#define nPEC_RESET 		0x50	// System Reset
#define nPEC_SWVERSION	0x51	// Softwareversion int16
#define nPEC_HWVERSION	0x52	// Hardwareversion int16
#define nPEC_NAME       0x53

#define nPEC_SETQMAX1   0x60
#define nPEC_SETQMAX2   0x61
#define nPEC_GETQMAX1   0x62
#define nPEC_GETQMAX2   0x63

#define nPEC_LIGHTON    0x64
#define nPEC_LIGHTOFF   0x65

#define nPEC_KEYON      0x66
#define nPEC_KEYHELP    0x67
#endif
