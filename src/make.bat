flex u.l
flex upre.l
bison -d u.y
bison -d upre.y
gcc -o u ulex.c uprelex.c u.tab.c upre.tab.c symboltable.c symbolstack.c functiontable.c parsetree.c compiler.c strutil.c stringtable.c list.c identifiertypes.c stringqueue.c prunefunctions.c