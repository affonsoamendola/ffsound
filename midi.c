#include <stdio.h>
#include <stdlib.h>

#include <sb.h>
#include <util.h>

#include <midi.h>

typedef struct SB_MIDI_NOTE
{
	int octave;
	NOTE note;

} SB_MIDI_NOTE;

typedef struct SB_MIDI_CHUNK_
{
	char[4] type;
	unsigned long int length;

} SB_MIDI_CHUNK;

typedef struct SB_MIDI_HEADER_
{
	SB_MIDI_CHUNK header_chunk;      //type:   Is always MThd, this doesnt *need* to be here but I put so I can easily write a header to a file by just flushing the struct to it.
								     //length: Is always 6, big endian.
	unsigned int format; 	    // 0 = single multi-channel track
							    // 1 = one or motre simultaneous tracks of a sequence.
							    // 2 = one or more sequentially independent single track patterns.
	unsigned int ntrks;	 	    // Number of track chunks in the file.
	unsigned int division;      // Has a complicated meaning

	/*  Division Explanation
		The third word, <division>, specifies the meaning of the delta-times. It has two formats, 
		one for metrical time, and one for time-code-based time:

		bit 15 | bits 14 thru 8	        |   bits 7 thru 0
		0 	   | ticks per quarter-note
		1 	   | negative SMPTE format 	|   ticks per frame

		If bit 15 of <division> is zero, the bits 14 thru 0 represent the number of delta time "ticks" 
		which make up a quarter-note. For instance, if division is 96, then a time interval of an eighth-note 
		between two events in the file would be 48.

		If bit 15 of <division> is a one, delta times in a file correspond to subdivisions of a second, in a 
		way consistent with SMPTE and MIDI Time Code. Bits 14 thru 8 contain one of the four 
		values -24, -25, -29, or -30, corresponding to the four standard SMPTE and MIDI Time Code formats 
		(-29 corresponds to 30 drop frame), and represents the number of frames per second. These negative 
		numbers are stored in two's compliment form. The second byte (stored positive) is the resolution within
		a frame: typical values may be 4 (MIDI Time Code resolution), 8, 10, 80 (bit resolution), or 100. This 
		stream allows exact specifications of time-code-based tracks, but also allows millisecond-based tracks
		by specifying 25 frames/sec and a resolution of 40 units per frame. If the events in a file are stored
		with a bit resolution of thirty-frame time code, the division word would be E250 hex. 

		From http://www.music.mcgill.ca/~ich/classes/mumt306/StandardMIDIfileformat.html
	 */
} SB_MIDI_HEADER;

typedef unsigned long int sb_variable_length;

typedef struct SB_MIDI_TRACK_
{
	SB_MIDI_CHUNK 	track_chunk;		//Chunk type is always MTrk for this one, and length varies.
	char* 			track_stream;
	char* 			next_event;

} SB_MIDI_TRACK;

typedef struct SB_MIDI_MESSAGE_
{
	char status;
	char data;
	char secondary_data;

}SB_MIDI_MESSAGE;

char is_playing_midi;

SB_MIDI_HEADER loaded_midi_header;
SB_MIDI_TRACK** loaded_midi_tracks;

unsigned long int current_tempo = 0;

void (interrupt far * old_timer_isr)();

void sb_install_timer_handler()			//Install the timer handler
{
	old_timer_isr = _dos_getvect(SB_TIMER_INTERRUPT);

	_dos_setvect(SB_TIMER_INTERRUPT, sb_timer_handler_driver);
}

void interrupt far sb_timer_handler()	//Gets called when one of the timers on the sound_board
										//overflows, should reset the timer flags, and calls the
										//midi functions needed.
{
	char fm_status = sb_fm_read_status();

	if(fm_status & 0x80)		//If timer interrupt bit is set, handle the interrupt, else gtfo.
	{
		if(fm_status & 0x40) 	//The midi system uses Timer 1 to check when to read a new event from
								//the file;
		{
			if(midi_playing)
			{
				sb_midi_next_event();
			}

			sb_fm_select_register(0x04);
			sb_fm_write_data_register(0x80);
		}
	}

	old_timer_isr(); //Chain the old timer ISR
}

