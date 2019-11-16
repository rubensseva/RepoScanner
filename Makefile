
compile: SearchLib/main.c
	gcc -Wall SearchLib/main.c -o SearchLib/RepoScannerProgram

install: compile
	mkdir -p /opt/repoScanner/SearchLib
	cp RepoScanner.sh /opt/repoScanner/
	chmod +x /opt/repoScanner/RepoScanner.sh
	cp SearchLib/RepoScannerProgram /opt/repoScanner/SearchLib/
	ln /opt/repoScanner/RepoScanner.sh /bin/reposcan

uninstall:
	rm -r /opt/repoScanner/
	rm /bin/reposcan
