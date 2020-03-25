#include <stdio.h>
#include <stdlib.h>

#include "sb.h"

SB_CHANNEL_STRUCT SB_CHANNELS[8];

SB_INSTRUMENT * SB_INSTRUMENT_BANK;

//This is just for debugging
void hexDump (const char * desc, const void * addr, const int len) {
    int i;
    unsigned char buff[17];
    const unsigned char * pc = (const unsigned char *)addr;

    // Output description if given.

    if (desc != NULL)
        printf ("%s:\n", desc);

    // Length checks.

    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    else if (len < 0) {
        printf("  NEGATIVE LENGTH: %d\n", len);
        return;
    }

    // Process every byte in the data.

    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Don't print ASCII buffer for the "zeroth" line.

            if (i != 0)
                printf ("  %s\n", buff);

            // Output the offset.

            printf ("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printf (" %02x", pc[i]);

        // And buffer a printable ASCII character for later.

        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) // isprint() may be better.
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.

    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }

    // And print the final ASCII buffer.

    printf ("  %s\n", buff);
}

//Resets the DSP for new use
void sb_reset_dsp()			
{
	_asm {
			mov 	dx, SB_DSP_RESET
			mov 	al, 1

			out 	dx, al

			sub		al, al
			Delay:
			dec 	al
			jnz 	Delay
			out 	dx, al

			sub 	cx, cx
			Empty:
			mov 	dx, SB_DSP_READ_BUFFER_STATUS

			in 		al, dx
			or 		al, al

			jns 	NextAttempt

			sub 	dl, 4
			in		al, dx
			cmp 	al, 0xAA
			je		ResetOK

			NextAttempt:
			loop Empty

			ResetOK:
		 }
}

//Reads a byte from the DSP
char sb_read_dsp()
{
	char dsp_data = 0;

	_asm{
		mov 	dx, SB_DSP_READ_BUFFER_STATUS
	Busy:
		in 		al, dx
		or 		al, al
		jns 	Busy

		sub 	dl, 4     ;//READ DATA PORT, 2xAh
		in  	al, dx
		mov 	dsp_data, al
		}

	return dsp_data;
}

//Writes a byte to the DSP
void sb_write_dsp(char data)
{
	_asm{
		mov 	dx, SB_DSP_WRITE_BUFFER_STATUS
	Busy:
		in 		al, dx
		or 		al, al
		js 		Busy

		mov		al, data
		out		dx, al
		}
}

//Select the relevant register in the OPL chip
void sb_fm_select_register(char register_index)
{
	_asm{
		mov dx, SB_FM_MUSIC_ADDRESS
		mov al, register_index
		out dx, al

		in al, dx
		in al, dx
		in al, dx
		in al, dx
		in al, dx 
		in al, dx; //WAIT 2.3 microseconds
		}
}

//Writes a byte to the data register of the FM Synthesizer (OPL chip)
void sb_fm_write_data_register(char register_data)
{
	_asm{
		mov dx, SB_FM_MUSIC_DATA
		mov al, register_data
		out dx, al

		in al, dx 
		in al, dx 
		in al, dx 
		in al, dx 
		in al, dx  
		in al, dx 
		in al, dx 
		in al, dx 
		in al, dx 
		in al, dx  
		in al, dx 
		in al, dx 
		in al, dx 
		in al, dx  
		in al, dx 
		in al, dx 
		in al, dx 
		in al, dx 
		in al, dx  
		in al, dx 
		in al, dx 
		in al, dx 
		in al, dx 
		in al, dx  
		in al, dx 
		in al, dx 
		in al, dx 
		in al, dx  
		in al, dx 
		in al, dx 
		in al, dx 
		in al, dx 
		in al, dx  
		in al, dx 
		in al, dx  
		in al, dx ; //WAIT 36 microseconds
		}
}

//Frequency Table, [Octave][Note (Starting at C=0 through B=11)]
const unsigned int freq_table[8][12] = { 	16,   17,   18,   19,   21,   22,   23,   24,   26,   27,   29,   31,
									 	 	33,   35,   37,   39,   41,   44,   46,   49,   52,   55,   58,   62,
									  		65,   69,   73,   78,   82,   87,   92,   98,  104,  110,  116,  123,
									 	   131,  139,  147,  156,  165,  175,  185,  196,  208,  220,  233,  247,
									 	   262,  277,  294,  311,  330,  349,  370,  392,  415,  440,  466,  494,
									 	   523,  554,  587,  622,  659,  698,  740,  784,  831,  880,  932,  988,
								     	  1046, 1109, 1175, 1244, 1318, 1397, 1480, 1568, 1661, 1760, 1865, 1975,
										  2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951};

