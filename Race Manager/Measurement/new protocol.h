#ifndef PROTOCOL_H_INSERTED
#define PROTOCOL_H_INSERTED

/*
--------------------------------------------------------------------------------

Komunikacni protokol pro krabici.
	
VERZE: 02
	
ZMENY:
	02: - Zrusen prikaz GET_ALL_SENSORS.
		 - Pridano nulovani vsech senzoru prikazem RESET_SENSORS.
	    - Komunikaci je nyni mozno zabezpecit proti analize:
	    		- je mozno nastavit XOR masku aplikovanou nasledne na pakety,
	    		  prikaz SET_MASK;
	    		- je mozno posilat pakety s nesmyslnou hlavickou, pokud bude
	    		  souhlasit CRC budou pakety posilany zarizenim zpet a vytvori
	    		  "mlhu";
	    		- dalsi info viz "Schema komunikace" a popis prikazu SET_MASK.

POPIS:
	Komunikuje se po lince RS-232, rychlost 19200 baud, bez parity, rizeni toku
	zadne.

	Data jsou prenasena v paketech o velikosti 6 Bytu.
	
	Struktura paketu:
	HEADER (1 Byte)  +  DATA (4 Byty) + CRC (1 Byte).

	HEADER udava typ dat. Data se vzdy vejdou do jednoho paketu. CRC se pocita
	z casti HEADER + DATA, polynom je 10000 0111.

	Vypocet CRC:

	const unsigned char gpLookupTable[] = {
	0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
	0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
	0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
	0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
	0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
	0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
	0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
	0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
	0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
	0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
	0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
	0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
	0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
	0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
	0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
	0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
	0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
	0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
	0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
	0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
	0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
	0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
	0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
	0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
	0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
	0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
	0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
	0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
	0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
	0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
	0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
	0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3};
	
	unsigned char inline CRC8(pBuffer[])
	{
		CRC8 = gpLookupTable[gpLookupTable[gpLookupTable[gpLookupTable[
			gpLookupTable[pBuffer[0]]^pBuffer[1]]^pBuffer[2]]^pBuffer[3]]^
			pBuffer[4]]; // A tak dale, mozno expandovat do For smycky
	};

	Schema komunikace:
	
	1) Zarizeni nasloucha.
	2) Host (PC) vysle paket, zarizeni paket prijme, dekoduje, zkontroluje CRC.
	3) Pokud behem prijmu, dekodovani nebo kontroly CRC nastala chyba, je paket
		zapomenut a nastava navrat do bodu 1). Pokud jsou ihned po sobe prijmuty
		dva pakety se spatnym CRC, je navic proveden reset kodovaciho klice.
	4) Zarizeni se pokusi identifikovat a provest prikaz.
	5) Zpet do PC budou poslana data pozadovana prikazem. Pokud zadna data
		pozadovana nejsou, bude zpet odeslan paket prijmuty v bode 2. Paket bude
		zakodovan stejnym zpusobem jako paket prichozi.
	6) Navrat bo bodu 1).

	Komunikace probiha vzdy simplexne. Pokud neni explicitne receno, ze zarizeni
	nasloucha, je prijem vypnuty. Pokud zarizeni nasloucha, nezacne samostatne
	vysilat.
	
	Prijem paketu je ukoncen, pokud byl prijat posledni byte nebo pokud od prijmu
	minuleho, ne posledniho bytu uplynulo vice nez IO_TIMEOUT ms (chyba behem
	prijmu). Toto lze implementovat i na strane PC.

	Prikazy:
	-----------------------------------------------------------------------------

	Hlavicka					Data
	SET_CONTROLS			1. Byte - 0x01 Napajeni zapni,   0x00 Napajeni vypni
								2. Byte - 0x01 Hodiny zapni,     0x00 Hodiny vypni
								3. Byte - 0x01 Casovac zapni,    0x00 Casovac vypni
								4. Byte - 0x01 Invertuj senzory, 0x00 Neinvertuj senzory
								
	Vyznam:
	Nastav kontrolni bity.
	
	Hodiny funguji stejne jako stopky. Velikost 32 bitu, Citaji nahoru po 1 ms,
	mozno spustit, zastavit, nastavit udaj.
	
	Casovac je spojen se spinacem napajeni. Velikost 32 bitu, Cita dolu po 1 ms,
	mozno spustit, zastavit, nastavit udaj. Po dosazeni nuly se Casovac zastavi
	a vypne spinac napajeni.
	
	Senzory tvori 8 Komparatoru. Mozno nastavit spinani pri 0-1 nebo 1-0.

	-----------------------------------------------------------------------------
	
	Hlavicka					Data
	GET_CONTROLS			????
	
	Vyznam:
	Odesli zpet nastaveni kontrolnich bitu.
	
	Hlavicka zpet odeslaneho paketu bude totozna (GET_CONTROLS), format 
	odeslanych dat	bude odpovidat formatu dat s hlavickou SET_CONTROLS.
	
	-----------------------------------------------------------------------------
	
	SET_CLOCK, GET_CLOCK, SET_TIMER, GET_TIMER - to je snad jasny.

	-----------------------------------------------------------------------------
	
	Hlavicka
	RESET_SENSORS
		
	Data: ????
	
	Vyznam:
	Vynuluj vsechny registry casu posledniho sepnuti senzoru.
	
	-----------------------------------------------------------------------------
	
	Hlavicka					Data
	SET_MASK					1. Byte - Maska hlavicky
								2. Byte
									0. bit - maska 1. bytu dat (0000000x)
									1. bit - maska 2. bytu dat (0000000x)
								3. Byte - Maska 3. bytu dat
								4. Byte - Maska 4. bytu dat

	Vyznam:
	Nastav klic pro kodovani komunikace. Novy klic bude aplikovan az od pristiho
	paketu, tento bude poslan zpet nezmenen.
		Zarizeni pouziva 5 bytovy klic pro kodovani/dekodovani vsech
	vysilanych/prijimanych paketu. Kodovaci algoritmus je (PAKET XOR MASKA)
	bit po bitu, pocatecni klic je 0x0000000000. Druhy a treti byte klice je
	vzdy nulovy az na 0. bit, ktery lze nastavit.
		CRC se posila nezakodovane a tyka se paketu pred zakodovanim. Pokud
	zarizeni prijme 2x za sebou paket se spatnym CRC, je klic nastaven zpet
	na same nuly.

	-----------------------------------------------------------------------------

	Hlavicka
	GET_SENSOR0, GET_SENSOR1, GET_SENSOR2, GET_SENSOR3,
	GET_SENSOR4, GET_SENSOR5, GET_SENSOR6, GET_SENSOR7
	
	Data: ????
	
	Vyznam:
	Odesli zpet cas posledniho sepnuti senzoru.
	
	Hlavicka zpet odeslaneho paketu bude totozna, data budou tvorena 32 bity
	udaje hodin v case sepnuti. 

	-----------------------------------------------------------------------------
      
*/

