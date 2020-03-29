#include <stdio.h>
#include <stdlib.h>

#include "sb.h"
#include "keyb.h"
#include "graph.h"

#define BLACK 0x00
#define BLUE 0x01
#define GREEN 0x02
#define CYAN 0x03
#define RED 0x04
#define MAGENTA 0x05
#define BROWN 0x06
#define LIGHTGRAY 0x07
#define GRAY 0x08
#define LIGHTBLUE 0x09
#define LIGHTGREEN 0x0A
#define LIGHTCYAN 0x0B
#define LIGHTRED 0x0C
#define LIGHTMAGENTA 0x0D
#define YELLOW 0x0E
#define WHITE 0x0F

char is_software_running = 0;

NOTE current_note = 0;
int current_octave = 5;
char active_note = 0;

int current_instrument = 0;

void On_Key_Down(int scan_code)
{
	char play_note = 0;

	switch(scan_code)
	{
		case MAKE_A:
			play_note = 1;
			current_note = C;
			break;
		case MAKE_W:
			play_note = 1;
			current_note = Cs;
			break;
		case MAKE_S:
			play_note = 1;
			current_note = D;
			break;
		case MAKE_E:
			play_note = 1;
			current_note = Ds;
			break;
		case MAKE_D:
			play_note = 1;
			current_note = E;
			break;
		case MAKE_F:
			play_note = 1;
			current_note = F;
			break;
		case MAKE_T:
			play_note = 1;
			current_note = Fs;
			break;
		case MAKE_G:
			play_note = 1;
			current_note = G;
			break;
		case MAKE_Y:
			play_note = 1;
			current_note = Gs;
			break;
		case MAKE_H:
			play_note = 1;
			current_note = A;
			break;
		case MAKE_U:
			play_note = 1;
			current_note = As;
			break;
		case MAKE_J:
			play_note = 1;
			current_note = B;
			break;
		case MAKE_Z:
			if(current_octave > 0) current_octave -= 1;
			break;
		case MAKE_X:
			if(current_octave < 8) current_octave += 1;
			break;
		case MAKE_ESC:
			is_software_running = 0;
			break;
		case MAKE_M:
			if(current_instrument < 143)
			{
				current_instrument += 1;
				change_instrument(current_instrument);
			}
			else
			{
				current_instrument = 0;
				change_instrument(current_instrument);
			}
			break;
		case MAKE_N:
			if(current_instrument > 0)
			{
				current_instrument -= 1;
				change_instrument(current_instrument);
			}
			else
			{
				current_instrument = 143;
				change_instrument(current_instrument);
			}
			break;
	}

	if(play_note) 
	{
		active_note = 1;
		sb_key_on(0, current_note, current_octave);
	}
}

void On_Key_Up(int scan_code)
{
	char stop_note = 0;

	switch(scan_code)
	{
		case MAKE_A:
			stop_note = 1;
			current_note = C;
			break;
		case MAKE_W:
			stop_note = 1;
			current_note = Cs;
			break;
		case MAKE_S:
			stop_note = 1;
			current_note = D;
			break;
		case MAKE_E:
			stop_note = 1;
			current_note = Ds;
			break;
		case MAKE_D:
			stop_note = 1;
			current_note = E;
			break;
		case MAKE_F:
			stop_note = 1;
			current_note = F;
			break;
		case MAKE_T:
			stop_note = 1;
			current_note = Fs;
			break;
		case MAKE_G:
			stop_note = 1;
			current_note = G;
			break;
		case MAKE_Y:
			stop_note = 1;
			current_note = Gs;
			break;
		case MAKE_H:
			stop_note = 1;
			current_note = A;
			break;
		case MAKE_U:
			stop_note = 1;
			current_note = As;
			break;
		case MAKE_J:
			stop_note = 1;
			current_note = B;
			break;
	}

	if(stop_note) 
	{
		active_note = 0;
		sb_key_off(0);
	}
}

void change_instrument(int instrument_number)
{
	sb_load_instrument(instrument_number, 0);
}

