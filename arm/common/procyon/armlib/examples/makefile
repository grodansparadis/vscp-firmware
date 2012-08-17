
###### Define directories to build ######

SUBDIRS =	rprintf		\
			uart		\

#SUBDIRS = $(dir ./*)

###### Define build rules ######

.PHONY: examples $(SUBDIRS)
examples: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY : all
all : examples

.PHONY : clean
clean : examples
	rm -vf $(addsuffix /GCCProject.ncb, $(SUBDIRS))
	rm -vf $(addsuffix /GCCProject.opt, $(SUBDIRS))
	rm -vf $(addsuffix /GCCProject.plg, $(SUBDIRS))
	rm -vf ../*.lst
	rm -vf ../arch/aduc7000/*.lst
	rm -vf ../arch/at91/*.lst
	rm -vf ../arch/lpc2000/*.lst

