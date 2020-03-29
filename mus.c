#include <stdio.h>
#include <stdlib.h>

#include <dos.h>

#include <sb.h>
#include <util.h>

#include <mus.h>

typedef struct SB_MUS_HEADER_
{
	unsigned char 		major_version;
	unsigned char 		minor_version;
	unsigned long int 	tune_id;
	char				tune_name[30];	
	unsigned char		tick_beat;
	unsigned char		beat_measure;
	unsigned long int 	total_tick;
	unsigned long int 	data_size;
	unsigned long int 	nr_command;
	char 				padding[8];
	char				sound_mode;
	char				pitch_bend_range;
	unsigned int 		basic_tempo;
	char				padding_2[8];
	char far *			sequence_data;

} SB_MUS_HEADER;

typedef unsigned long int sb_variable_length;

char is_playing_mus;
char is_mus_loaded;
char far * next_mus_event;

unsigned long int next_event_microseconds;

SB_MUS_HEADER loaded_mus_header;

void (interrupt far * old_timer_isr)();

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
			if(is_playing_mus)
			{
				sb_mus_next_event();
			}

			sb_fm_select_register(0x04);
			sb_fm_write_data_register(0x80);
		}
	}

	old_timer_isr(); //Chain the old timer ISR
}

void sb_install_timer_handler()			//Install the timer handler
{
	old_timer_isr = _dos_getvect(SB_TIMER_INTERRUPT);

	_dos_setvect(SB_TIMER_INTERRUPT, sb_timer_handler);
}

int sb_load_mus_file(const char * file_path) //File format described at http://vgmpf.com/Wiki/index.php/MUS_(AdLib)
{
	int t = 0;
	int i = 0;
	FILE * mus_file;

	mus_file = fopen(file_path, "r");

	loaded_mus_header.major_version = fgetc(mus_file);
	loaded_mus_header.minor_version = fgetc(mus_file);

	loaded_mus_header.tune_id = fgeti32le(mus_file);

	for(i = 0; i < 30; i++)
	{
		loaded_mus_header.tune_name[i] = fgetc(mus_file);
	}

	loaded_mus_header.tick_beat = fgetc(mus_file);
	loaded_mus_header.beat_measure = fgetc(mus_file);
	loaded_mus_header.total_tick = fgeti32le(mus_file);
	loaded_mus_header.data_size = fgeti32le(mus_file);
	loaded_mus_header.nr_command = fgeti32le(mus_file);

	for(i = 0; i < 8; i++)
	{
		loaded_mus_header.padding[i] = fgetc(mus_file);
	}

	loaded_mus_header.sound_mode = fgetc(mus_file);
	loaded_mus_header.pitch_bend_range = fgetc(mus_file);

	loaded_mus_header.basic_tempo = fgeti16le(mus_file);

	for(i = 0; i < 8; i++)
	{
		loaded_mus_header.padding_2[i] = fgetc(mus_file);
	}

	loaded_mus_header.sequence_data = (char far *)malloc(loaded_mus_header.data_size * sizeof(char));

	for(i = 0; i < loaded_mus_header.data_size; i++)
	{
		loaded_mus_header.sequence_data[i] = fgetc(mus_file);
	}

	fclose(mus_file);
	is_mus_loaded = 1;

	return 0;
}

void sb_play_mus_track()
{
	if(is_playing_mus == 0 && is_mus_loaded)
	{
		is_playing_mus = 1;
		next_mus_event = loaded_mus_header.sequence_data;
		sb_mus_next_event();
	}
}

void sb_mus_next_event()
{
	unsigned long int delta_time;
	char current_byte;
	char current_channel;
	int i = 0;

	do
	{
		delta_time = sb_read_var_length(&next_mus_event);	//Decodes the delta time for the next event

		current_byte = readc_stream(&next_mus_event);

		if((current_byte & 0xF0) == SB_MUS_MSG_NOTE_OFF)	//If current message is a note off event, with masking the channel numbers
		{
			current_channel = current_byte & 0x0F;	//Masking the message part gives me only the channel number
			sb_key_off(current_channel);		//Key off for this channel

			next_mus_event += 2;	//Skips 2 bytes. One of them is key, which makes no sense here since every channel is monophonic and velocity which we dont use.
		}
		else if((current_byte & 0xF0) == SB_MUS_MSG_NOTE_ON)	//If current message is note on event.
		{
			SB_MIDI_NOTE note;
			
			current_channel = current_byte & 0x0F; //Getting channel
			current_byte = readc_stream(&next_mus_event);	//The next byte on the stream should contain the codified MIDI note to play.

			note = sb_get_midi_note(current_byte);	//Decodes the midi note into our NOTE struct and octave.

		//	sb_change_volume(current_channel, readc_stream(&next_mus_event)); //On mus format the velocity byte indicates de volume.
			sb_key_on(current_channel, note.note, note.octave);	//Key on for this note.
		}
		else if((current_byte & 0xF0) == SB_MUS_MSG_PROGRAM_CHANGE)	//Change instrument instruction on a certain channel
		{

			sb_load_instrument(current_byte & 0x0F, readc_stream(&next_mus_event));
		}
		else if((current_byte & 0xF0) == SB_MUS_MSG_CONTROL_CHANGE)	//Unused skip 2 bytes
		{
			next_mus_event += 2;
		}
		else if((current_byte & 0xF0) == SB_MUS_MSG_CHANNEL_PRESSURE)	//Unused skip 2 bytes
		{
			next_mus_event += 1;
		}
		else if((current_byte & 0xF0) == SB_MUS_MSG_PITCH_WHEEL)	//TODO: Implement Pitch bending.
		{
			next_mus_event += 2;
		}
		else if(current_byte == 0xF0)
		{
			if(readc_stream(&next_mus_event) == 0x7F && readc_stream(&next_mus_event) == 0x00) //TODO: Implement SysEx SpeedControl Tempo Multiplier.
			{
				next_mus_event += 3; //skips 3 bytes, for now
			}
		}

		next_event_microseconds = delta_time * ((60000000 / (unsigned long int)loaded_mus_header.tick_beat) / (unsigned long int)loaded_mus_header.basic_tempo);

	}while(delta_time == 0);	
}

SB_MIDI_NOTE sb_get_midi_note(char midi_note_value)
{
	SB_MIDI_NOTE current_note;

	current_note.note = midi_note_value % 12;

	current_note.octave = (midi_note_value / 12) - 1;

	if(current_note.octave < 0) current_note.octave = 0;

	return current_note;
}

unsigned long int sb_read_var_length(char far ** byte_stream)
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

int main(int argc, char * argv[])
{
	printf("LOADING FILE\n");
	sb_load_mus_file("DARKSPY1.MUS");
	printf("PLAYING TRACK\n");
	sb_play_mus_track();
	return 1;
}