int sb_load_midi_file(const char * file_path)
{
	int t = 0;
	int i = 0;
	FILE * midi_file;

	midi_file = fopen(file_path, "r");

	if(fgetc(midi_file) != 'M') return SB_ERR_READING_MIDI_FILE;
	if(fgetc(midi_file) != 'T') return SB_ERR_READING_MIDI_FILE;
	if(fgetc(midi_file) != 'h') return SB_ERR_READING_MIDI_FILE;
	if(fgetc(midi_file) != 'd') return SB_ERR_READING_MIDI_FILE; 	//Checks if header signature matches.

	loaded_midi_header.header_chunk.type = "MThd";

	if(fgetc(midi_file) != 0) return SB_ERR_READING_MIDI_FILE;
	if(fgetc(midi_file) != 0) return SB_ERR_READING_MIDI_FILE;
	if(fgetc(midi_file) != 0) return SB_ERR_READING_MIDI_FILE;
	if(fgetc(midi_file) != 6) return SB_ERR_READING_MIDI_FILE;	//Checks if the header length matches (Basically a secondary signature)

	loaded_midi_header.header_chunk.length = 6;

	loaded_midi_header.format = fgetc(midi_file);
	loaded_midi_header.format = (loaded_midi_header.format << 8) | fgetc(midi_file);	//Gets the format

	loaded_midi_header.ntrks = fgetc(midi_file);
	loaded_midi_header.ntrks = (loaded_midi_header.ntrks << 8) | fgetc(midi_file);	//Gets the number of tracks

	loaded_midi_header.division = fgetc(midi_file);
	loaded_midi_header.division = (loaded_midi_header.division << 8) | fgetc(midi_file);	//Gets the division number see explanation on the struct for wtf it is, I STILL DONT UNDERSTAND IT VERY WELL FUCK MY LIFE.

	loaded_midi_tracks = malloc(loaded_midi_header.ntrks * sizeof(SB_MIDI_TRACK *));

	for(t = 0; t < loaded_midi_header.ntrks; t++) //If there are multiple tracks they should be right after one another
	{
		unsigned long int length = 0;
		char * track_stream;

		*(loaded_midi_tracks + t) = malloc(sizeof(SB_MIDI_TRACK));		
		
		if(fgetc(midi_file) != 'M') return SB_ERR_READING_MIDI_FILE;
		if(fgetc(midi_file) != 'T') return SB_ERR_READING_MIDI_FILE;
		if(fgetc(midi_file) != 'r') return SB_ERR_READING_MIDI_FILE;
		if(fgetc(midi_file) != 'k') return SB_ERR_READING_MIDI_FILE; //Get signature for track chunk

		*(*(loaded_midi_tracks + t)).track_chunk.type = "MTrk";		//Writes the signature to the loaded header

		length =                 ((unsigned long int)fgetc(midi_file));
		length = (length << 8) | ((unsigned long int)fgetc(midi_file));
		length = (length << 8) | ((unsigned long int)fgetc(midi_file));
		length = (length << 8) | ((unsigned long int)fgetc(midi_file)); //Gets the length of the data stream

		*(*(loaded_midi_tracks + t)).track_chunk.length = length;	//Writes the length loaded to the loaded track chunk

		track_stream = malloc(length);	//Allocates memory  for the track data stream

		*(*(loaded_midi_tracks + t)).track_stream = track_stream;

		for(i = 0; i < length; i ++)
		{
			*(track_stream + i) = fgetc(midi_file);	//Copies the data stream from the hd to RAM
		}
	}

	fclose(midi_file);
}

void sb_play_midi_track(SB_MIDI_TRACK* midi_track)
{
	if(is_playing_midi == 0)
	{
		is_playing_midi = 1;

		midi_track->next_event = midi_track->track_stream;
	}
}