int main(int argc, char * argv[])
{
	char* current_notechar = "";
	char buffer[3];
	char namebuffer[12];

	int i = 0;

	is_software_running = 1;

	Keyboard_Install_Driver();
	sb_init_fm();

	sb_load_instrument_bank("instbank.bnk", &SB_INSTRUMENT_BANK);
	sb_load_instrument(0, 0);

	_clearscreen(0);
	_displaycursor(0);

	while(is_software_running == 1)
	{	
		if(active_note)
		{
			switch(current_note)
			{
				case C:
					current_notechar = "C ";
					break;
				case Cs:
					current_notechar = "C#";
					break;
				case D:
					current_notechar = "D ";
					break;
				case Ds:
					current_notechar = "D#";
					break;
				case E:
					current_notechar = "E ";
					break;
				case F:
					current_notechar = "F ";
					break;
				case Fs:
					current_notechar = "F#";
					break;
				case G:
					current_notechar = "G ";
					break;
				case Gs:
					current_notechar = "G#";
					break;
				case A:
					current_notechar = "A ";
					break;
				case As:
					current_notechar = "A#";
					break;
				case B:
					current_notechar = "B ";
					break;
			}	
		}
		else
		{

			current_notechar = "  ";
		}


		_settextposition(2,3);
		_setbkcolor(RED);
		_settextcolor(LIGHTMAGENTA);  
		_outtext("                                                                            ");
		_settextposition(3,3);
		_outtext("                  WELCOME TO FOFFONSO'S KEYBOARD OF ");
		_settextcolor(_LIGHTRED); //For some weird reason this causes things to blink!
		_outtext("FUN!                    ");
		_settextposition(4,3);
		_outtext("                                                                            ");
		_settextcolor(WHITE); 
		_settextposition(5,3);
		_outtext(" This is an AdLib/Soundblaster keyboard instrument bank test thing.         ");
		_settextposition(6,3);
		_outtext(" Made by Affonso Amendola, the best (and prettiest) programmer to ever live ");
		_settextposition(7,3);
		_outtext("                                                                            ");
		_settextposition(8,3);
		_outtext("                                                                            ");
		_settextposition(9,3);
		_outtext(" Current Instrument : ");
		sprintf(buffer, "%-3d", current_instrument);
		_outtext(buffer);
		_outtext(" ");
		sprintf(namebuffer, "%-12s", SB_INSTRUMENT_BANK[current_instrument].name);
		_outtext(namebuffer);
		_outtext("                                      ");
		_settextposition(10,3);
		_outtext(" Current note : ");
		_outtext(current_notechar);
		_outtext("                                                          ");
		_settextposition(11,3);
		_outtext(" Current octave : ");
		sprintf(buffer, "%-3d", current_octave);
		_outtext(buffer);
		_outtext("                                                       ");
		_settextposition(12,3);
		_outtext("                                                                            ");
		_settextposition(13,3);
		_outtext("               W-> C#   E-> D#        T-> F#   Y-> G#   U-> A#              ");
		_settextposition(14,3);
		_outtext("                                                                            ");
		_settextposition(15,3);
		_outtext(" Controls : A-> C   S-> D   D-> E   F-> F   G-> G    H->A    J->B           ");
		_settextposition(16,3);
		_outtext("                             Key -> Note                                    ");
		_settextposition(17,3);
		_outtext("                                                                            ");
		_settextposition(18,3);
		_outtext("  Z: Decreases Octave      X: Increases Octave                              ");
		_settextposition(19,3);
		_outtext("  N: Previous Instrument   M: Next Instrument                               ");
		_settextposition(20,3);
		_outtext("                                                                            ");
		_settextposition(21,3);
		_outtext("  ESC: Quits :(                                                             ");
		_settextposition(22,3);
		_outtext("                                                                            ");
		_settextposition(23,3);
		_outtext("                      Copyright Affonso Amendola 2020                       ");
		_settextposition(24,3);
		_outtext("                     Made during the CoronaVirus Crisis                     ");





	}
	
	return 0;
}
