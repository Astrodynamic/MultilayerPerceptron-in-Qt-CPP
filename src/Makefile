#################################################
##              Makefile template              ##
#################################################

SOURCES = chartarea/* controller/* drowarea/* matrix/* network/* normalaizeimage/* primitive/* statecontrol/* tablecontroller/* timeceeper/*

#     These special targets are called phony     #
.PHONY: all dist clean install uninstall dvi build

build: ########################
	cmake -S . -B ./../build
	cmake --build ./../build

install: build ########################
	cp -rf ./../build/MLP.app $(HOME)/Applications/

uninstall: ########################
	rm -rf $(HOME)/Applications/MLP.app

############ Cleaning up unused files ############
clean: ########################
	rm -rf ./../build

################## Style check ###################
style: ########################
	clang-format -n -verbose -style=Google $(SOURCES)

cppcheck: ########################
	cppcheck --std=c++17 --suppress=syntaxError $(SOURCES)
##############  Project archiving   ##############
dist: clean ########################
	tar -czf MLP.tgz ./*

##############     show instuction      ##############
dvi: ########################
	open documentation/README.md
