obj = main.obj
bin = hello.exe

CC = wcc
CFLAGS = -0
LD = wlink

$(bin): $(obj)
    $(LD) name $@ system dos file main.obj

.c.obj:
    $(CC) $(CFLAGS) $<

clean: .symbolic
    del *.obj
    del $(bin)
