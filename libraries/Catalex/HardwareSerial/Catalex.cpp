/********************************************************/
/*                                                      */
/*                 MP3 Source File                      */
/*                                                      */
/********************************************************/

#include "Catalex.h"

#define DEBUG

/******************************************************/
/*             Constants and Variables                */
/******************************************************/
static int8_t Send_buf[8] = {0}; // Buffer for Send commands.  // BETTER LOCALLY
static uint8_t ansbuf[10] = {0}; // Buffer for the answers.    // BETTER LOCALLY
static int8_t volume = 0x0f; // Volume. 0-30 DEC values. 0x0f = 15.
static int8_t min_vol = 0;
static int8_t max_vol = 30;


/********************************************************************************/
/*Class methods*/

// < constructor >
Catalex::Catalex()
{
	ser = NULL;
}

// < Destructor >
Catalex::~Catalex()
{
	ser->flush();
	ser = NULL;
}

void Catalex::setup()
{
	setup(&Serial, 9600);
}

void Catalex::setup(HardwareSerial* serial, int baud=9600)
{
	ser = serial;
	ser->begin(baud);
	#ifdef DEBUG
		Serial.print("[CATALEX] Set device storage: ");
	#endif // DEBUG
	sendCommand(CMD_SEL_DEV, 0x0002);		// Storage set

	#ifdef DEBUG
		Serial.print("[CATALEX] Wake device: ");
	#endif // DEBUG
	sendCommand(CMD_WAKE_UP, 0x0000);		// Wake

	#ifdef DEBUG
		Serial.print("[CATALEX] Set Volume: ");
	#endif // DEBUG
	setVolume(volume);						// Initial volume

}

int8_t Catalex::volumeUp()
{
	volume += 1;
	setVolume(volume);
	return volume;
}

int8_t Catalex::volumeDown()
{
	volume -= 1;
	setVolume(volume);
	return volume;
}

void Catalex::setVolume(int16_t vol)
{
	vol = constrain(vol, min_vol, max_vol);
	sendCommand(CMD_SET_VOLUME, vol);
	volume = vol;  // This updates the member, in case this function was called manually.
}

void Catalex::play(int16_t track)
{
	sendCommand(CMD_PLAY_FOLDER_FILE, track);
}


/********************************************************************************/
/*Function getResponse: Decode response from MP3 board.                         */
/*Parameter:-void                                                               */
/*Return: A string containing human-readable description of response            */
String Catalex::getResponse()
{
	String decodedMP3Answer = "";

	decodedMP3Answer += sanswer();

	switch (ansbuf[3]) {
		case 0x3A:
			decodedMP3Answer += " -> Memory card inserted";
			break;

		case 0x3D:
			decodedMP3Answer += " -> Completed playing file index " + String(ansbuf[6], DEC);
			//MP3_Playing = false;
			break;

		case 0x4C:
			decodedMP3Answer += " -> Playing file index number " + String(ansbuf[6], DEC);
			break;

		case 0x41:
			decodedMP3Answer += " -> Data received correctly";
			break;

	}

	return decodedMP3Answer;
}


/********************************************************************************/
/*Function: Ask MP3 board if it is currently playing something                  */
/*Parameter:                                                                    */
/*Returns: true if something is playing, otherwise false                        */

boolean Catalex::isPlaying()
{
	Send_buf[0] = 0x7e;   // Start
	Send_buf[1] = 0xff;   // Version
	Send_buf[2] = 0x06;   // Len 
	Send_buf[3] = 0x42;   // Query
	Send_buf[4] = 0x00;   // 0x00 No feedback, 0x01 feedback
	Send_buf[5] = 0x00;   // Data high
	Send_buf[6] = 0x00;   // Data low
	Send_buf[7] = 0xef;   // 
	for (uint8_t i = 0; i<8; i++)
		ser->write(Send_buf[i]);

	String foo = sanswer();
	// Probably should check here to see if response is actually to this query.
	// Response: 7E-FF-06-42-00-02-00-FE-B7-EF

	return ansbuf[6] != 0x00;
}



/********************************************************************************/
/*Function: Send command to the MP3                                             */
/*Parameter:-int8_t command to send                                             */
/*Parameter:-int16_ data parameter for the command                              */

void Catalex::sendCommand(int8_t command, int16_t dat)
{
	#ifdef DEBUG
		String CommandSent = "Catalex::sendCommand says command was ";
	#endif // DEBUG

    Send_buf[0] = 0x7e;					//
    Send_buf[1] = 0xff;					//
    Send_buf[2] = 0x06;					// Len 
    Send_buf[3] = command;				//
    Send_buf[4] = 0x01;					// 0x00 No feedback, 0x01 Yes feedback
    Send_buf[5] = (int8_t)(dat >> 8);	// data high byte
    Send_buf[6] = (int8_t)(dat);		// data low byte
    Send_buf[7] = 0xef;					//

	for (uint8_t i = 0; i < 8; i++)
	{
		ser->write(Send_buf[i]);
		#ifdef DEBUG
			CommandSent += (sbyte2hex(Send_buf[i]));	 // Testing
		#endif // DEBUG
	}

	#ifdef DEBUG
		Serial.println(CommandSent);					// Testing
	#endif // DEBUG
	
	// Ouput response
	/*if (ser->available() >=10 )
	{
		CommandSent = "Catalex::getResponse: '";
		CommandSent += getResponse();
		CommandSent += "'.";
		Serial.println(CommandSent);
	}*/
}


/********************************************************************************/
/*Function: sbyte2hex. Returns 1-2 byte data in HEX format.                     */
/*Parameter:- uint8_t b. Byte to convert to HEX.                                */
/*Return: String representing hex value: e.g. input 15, output "0X0F "           */

String Catalex::sbyte2hex(uint8_t b)
{
    String shex;
    
    shex = "0x";
    if (b < 16)
        shex += "0";
    
    shex += String(b,HEX);
    shex += " ";

    return shex;
}



/********************************************************************************/
/*Function: sanswer. Returns a String answer from mp3 UART module.              */
/*Parameter:- uint8_t b. void.                                                  */
/*Return: String. If the answer is well formated answer.                        */

String Catalex::sanswer(void)
{
	uint8_t i = 0;
	String mp3answer="";

	while (ser->available() && (i < 10))
	{
		uint8_t b = ser->read();
		ansbuf[i] = b;
		i++;
        
		mp3answer += sbyte2hex(b);

	}
    
    // if the answer format is correct.
    if ((ansbuf[0] == 0x7E) && (ansbuf[9] == 0xEF)) 
	{
		return mp3answer;
    }
    
    // Answer format incorrect
    return "???: "+mp3answer;
}