//Theres a complicated calculation to define f_numbers, read the manuals, these numbers cover most of what Ill need
//F_Num Table, [Octave][Note (Starting at C=0 through B=11)]					
const int f_num_table[8][12] = { 	345, 365, 387, 410, 435, 460, 488, 517, 547, 580, 615, 651,
									690, 731, 774, 820, 869, 921, 975, 517, 547, 580, 615, 651,
									690, 731, 774, 820, 869, 921, 975, 517, 547, 580, 615, 651,
									690, 731, 774, 820, 869, 921, 975, 517, 547, 580, 615, 651,
									690, 731, 774, 820, 869, 921, 975, 517, 547, 580, 615, 651,
									690, 731, 774, 820, 869, 921, 975, 517, 547, 580, 615, 651,
									690, 731, 774, 820, 869, 921, 975, 517, 547, 580, 615, 651,
  									690, 731, 774, 820, 869, 921, 975, 517, 547, 580, 615, 651};

//Determines which block to use for each frequency, since the equality block = octave is not really true.
//[Octave][Note (Starting at C=0 through B=11)]	
const char f_block_table[8][12] = {  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
									0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   1,   1,
									1,   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,
									2,   2,   2,   2,   2,   2,   2,   3,   3,   3,   3,   3,
									3,   3,   3,   3,   3,   3,   3,   4,   4,   4,   4,   4,
									4,   4,   4,   4,   4,   4,   4,   5,   5,   5,   5,   5,
									5,   5,   5,   5,   5,   5,   5,   6,   6,   6,   6,   6,
									6,   6,   6,   6,   6,   6,   6,   7,   7,   7,   7,   7};

inline int sb_get_f_number_note(NOTE note, int octave)
{

 	return f_num_table[octave][note];
}

inline int sb_get_f_block_note(NOTE note, int octave)
{

 	return f_block_table[octave][note];
}

void sb_init_fm()
{
	int op = 0;
	int ch = 0;

	sb_fm_select_register(0x01); 		//Select register 0x01, which is WaveSelect enable and Test Register
	sb_fm_write_data_register(0x00);	//Write 0 to wave select and 0 to Test Register
	
	sb_fm_select_register(0x04);		//Select register 0x04, which is IRQ-Reset and a bunch of other non important things right now
	sb_fm_write_data_register(0x80);	//Reset IRQ and timer flags in Status Register

	sb_fm_select_register(0x08);		//Select register 0x08, which is teh Composite Sine Wave mode on/off and NOTE-SEL
	sb_fm_write_data_register(0x00);	//Resets both of them to 0

	for(op = 0; op <= 0x15; op++)
	{
		if(op == 0x0E || op == 0x0F) continue;

		sb_fm_select_register(0x20 + op); //Select Tremolo, Vibrato and etc register for the op operator
		sb_fm_write_data_register(0x01);  //Resets everything to zero, disables Tremolo, freqVibrato, Sustain, envelop Scaling,
										  //Except for frequency Multiplication, which is set at 1

	}

	for(op = 0; op <= 0x15; op++)
	{
		if(op == 0x0E || op == 0x0F) continue;

		sb_fm_select_register(0x40 + op); //Select KeyScale and Output Level register for the op operator
		sb_fm_write_data_register(0x00); //Resets KeyScale Level to 0 and sets Output level to Max for all operators  

	}

	for(op = 0; op <= 0x15; op++)
	{
		if(op == 0x0E || op == 0x0F) continue;

		sb_fm_select_register(0x60 + op); //Select Attack and Decay Rate register for the op operator
		sb_fm_write_data_register(0x10); //Resets attack and decay to their fastest values (both at max achieves that) 

	}

	for(op = 0; op <= 0x15; op++)
	{
		if(op == 0x0E || op == 0x0F) continue;

		sb_fm_select_register(0x80 + op); //Select Sustain Level and Decay Rate register for the op operator
		sb_fm_write_data_register(0x77); //Sets Sustain Level to 0 (needs enabling for it to work), amd release rate to max
	}

	for(ch = 0; ch < 8; ch++)
	{
		int default_f_num = f_num_table[4][C]; //Default f_num is middle C ( octave 5, note C)
		int default_block = f_block_table[4][C]; //Default f_block as middle C

		sb_fm_select_register(0xA0 + ch); //Select Frequency number (low) for each ch channel
		sb_fm_write_data_register((char)(0xFF & default_f_num));  //Put Default f_num for each channel
		sb_fm_select_register(0xB0 + ch); //Select Key On, Block Number, Frequency number (high) for each ch channel
		sb_fm_write_data_register((char) ((0x3 & (default_f_num>>8)) | (default_block << 2)) );  //Put Default f_num for each channel
	}

	sb_fm_select_register(0xBD); //Select the Trem Dep/ Vibr Dep/ PercMode /Perc KEYONs
	sb_fm_write_data_register(0x00);  //Reset all to 0

	for(ch = 0; ch < 8; ch++)
	{
		sb_fm_select_register(0xC0 + ch); //Select FeedBack Modulation and Additive/FM selector register
		sb_fm_write_data_register(0x00);  //Reset all Feedback Modulation to 0 and sets every channel to FM synth (Op1 into Op2)
		
	}

	for(op = 0; op <= 0x15; op++)
	{
		if(op == 0x0E || op == 0x0F) continue;

		sb_fm_select_register(0xE0 + op); //Select Waveform select register
		sb_fm_write_data_register(0x00); // Set waveform to sine	
	}
}

