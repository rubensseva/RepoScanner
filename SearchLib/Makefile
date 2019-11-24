main.o:
	gcc -Wall -c main.c

paths.o:
	gcc -Wall -c paths.c

compile: main.o paths.o
	gcc -Wall main.o paths.o -o RepoScannerProgram

install: compile
	mkdir -p /opt/repoScanner/SearchLib
	cp RepoScannerProgram /opt/repoScanner/SearchLib/
	ln /opt/repoScanner/SearchLib/RepoScannerProgram /bin/reposcan

clean:
	rm main.o paths.o RepoScannerProgram

uninstall:
	rm -r /opt/repoScanner/
	rm /bin/reposcan
