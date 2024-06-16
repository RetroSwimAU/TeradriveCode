obj = regdump.obj
bin = regdump.exe

CC = wcc
CFLAGS = -0
LD = wlink

$(bin): $(obj)
    $(LD) name $@ system dos file regdump.obj

.c.obj:
    $(CC) $(CFLAGS) $<

clean: .symbolic
    del *.obj
    del $(bin)