void sb_key_on(int channel, NOTE note, int octave)
{
	if(SB_CHANNELS[channel].key_on == 0)
	{
		int f_num = f_num_table[octave][note];
		char f_block = f_block_table[octave][note];

		SB_CHANNELS[channel].key_on = 1;
		SB_CHANNELS[channel].note = note;
		SB_CHANNELS[channel].octave = octave;
		SB_CHANNELS[channel].f_num = f_num;
		SB_CHANNELS[channel].f_block = f_block;

		sb_fm_select_register(0xA0 + channel); //Select Frequency number (low) 
		sb_fm_write_data_register((char)(0xFF & f_num));  //Put f_num on register
		sb_fm_select_register(0xB0 + channel); //Select Key On, Block Number, Frequency number (high) for each ch channel
		sb_fm_write_data_register((char) ((0x3 & (f_num>>8)) | (f_block << 2) | 0x20) );  //Put Default f_num for each channel
	}
}

void sb_key_off(int channel)
{
	if(SB_CHANNELS[channel].key_on == 1)
	{
		int f_num = SB_CHANNELS[channel].f_num;
		char f_block = SB_CHANNELS[channel].f_block;

		SB_CHANNELS[channel].key_on = 0;

		sb_fm_select_register(0xB0 + channel); //Select Key On, Block Number, Frequency number (high) for each ch channel
		sb_fm_write_data_register((char) ((0x3 & (f_num>>8)) | (f_block << 2)) );  //Put Default f_num for each channel
	}
}

