#ifndef SB_MUS_H
#define SB_MUS_H

#define SB_MUS_MSG_NOTE_OFF 			0x80
#define SB_MUS_MSG_NOTE_ON				0x90
#define SB_MUS_MSG_AFTERTOUCH			0xA0
#define SB_MUS_MSG_CONTROL_CHANGE		0xB0
#define SB_MUS_MSG_PROGRAM_CHANGE		0xC0
#define SB_MUS_MSG_CHANNEL_PRESSURE		0xD0	//All of these need to be OR-ed to a channelNo nibble
#define SB_MUS_MSG_PITCH_WHEEL			0xE0	//Ex. 0x80 | 0x01 = NOTE OFF CHANNEL 1

#define SB_TIMER_INTERRUPT				0x08

#define SB_ERR_READING_MUS_FILE			0x01

typedef struct SB_MIDI_NOTE_
{
	int octave;
	NOTE note;

} SB_MIDI_NOTE;

void sb_mus_next_event();
unsigned long int sb_read_var_length(char far ** byte_stream);

SB_MIDI_NOTE sb_get_midi_note(char midi_note_value);

#endif