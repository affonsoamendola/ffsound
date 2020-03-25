#ifndef SB_H
#define SB_H

#define SB_BASE_ADDRESS 	0x220
#define SB_MPU_401_ADDRESS 	0x330

#define SB_MPU_401_DATA		SB_MPU_401_ADDRESS + 0x0
#define SB_MPU_401_STATUS	SB_MPU_401_ADDRESS + 0x1
#define SB_MPU_401_COMMAND	SB_MPU_401_ADDRESS + 0x1

#define SB_MUSIC_STATUS 					SB_BASE_ADDRESS + 0x0
#define SB_MUSIC_REGISTER_ADDRESS 			SB_BASE_ADDRESS + 0x0
#define SB_MUSIC_REGISTER_DATA 				SB_BASE_ADDRESS + 0x1

#define SB_ADV_FM_MUSIC_STATUS 				SB_BASE_ADDRESS + 0x2
#define SB_ADV_FM_MUSIC_ADDRESS 			SB_BASE_ADDRESS + 0x2
#define SB_ADV_FM_MUSIC_DATA				SB_BASE_ADDRESS + 0x3

#define SB_MIXER_ADDRESS				 	SB_BASE_ADDRESS + 0x4
#define SB_MIXER_DATA						SB_BASE_ADDRESS + 0x5

#define SB_DSP_RESET						SB_BASE_ADDRESS + 0x6

#define SB_FM_MUSIC_STATUS 					SB_BASE_ADDRESS + 0x8
#define SB_FM_MUSIC_ADDRESS 				SB_BASE_ADDRESS + 0x8
#define SB_FM_MUSIC_DATA					SB_BASE_ADDRESS + 0x9

#define SB_DSP_READ_DATA					SB_BASE_ADDRESS + 0xA
#define SB_DSP_WRITE_COMMAND				SB_BASE_ADDRESS + 0xC
#define SB_DSP_WRITE_DATA					SB_BASE_ADDRESS + 0xC
#define SB_DSP_WRITE_BUFFER_STATUS			SB_BASE_ADDRESS + 0xC
#define SB_DSP_READ_BUFFER_STATUS			SB_BASE_ADDRESS + 0xE

#define SB_CD_COMMAND						SB_BASE_ADDRESS + 0x10
#define SB_CD_DATA							SB_BASE_ADDRESS + 0x10
#define SB_CD_STATUS						SB_BASE_ADDRESS + 0x11
#define SB_CD_RESET							SB_BASE_ADDRESS + 0x12
#define SB_CD_ENABLE						SB_BASE_ADDRESS + 0x13

typedef enum NOTE_
{

	C, Cs, D, Ds, E, F, Fs, G, Gs, A, As, B
} NOTE;

typedef struct SB_OPERATOR_
{
	char key_scale_level;    			//Most fields are self explanatory
	char frequency_multiplier;			//4 bits
	char attack_rate;					//4 bits
	char sustain_level;					//4 bits
	char sustain_enable;				//1 bit on or off
	char decay_rate;					//4 bits
	char release_rate;					//4 bits
	char output_level;					//6 bits
	char amplitude_vibrato;				//1 bit on or off
	char frequency_vibrato;				//1 bit on or off
	char envelope_scaling;				//1 bit on or off
	char waveform;						//2 bits

}SB_OPERATOR;

typedef struct SB_INSTRUMENT_
{
	char name[8];
	char percurssive;		//1 bit, 0 melodic, 1 percurssive.
	char voice_number;		//Dont know what this does yet
	SB_OPERATOR modulator; //Remember modulator modulates the carrier.
	SB_OPERATOR carrier;
	char feedback_modulation_factor;	//3 bits
	char type_of_synth; //0 for fm synth 1 for additive synth

}SB_INSTRUMENT;

typedef struct SB_CHANNEL_STRUCT_
{
	SB_INSTRUMENT loaded_instrument;	//currently loaded instrument on this channel
	char key_on;						//If channel "key" is currently being pressed

	char octave;						//Octave of the current note being played
	NOTE note;							//NOTE of the note currently being played.
	char f_block;						//Block num of the current note being played
	unsigned int f_num;					//Fnum of the current note being played

}SB_CHANNEL_STRUCT;

extern SB_INSTRUMENT * SB_INSTRUMENT_BANK;

void sb_init_fm();

void sb_key_on(int channel, NOTE note, int octave);
void sb_key_off(int channel);

void sb_load_instrument(SB_INSTRUMENT instrument, char channel);

void sb_load_instrument_file(const char* path, SB_INSTRUMENT * instrument);
int sb_load_instrument_bank(const char* path, SB_INSTRUMENT** instrument_bank);


#endif