void sb_load_instrument(SB_INSTRUMENT instrument, char channel)
{
	int carrier_op_offset;

	int modulator_op_offset = channel;
	if(modulator_op_offset >= 3) modulator_op_offset += 3;			//This is all code to define the offset,
	if(modulator_op_offset >= 6) modulator_op_offset += 3;			//There are some stupid rules, check the manual

	carrier_op_offset = modulator_op_offset + 3;

	if(modulator_op_offset >= 0x0D) modulator_op_offset += 3;		//the range of indexes are 0x00 to 0x0D then skips to 0x10 to 0x15
	if(carrier_op_offset >= 0x0D) carrier_op_offset += 3;

	sb_fm_select_register(0x20 + modulator_op_offset); 		
	sb_fm_write_data_register(	instrument.modulator.amplitude_vibrato << 7 | 
								instrument.modulator.frequency_vibrato << 6 | 
								instrument.modulator.sustain_enable << 5 | 
								instrument.modulator.envelope_scaling << 4 |
								instrument.modulator.frequency_multiplier);

	sb_fm_select_register(0x20 + carrier_op_offset); 		
	sb_fm_write_data_register(	instrument.carrier.amplitude_vibrato << 7 | 
								instrument.carrier.frequency_vibrato << 6 | 
								instrument.carrier.sustain_enable << 5 | 
								instrument.carrier.envelope_scaling << 4 |
								instrument.carrier.frequency_multiplier);



	sb_fm_select_register(0x40 + modulator_op_offset); 		
	sb_fm_write_data_register(	instrument.modulator.key_scale_level << 6 |
								instrument.modulator.output_level);

	sb_fm_select_register(0x40 + carrier_op_offset); 		
	sb_fm_write_data_register(	instrument.carrier.key_scale_level << 6 |
								instrument.carrier.output_level);



	sb_fm_select_register(0x60 + modulator_op_offset); 		
	sb_fm_write_data_register(	instrument.modulator.attack_rate << 4 |
								instrument.modulator.decay_rate);

	sb_fm_select_register(0x60 + carrier_op_offset); 		
	sb_fm_write_data_register(	instrument.carrier.attack_rate << 4 |
								instrument.carrier.decay_rate);



	sb_fm_select_register(0x60 + modulator_op_offset); 		
	sb_fm_write_data_register(	instrument.modulator.sustain_level<< 4 |
								instrument.modulator.release_rate);

	sb_fm_select_register(0x60 + carrier_op_offset); 		
	sb_fm_write_data_register(	instrument.carrier.sustain_level << 4 |
								instrument.carrier.release_rate);



	sb_fm_select_register(0xE0 + modulator_op_offset); 		
	sb_fm_write_data_register(	instrument.modulator.waveform);

	sb_fm_select_register(0xE0 + carrier_op_offset); 		
	sb_fm_write_data_register(	instrument.carrier.waveform);



	sb_fm_select_register(0xC0 + channel); 	
	sb_fm_write_data_register(	instrument.feedback_modulation_factor << 1 | 
								instrument.type_of_synth);

	SB_CHANNELS[channel].loaded_instrument = instrument;
	SB_CHANNELS[channel].key_on = 0;
}

void sb_load_instrument_file(const char* path, SB_INSTRUMENT * instrument)
{
	int i = 0;
	FILE * instrument_file = fopen(path, "r");

	for(i = 0; i < 8; i++)
	{
		instrument->name[i] = fgetc(instrument_file);
	}

	instrument->modulator.key_scale_level = 		(fgetc(instrument_file) & 0x03);
	instrument->modulator.frequency_multiplier = 	(fgetc(instrument_file) & 0x0F);
	instrument->modulator.attack_rate = 			(fgetc(instrument_file) & 0x0F);
	instrument->modulator.sustain_level = 			(fgetc(instrument_file) & 0x0F);
	instrument->modulator.sustain_enable = 			(fgetc(instrument_file) & 0x01);
	instrument->modulator.decay_rate = 				(fgetc(instrument_file) & 0x0F);
	instrument->modulator.release_rate = 			(fgetc(instrument_file) & 0x0F);
	instrument->modulator.output_level = 			(fgetc(instrument_file) & 0x3F);
	instrument->modulator.amplitude_vibrato = 		(fgetc(instrument_file) & 0x01);
	instrument->modulator.frequency_vibrato = 		(fgetc(instrument_file) & 0x01);
	instrument->modulator.envelope_scaling = 		(fgetc(instrument_file) & 0x01);
	instrument->modulator.waveform = 				(fgetc(instrument_file) & 0x03);

	instrument->carrier.key_scale_level = 			(fgetc(instrument_file) & 0x03);
	instrument->carrier.frequency_multiplier = 		(fgetc(instrument_file) & 0x0F);
	instrument->carrier.attack_rate = 				(fgetc(instrument_file) & 0x0F);
	instrument->carrier.sustain_level = 			(fgetc(instrument_file) & 0x0F);
	instrument->carrier.sustain_enable = 			(fgetc(instrument_file) & 0x01);
	instrument->carrier.decay_rate = 				(fgetc(instrument_file) & 0x0F);
	instrument->carrier.release_rate = 				(fgetc(instrument_file) & 0x0F);
	instrument->carrier.output_level = 				(fgetc(instrument_file) & 0x3F);
	instrument->carrier.amplitude_vibrato = 		(fgetc(instrument_file) & 0x01);
	instrument->carrier.frequency_vibrato = 		(fgetc(instrument_file) & 0x01);
	instrument->carrier.envelope_scaling = 			(fgetc(instrument_file) & 0x01);
	instrument->carrier.waveform = 					(fgetc(instrument_file) & 0x03);

	instrument->feedback_modulation_factor = 		(fgetc(instrument_file) & 0x07);
	instrument->type_of_synth = 					(fgetc(instrument_file) & 0x01);

	fclose(instrument_file);
}

