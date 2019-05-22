ex: ex.o
	$(CCC) $(CCFLAGS) ex.o -o ex $(CCLNFLAGS)
ex.o: ex.cpp
	$(CCC) -c $(CCFLAGS) ex.cpp -o ex.o
clean:
	rm --force ex ex.o

################################

# Uncomment for CPLEX_Enterprise_Server126
# SYSTEM     = x86-64_linux
# LIBFORMAT  = static_pic
# CPLEXDIR      = /opt/ibm/ILOG/CPLEX_Enterprise_Server126/CPLEX_Studio/cplex
# CONCERTDIR    = /opt/ibm/ILOG/CPLEX_Enterprise_Server126/CPLEX_Studio/concert

# Uncomment for CPLEX_Studio124
SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic
CPLEXDIR      = /home/sergio/MIRI/CPLEX/cplex
CONCERTDIR    = /home/sergio/MIRI/CPLEX/concert

################

CCC = g++

CCOPT = -m64 -O3 -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lm -pthread

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR)
