/********************************************************/
/*                                                      */
/*                 MP3 Header File                      */
/*                                                      */
/********************************************************/

#ifndef HEADER_CATALEX
    #define HEADER_CATALEX
    
	#if defined(ARDUINO) && ARDUINO >= 100
		#include "Arduino.h"
	#else
		#include "WProgram.h"
	#endif

	#include <SoftwareSerial.h>
    
    /************ Command byte **************************/
    #define CMD_NEXT_SONG     0X01  // Play next song.
    #define CMD_PREV_SONG     0X02  // Play previous song.
    #define CMD_PLAY_W_INDEX  0X03
    #define CMD_VOLUME_UP     0X04
    #define CMD_VOLUME_DOWN   0X05
    #define CMD_SET_VOLUME    0X06

    #define CMD_SNG_CYCL_PLAY 0X08  // Single Cycle Play.
    #define CMD_SEL_DEV       0X09
    #define CMD_SLEEP_MODE    0X0A
    #define CMD_WAKE_UP       0X0B
    #define CMD_RESET         0X0C
    #define CMD_PLAY          0X0D
    #define CMD_PAUSE         0X0E
    #define CMD_PLAY_FOLDER_FILE 0X0F

    #define CMD_STOP_PLAY     0X16
    #define CMD_FOLDER_CYCLE  0X17
    #define CMD_SHUFFLE_PLAY  0x18 //
    #define CMD_SET_SNGL_CYCL 0X19 // Set single cycle.

    #define CMD_SET_DAC       0X1A
    #define DAC_ON            0X00
    #define DAC_OFF           0X01
      
    #define CMD_PLAY_W_VOL    0X22
    #define CMD_PLAYING_N     0x4C
    #define CMD_QUERY_PLAY    0x42

    /************ Options **************************/  
    #define DEV_TF            0X02  
    #define SINGLE_CYCLE_ON   0X00
    #define SINGLE_CYCLE_OFF  0X01
    

    /******************************************************/
    /*             Constants and Variables                */
    /******************************************************/


    
    /******************************************************/
    /*Class Declaration                                   */

	class Catalex
	{
		public:
			Catalex();
			~Catalex();
			//void setup();
			void setup(SoftwareSerial* serial, int baud);
			int8_t volumeUp();
			int8_t volumeDown();
			void setVolume(int16_t);			// Changes the volume setting( 0 - 30 decimal).
			void play(int16_t sound);
			String getResponse();				// Return the board's response to the last command
			boolean isPlaying();				// Return whether a sound is currently playing

		private:
			// < Local attributes >
			boolean mp3Playing = false;			// Is something playing?

			// Hardware
			SoftwareSerial* ser;	// Serial port
			
			// Methods
			void sendCommand(int8_t, int16_t);
			String sanswer(void);
			String sbyte2hex(uint8_t);
			
	};

#endif // HEADER_CATALEX
