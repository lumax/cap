
// Kommandofile V1.1 07.09.2011 bru

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