#define SET_CONTROLS					0x01
#define GET_CONTROLS					0x02
#define SET_CLOCK						0x03
#define GET_CLOCK						0x04
#define SET_TIMER						0x05
#define GET_TIMER						0x06
#define RESET_SENSORS				0x07
#define SET_MASK						0x08
#define GET_SENSOR0					0xF0
#define GET_SENSOR1					0xF1
#define GET_SENSOR2					0xF2
#define GET_SENSOR3					0xF3
#define GET_SENSOR4					0xF4
#define GET_SENSOR5					0xF5
#define GET_SENSOR6					0xF6
#define GET_SENSOR7					0xF7

// Dal jen interni hodnoty, nedulezity

#define OUTPUT_READY					0x21
#define OUTPUT_CLEAR					0x22
#define INPUT_READY					0x23
#define INPUT_CLEAR					0x24
#define OUTPUT_DONE					0x25
#define INPUT_HEADER_INVALID		0x26
#define INPUT_CRC_INVALID			0x27
#define BREAK							0x00

#define SENSOR_HEADER_MASK			0xF0

#define IO_BUFFER_LENGTH			6
#define IO_MASK_LENGTH				5

#define HEADER_POSITION				0
#define POWER_ON_POSITION			1
#define CLOCK_ON_POSITION			2
#define TIMER_ON_POSITION			3
#define INV_SENS_POSITION			4
#define CRC_POSITION					5

#define IO_TIMEOUT					10

#endif
