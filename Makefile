compile: SearchLib/main.c
	gcc -Wall SearchLib/main.c -o SearchLib/RepoScannerProgram

install: compile
	mkdir -p /opt/repoScanner/SearchLib
	cp SearchLib/RepoScannerProgram /opt/repoScanner/SearchLib/
	ln /opt/repoScanner/SearchLib/RepoScannerProgram /bin/reposcan

uninstall:
	rm -r /opt/repoScanner/
	rm /bin/reposcan
