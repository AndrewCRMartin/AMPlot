CC   = gcc
COPT = -Wall 
ANSI = /home/andrew/bin/ansi
LFLAGS = -lm

OFILES = main.o commands.o port.o plots.o smooth.o handlers.o \
	 svdfit.o plotting.o psroutines.o hpgl.o csclip.o fonts.o \
	 FileIO.o parse.o axes.o piechart.o barchart.o robust.o \
         myio.o
# Amiga only
AOFILES = rexx.o messages.o dr2d.o iffutil.o menus.o requesters.o \
	  fonts_amiga.o main_amiga.o plotting_amiga.o plots_amiga.o \
	  handlers_amiga.o
PFILES = menus.p main.p requesters.p handlers.p \
         FileIO.p plots.p commands.p axes.p \
         fonts.p plotting.p svdfit.p smooth.p \
         robust.p rexx.p barchart.p piechart.p \
         hpgl.p messages.p dr2d.p iffutil.p main_amiga.p plots_amiga.p \
         plotting_amiga.p fonts_amiga.p port.p handlers_amiga.p 
IFILES = amplot.h interface.h myio.h 

amplot : $(OFILES)
	$(CC) $(COPT) -o amplot $(OFILES) $(LFLAGS)


main.o : main.c $(IFILES) menudefs.h
	$(CC) $(COPT) -c main.c

commands.o : commands.c $(IFILES) menudefs.h
	$(CC) $(COPT) -c commands.c

rexx.o : rexx.c $(IFILES) menudefs.h
	$(CC) $(COPT) -c rexx.c

psroutines.o :    psroutines.c   ps.h
	$(CC) $(COPT) -c psroutines.c

hpgl.o :          hpgl.c         hpgl.h
	$(CC) $(COPT) -c hpgl.c

messages.o :      messages.c
	$(CC) $(COPT) -c messages.c

dr2d.o :          dr2d.c         dr2d.h
	$(CC) $(COPT) -c dr2d.c

iffutil.o :       iffutil.c
	$(CC) $(COPT) -c iffutil.c

.c.o :  $< $(IFILES)
	$(CC) $(COPT) -o $@ -c $<
   

clean :
	rm -f $(OFILES) $(AOFILES)

protos : $(PFILES)


.c.p :
	$(ANSI) -p $< $@

