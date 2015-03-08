LEX = flex
YACC = bison

CC = gcc 

u: ulex.c uprelex.c u.tab.c upre.tab.c symboltable.c symbolstack.c functiontable.c parsetree.c compiler.c strutil.c stringtable.c
	$(CC) -o u ulex.c uprelex.c u.tab.c upre.tab.c symboltable.c symbolstack.c functiontable.c parsetree.c compiler.c strutil.c stringtable.c list.c identifiertypes.c stringqueue.c prunefunctions.c optimizer.c -ll
	rm ulex.c uprelex.c upre.tab.c upre.tab.h u.tab.c u.tab.h
	
debug: ulex.c uprelex.c u.tab.c upre.tab.c symboltable.c symbolstack.c functiontable.c parsetree.c compiler.c strutil.c stringtable.c
	$(CC) -g -o u ulex.c uprelex.c u.tab.c upre.tab.c symboltable.c symbolstack.c functiontable.c parsetree.c compiler.c strutil.c stringtable.c list.c identifiertypes.c stringqueue.c prunefunctions.c optimizer.c -ll
	rm ulex.c uprelex.c upre.tab.c upre.tab.h u.tab.c u.tab.h

u.tab.c u.tab.h: u.y
	rm -f u.tab.h u.tab.c upre.tab.h upre.tab.c
	$(YACC) -d u.y
	$(YACC) -d upre.y

ulex.c uprelex.c: u.l u.tab.h upre.l upre.tab.h
	rm -f ulex.c uprelex.c
	$(LEX) u.l
	$(LEX) upre.l

clean:
	rm -f u u.tab.h u.tab.c upre.tab.c upre.tab.h ulex.c uprelex.c
