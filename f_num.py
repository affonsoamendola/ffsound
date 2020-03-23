def frequency(note_n):
	return 16.352 *( 2.**(note_n/12.))


def f_num(frequency):
	if(frequency < 48.5):
		block = 0
	elif(frequency < 97.0):
		block = 1
	elif(frequency < 194):
		block = 2
	elif(frequency < 388):
		block = 3
	elif(frequency < 776):
		block = 4
	elif(frequency < 1552.1):
		block = 5
	elif(frequency < 3104.2):
		block = 6
	elif(frequency < 6208.4):
		block = 7

	return (frequency * 2**(20-block)) / 49716 

for i in range(12*8):
	print int(round(f_num(frequency(i))))