int sb_load_instrument_bank(const char* path, SB_INSTRUMENT** instrument_bank)
{
	int i = 0;

	unsigned int instrument_number_used = 0;
	unsigned int instrument_number_all = 0;

	long unsigned int name_list_offset = 0;
	long unsigned int data_start_offset = 0;

	unsigned int used_instrument_index = 0;

	SB_INSTRUMENT * current_instrument_data;

	FILE * instrument_file = fopen(path, "r");

	fgetc(instrument_file); //Skip file version Major
	fgetc(instrument_file); //Skip file version minor

	if(fgetc(instrument_file) != 'A') return -1;
	if(fgetc(instrument_file) != 'D') return -1;
	if(fgetc(instrument_file) != 'L') return -1;
	if(fgetc(instrument_file) != 'I') return -1;
	if(fgetc(instrument_file) != 'B') return -1;
	if(fgetc(instrument_file) != '-') return -1;	//Check file signature

	instrument_number_used |=  (unsigned int)fgetc(instrument_file);
	instrument_number_used |= ((unsigned int)fgetc(instrument_file) << 8);  //Get number of instruments used (UINT16LE)

	instrument_number_all |=  (unsigned int)fgetc(instrument_file);
	instrument_number_all |= ((unsigned int)fgetc(instrument_file) << 8);  //Get number of instruments used (UINT16LE)

	name_list_offset |=  (long unsigned int)fgetc(instrument_file);
	name_list_offset |= ((long unsigned int)fgetc(instrument_file) << 8);  
	name_list_offset |= ((long unsigned int)fgetc(instrument_file) << 16);  
	name_list_offset |= ((long unsigned int)fgetc(instrument_file) << 24);  //Get absolute index of name_list (UINT32LE)

	data_start_offset |=  (long unsigned int)fgetc(instrument_file);
	data_start_offset |= ((long unsigned int)fgetc(instrument_file) << 8);  
	data_start_offset |= ((long unsigned int)fgetc(instrument_file) << 16);  
	data_start_offset |= ((long unsigned int)fgetc(instrument_file) << 24);  //Get absolute index of name_list (UINT32LE)

	fgetc(instrument_file);
	fgetc(instrument_file);
	fgetc(instrument_file);
	fgetc(instrument_file);
	fgetc(instrument_file);
	fgetc(instrument_file);
	fgetc(instrument_file);
	fgetc(instrument_file);			//Padding

	*instrument_bank = malloc(sizeof(SB_INSTRUMENT) * instrument_number_used);

	for(i = 0; i < instrument_number_all; i ++)
	{
		unsigned int data_index = 0;
		long unsigned int data_offset;

		fseek(instrument_file, name_list_offset + i * (12), 0); //12 is the number of bytes for each instrument name record
															   //This puts the stream pointer at the right byte.

		data_index |= (unsigned int)fgetc(instrument_file);
		data_index |= (unsigned int)fgetc(instrument_file) << 8;

		//debug prints printf("CURRENTLY READING INSTR N %d\n", i);

		if(fgetc(instrument_file) == 1)		//If record is used, go find its data section and read it.
		{
		/*
			getch();
			printf("IS USED\n");
		*/
			current_instrument_data = ((*instrument_bank) + used_instrument_index);
			//^ Sets current instrument data to point to the currently being written to memory instrument

			current_instrument_data->name[0] = fgetc(instrument_file);
			current_instrument_data->name[1] = fgetc(instrument_file);
			current_instrument_data->name[2] = fgetc(instrument_file);
			current_instrument_data->name[3] = fgetc(instrument_file);
			current_instrument_data->name[4] = fgetc(instrument_file);
			current_instrument_data->name[5] = fgetc(instrument_file);
			current_instrument_data->name[6] = fgetc(instrument_file);
			current_instrument_data->name[7] = fgetc(instrument_file); //Gets instrument name.

			// Debug printprintf(current_instrument_data->name);

			data_offset = data_start_offset + data_index * (30);  //30 is the number of bytes of each instrument data record 
			
			fseek(instrument_file, data_offset-1, 0); 

			current_instrument_data->percurssive = (fgetc(instrument_file) & 0x01);
			current_instrument_data->voice_number = fgetc(instrument_file); //Still Dont know what this does, but its unused right now.

			current_instrument_data->modulator.key_scale_level      = 	(fgetc(instrument_file) & 0x03);	//Gets all values for the modulator
			current_instrument_data->modulator.frequency_multiplier =	(fgetc(instrument_file) & 0x0F);
			current_instrument_data->feedback_modulation_factor 	=	(fgetc(instrument_file) & 0x07);	//Only has a significant value for the modulator
			current_instrument_data->modulator.attack_rate			=	(fgetc(instrument_file) & 0x0F);
			current_instrument_data->modulator.sustain_level		=	(fgetc(instrument_file) & 0x0F);
			current_instrument_data->modulator.sustain_enable		=	(fgetc(instrument_file) & 0x01);
			current_instrument_data->modulator.decay_rate			=	(fgetc(instrument_file) & 0x0F);
			current_instrument_data->modulator.release_rate			=	(fgetc(instrument_file) & 0x0F);
			current_instrument_data->modulator.output_level			=	(fgetc(instrument_file) & 0x3F);
			current_instrument_data->modulator.amplitude_vibrato	=	(fgetc(instrument_file) & 0x01);
			current_instrument_data->modulator.frequency_vibrato	=	(fgetc(instrument_file) & 0x01);
			current_instrument_data->modulator.envelope_scaling		=	(fgetc(instrument_file) & 0x01);
			current_instrument_data->type_of_synth					=	(fgetc(instrument_file) & 0x01);	//Only has a significant value for the modulator

			/*
			printf("%d\n", current_instrument_data->modulator.key_scale_level)      ;	//Gets all values for the modulator
			printf("%d\n", current_instrument_data->modulator.frequency_multiplier) ;
			printf("%d\n", current_instrument_data->feedback_modulation_factor 	);	//Only has a significant value for the modulator
			printf("%d\n", current_instrument_data->modulator.attack_rate		)	;
			printf("%d\n", current_instrument_data->modulator.sustain_level		);
			printf("%d\n", current_instrument_data->modulator.sustain_enable	)	;
			printf("%d\n", current_instrument_data->modulator.decay_rate		)	;
			printf("%d\n", current_instrument_data->modulator.release_rate		)	;
			printf("%d\n", current_instrument_data->modulator.output_level		)	;
			printf("%d\n", current_instrument_data->modulator.amplitude_vibrato	);
			printf("%d\n", current_instrument_data->modulator.frequency_vibrato	);
			printf("%d\n", current_instrument_data->modulator.envelope_scaling		);
			printf("%d\n", current_instrument_data->type_of_synth);
			*/

			current_instrument_data->carrier.key_scale_level      	= 	(fgetc(instrument_file) & 0x03);	//Gets all values for the carrier
			current_instrument_data->carrier.frequency_multiplier 	=	(fgetc(instrument_file) & 0x0F);
			fgetc(instrument_file);																			//Skips irrelevant data
			current_instrument_data->carrier.attack_rate			=	(fgetc(instrument_file) & 0x0F);
			current_instrument_data->carrier.sustain_level			=	(fgetc(instrument_file) & 0x0F);
			current_instrument_data->carrier.sustain_enable			=	(fgetc(instrument_file) & 0x01);
			current_instrument_data->carrier.decay_rate				=	(fgetc(instrument_file) & 0x0F);
			current_instrument_data->carrier.release_rate			=	(fgetc(instrument_file) & 0x0F);
			current_instrument_data->carrier.output_level			=	(fgetc(instrument_file) & 0x3F);
			current_instrument_data->carrier.amplitude_vibrato		=	(fgetc(instrument_file) & 0x01);
			current_instrument_data->carrier.frequency_vibrato		=	(fgetc(instrument_file) & 0x01);	
			current_instrument_data->carrier.envelope_scaling		=	(fgetc(instrument_file) & 0x01);	
			fgetc(instrument_file);																			//Skips irrelevant data
			
			current_instrument_data->modulator.waveform				=	(fgetc(instrument_file) & 0x03);
			current_instrument_data->carrier.waveform				=	(fgetc(instrument_file) & 0x03);

			used_instrument_index += 1;		//Increases the currently used instrument number, so that we know where to put the next one on the 
											//Instrument bank struct
		}
	}

	fclose(instrument_file);

	return 0;
}
