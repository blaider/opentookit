ifdef CROSS_COMPILE
#CC := $(CROSS_COMPILE)gcc
endif
OUTDIR = bin
all:
	mkdir -p  $(OUTDIR)
	$(CC) -Wall comtest_main.c -o $(OUTDIR)/BurnInTest-CSC -lpthread
	$(CC) -Wall comtestA.c -o $(OUTDIR)/AComTest 
	$(CC) -Wall comtestB.c -o $(OUTDIR)/BComTest 
	$(CC) -Wall comtest_lookback.c -o $(OUTDIR)/lookbackTest 
	$(CC) -Wall timerkill.c -o $(OUTDIR)/timerkill 
	cp ftpupdate.sh $(OUTDIR)/
clean:
	rm $(OUTDIR)/*