void sb_midi_next_event(SB_MIDI_TRACK* midi_track)
{
	unsigned long int delta_time;
	char current_byte;
	char current_channel;

	do
	{
		delta_time = sb_read_var_length(&(midi_track->next_event));	//Decodes the delta time for the next event
		current_byte = readc_stream(&(midi_track->next_event));

		if((current_byte & 0xF0) == SB_MIDI_MSG_NOTE_OFF)	//If current message is a note off event, with masking the channel numbers
		{
			current_channel = current_byte & 0x0F;	//Masking the message part gives me only the channel number
			sb_key_off(current_channel);		//Key off for this channel

			midi_track->next_event += 2;	//Skips 2 bytes. One of them is key, which makes no sense here since every channel is monophonic and velocity which we dont use.
		}
		else if((current_byte & 0xF0) == SB_MIDI_MSG_NOTE_ON)	//If current message is note on event.
		{
			SB_MIDI_NOTE note;
			
			current_channel = current_byte & 0x0F; //Getting channel
			current_byte = readc_stream(&(midi_track->next_event));	//The next byte on the stream should contain the codified MIDI note to play.

			note = sb_get_midi_note(current_byte);	//Decodes the midi note into our NOTE struct and octave.

			sb_key_on(current_channel, note.note, note.octave);	//Key on for this note.

			midi_track->next_event += 1;	//Skips next byte since it is velocity information and we dont use that
		}
		else if((current_byte & 0xF0) == SB_MIDI_MSG_PROGRAM_CHANGE)	//Change instrument instruction on a certain channel
		{

			sb_load_instrument(current_byte & 0x0F, readc_stream(&(midi_track->next_event)));
		}
		else if((current_byte == SB_MIDI_MSG_CONTROL_CHANGE))	//Skips 2 bytes
		{
			midi_track->next_event += 2;
		}
		else if((current_byte == 0xFF)) //META-EVENTS
		{
			current_byte = readc_stream(&(midi_track->next_event));

			if(current_byte == 0x03)	//Title Meta-event, Ignore for now.
			{
				unsigned long int length = sb_read_var_length(&(midi_track->next_event));
				
				midi_track->next_event += length;  //*SKIP*
			}
			else if(current_byte == 0x51)	//Tempo Change.
			{
				if(current_byte = readc_stream(&(midi_track->next_event)) == 0x03)	//Confirm tempo change.
				{
					unsigned long int tempo;

					tempo = (unsigned long int)readc_stream(&(midi_track->next_event));
					tempo = (tempo << 8) | ((unsigned long int)readc_stream(&(midi_track->next_event)));
					tempo = (tempo << 8) | ((unsigned long int)readc_stream(&(midi_track->next_event)));
					
					current_tempo = tempo;
				}
			}
			else if(current_byte == 0x2F) //Track end.
			{
				if(current_byte = readc_stream(&(midi_track->next_event)) == 0x0)	//Confirm Track End
				{
					midi_track->next_event = midi_track->track_stream;
					midi_playing = 0;
				}
			}
			else	//Every Meta-Event Should be of the struct FF(type)(varlen length) so we can assume that even if we dont know the event we can skip it by jumping length bytes.
			{
				unsigned long int length = sb_read_var_length(&(midi_track->next_event));
				
				midi_track->next_event += length; 
			}
		}

	}while(delta_time == 0);	
}

inline SB_MIDI_NOTE sb_get_midi_note(char midi_note_value)
{
	SB_MIDI_NOTE current_note;

	current_note.note = midi_note_value % 12;

	current_note.octave = (midi_note_value / 12) - 1;

	if(current_note.octave < 0) current_note.octave = 0;

	return current_note;
}

unsigned long int sb_read_var_length(char** byte_stream)
{
	unsigned long int value;
	char c;

	if((value = readc_stream(byte_stream)) & 0x80)
	{
		value &= 0x7F;
		do
		{
			value = (value << 7) + ((c = readc_stream(byte_stream)) & 0x7F);
		
		}while(c & 0x80);
	}
	return value;
}
