#	Copyright Affonso Amendola 2019-2020			#
#													#
#	Foffonso's Sound Blaster Xperiment				#
#---------------------------------------------------#

obj = sb.obj
bin = sb.exe

CC = wcc
CFLAGS = -zq -otx
LD = wlink
INCLUDE = .
LIB = /usr/share/openwatcom/lib286/dos/:/usr/share/openwatcom/lib286

$(bin): $(obj)
	$(LD) name $@ file { $(obj) } libpath $(LIB) form dos

.c.obj:
	$(CC) -i$(INCLUDE) -fo=$@ $(CFLAGS) $<

clean: .symbolic
	rm *.obj
	rm *.err
	rm $(bin)

run: .symbolic
	dosbox $(bin)
