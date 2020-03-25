#ifndef KEYB_H
#define KEYB_H

//MAKE CODES

#define MAKE_ESC				1
#define MAKE_1					2
#define MAKE_2					3
#define MAKE_3					4
#define MAKE_4					5
#define MAKE_5					6
#define MAKE_6					7
#define MAKE_7					8
#define MAKE_8					9
#define MAKE_9					10
#define MAKE_0					11
#define MAKE_MINUS				12
#define MAKE_EQUALS				13
#define MAKE_BKSP				14
#define MAKE_TAB				15
#define MAKE_Q  				16
#define MAKE_W 					17
#define MAKE_E 					18
#define MAKE_R 					19
#define MAKE_T 					20
#define MAKE_Y 					21
#define MAKE_U 					22
#define MAKE_I 					23
#define MAKE_O					24
#define MAKE_P 					25
#define MAKE_LFT_BRACKET		26
#define MAKE_RGT_BRACKET		27
#define MAKE_ENTER				28
#define MAKE_CTRL				29
#define MAKE_A 					30
#define MAKE_S 					31
#define MAKE_D					32
#define MAKE_F 					33
#define MAKE_G 					34
#define MAKE_H 					35
#define MAKE_J 					36
#define MAKE_K  				37
#define MAKE_L 					38
#define MAKE_SEMI				39
#define MAKE_APOS				40
#define MAKE_TILDE				41
#define MAKE_LEFT_SHIFT			42
#define MAKE_BACK_SLASH			43
#define MAKE_Z					44
#define MAKE_X					45
#define MAKE_C 					46
#define MAKE_V					47
#define MAKE_B 					48
#define MAKE_N 					49
#define MAKE_M 					50
#define MAKE_COMMA 				51
#define MAKE_PERIOD 			52
#define MAKE_FORWARD_SLASH		53
#define MAKE_RIGHT_SHIFT		54
#define MAKE_PRT_SCRN			55
#define MAKE_ALT 				56
#define MAKE_SPACE				57
#define MAKE_CAPS_LOCK			58
#define MAKE_F1					59
#define MAKE_F2					60
#define MAKE_F3					61
#define MAKE_F4					62
#define MAKE_F5					63
#define MAKE_F6					64
#define MAKE_F7					65
#define MAKE_F8					66
#define MAKE_F9					67
#define MAKE_F10				68
#define MAKE_F11				87
#define MAKE_F12				88
#define MAKE_NUM_LOCK			69
#define MAKE_SCROLL_LOCK		70
#define MAKE_HOME				71
#define MAKE_UP					72
#define MAKE_PGUP				73
#define MAKE_KEYPAD_MINUS		74
#define MAKE_LEFT 				75
#define MAKE_CENTER				76
#define MAKE_RIGHT 				77
#define MAKE_KEYPAD_PLUS		78
#define MAKE_END 				79
#define MAKE_DOWN				80
#define MAKE_PGDOWN				81
#define MAKE_INS 				82
#define MAKE_DEL 				83

//BREAK CODES

#define BREAK_ESC				129
#define BREAK_1					130
#define BREAK_2					131
#define BREAK_3					132
#define BREAK_4					133
#define BREAK_5					134
#define BREAK_6					135
#define BREAK_7					136
#define BREAK_8					137
#define BREAK_9					138
#define BREAK_0					139
#define BREAK_MINUS				140
#define BREAK_EQUALS			141
#define BREAK_BKSP				142
#define BREAK_TAB				143
#define BREAK_Q  				144
#define BREAK_W 				145
#define BREAK_E 				146
#define BREAK_R 				147
#define BREAK_T 				148
#define BREAK_Y 				149
#define BREAK_U 				150
#define BREAK_I 				151
#define BREAK_O					152
#define BREAK_P 				153
#define BREAK_LFT_BRACKET		154
#define BREAK_RGT_BRACKET		155
#define BREAK_ENTER				156
#define BREAK_CTRL				157
#define BREAK_A 				158
#define BREAK_S 				159
#define BREAK_D					160
#define BREAK_F 				161
#define BREAK_G 				162
#define BREAK_H 				163
#define BREAK_J 				164
#define BREAK_K  				165
#define BREAK_L 				166
#define BREAK_SEMI				167
#define BREAK_APOS				168
#define BREAK_TILDE				169
#define BREAK_LEFT_SHIFT		170
#define BREAK_BACK_SLASH		171
#define BREAK_Z					172
#define BREAK_X					173
#define BREAK_C 				174
#define BREAK_V					175
#define BREAK_B 				176
#define BREAK_N 				177
#define BREAK_M 				178
#define BREAK_COMMA 			179
#define BREAK_PERIOD 			180
#define BREAK_FORWARD_SLASH		181
#define BREAK_RIGHT_SHIFT		182
#define BREAK_PRT_SCRN			183
#define BREAK_ALT 				184
#define BREAK_SPACE				185
#define BREAK_CAPS_LOCK			186
#define BREAK_F1				187
#define BREAK_F2				188
#define BREAK_F3				189
#define BREAK_F4				190
#define BREAK_F5				191
#define BREAK_F6				192
#define BREAK_F7				193
#define BREAK_F8				194
#define BREAK_F9				195
#define BREAK_F10				196
#define BREAK_F11				215
#define BREAK_F12				216
#define BREAK_NUM_LOCK			197
#define BREAK_SCROLL_LOCK		198
#define BREAK_HOME				199
#define BREAK_UP				200
#define BREAK_PGUP				201
#define BREAK_KEYPAD_MINUS		202
#define BREAK_LEFT 				203
#define BREAK_CENTER			204
#define BREAK_RIGHT 			205
#define BREAK_KEYPAD_PLUS		206
#define BREAK_END 				207
#define BREAK_DOWN				208
#define BREAK_PGDOWN			209
#define BREAK_INS 				210
#define BREAK_DEL 				211

//BITMASKS FOR SHIFT STATES

#define BM_SHIFT_RIGHT			0x0001
#define BM_SHIFT_LEFT			0x0002
#define BM_CTRL					0x0004
#define BM_ALT					0x0008
#define BM_SCROLL_LOCK_ON		0x0010
#define BM_NUM_LOCK_ON			0x0020
#define BM_CAPS_LOCK_ON			0x0040
#define BM_INSERT_MODE			0x0080
#define BM_CTRL_LEFT_DOWN		0x0100
#define BM_ALT_LEFT_DOWN		0x0200
#define BM_CTRL_RIGHT_DOWN		0x0400 
#define BM_ALT_RIGHT_DOWN		0x0800
#define BM_SCROLL_LOCK_DOWN		0x1000
#define BM_NUM_LOCK_DOWN		0x2000 
#define BM_CAPS_LOCK_DOWN		0x4000 
#define BM_SYS_REQ_DOWN			0x8000

// KEYBOARD INTEFACE

#define KEYBOARD_INTERRUPT 		0x09	//Keyboard Interrupt number
#define KEY_BUFFER				0x60	//port of the keyboard buffer
#define KEY_CONTROL				0x61	//port of the keyboard controller
#define PIC_PORT				0x20 	//port of the peripheral interrupt controller (PIC)

//OTHER KEYBOARD DEFINES

#define KEY_DOWN				1
#define KEY_UP 					0

extern void On_Key_Down(int scan_code);
extern void On_Key_Up(int scan_code);

#endif 	//KEYB_H
