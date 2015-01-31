ifdef CROSS_COMPILE
#CC := $(CROSS_COMPILE)gcc
endif
OUTDIR = bin
HOST1 = 10.10.2.3
HOST2 = 10.10.2.2
NETCARD = eth1
all:
	mkdir -p  $(OUTDIR)
#	$(CC) -Wall comtest_main.c -o $(OUTDIR)/BurnInTest-CSC -lpthread
	$(CC) -Wall -DNETCARD=\"$(NETCARD)\" -DSRCHOST=\"$(HOST1)\" -DDSTHOST=\"$(HOST2)\" comtest_main.c selfping.c log.c -o $(OUTDIR)/BurnInTest-CSCA -lpthread
	$(CC) -Wall -DNETCARD=\"$(NETCARD)\" -DSRCHOST=\"$(HOST2)\" -DDSTHOST=\"$(HOST1)\" comtest_main.c selfping.c log.c -o $(OUTDIR)/BurnInTest-CSCB -lpthread
	$(CC) -Wall -DNETCARD=\"$(NETCARD)\" -DSRCHOST=\"$(HOST1)\" -DDSTHOST=\"$(HOST2)\" comtest_main_pair.c selfping.c log.c -o $(OUTDIR)/pairBurnInTest-CSCA -lpthread
	$(CC) -Wall -DNETCARD=\"$(NETCARD)\" -DSRCHOST=\"$(HOST2)\" -DDSTHOST=\"$(HOST1)\" comtest_main_pair.c selfping.c log.c -o $(OUTDIR)/pairBurnInTest-CSCB -lpthread
	$(CC) -Wall comtestA.c -o $(OUTDIR)/pairAComTest 
	$(CC) -Wall comtestB.c -o $(OUTDIR)/pairBComTest 
	$(CC) -Wall comtest_lookback.c -o $(OUTDIR)/lookbackTest 
	$(CC) -Wall comtest_pair.c -o $(OUTDIR)/pairTest 
	$(CC) -Wall timerkill.c -o $(OUTDIR)/timerkill 
	cp ftpupdate.sh $(OUTDIR)/
clean:
	rm $(OUTDIR)/*
