main: myshell.cpp
	g++ -std=gnu++11 -o myShell myshell.cpp stoarg.h stoarg.cpp myexception.h mydict.h mydict.cpp myvar.h myvar.cpp myexecute.h myexecute.cpp -Wall -Werror -pedantic #-O2
