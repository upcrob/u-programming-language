U Programming Language
--------------------------------------------------------------------------------
Version 0.0.1 Documentation 12/11/2012
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Introduction
--------------------------------------------------------------------------------

Around 10 years ago, I became obsessed with computer operating systems.  I'm not
entirely sure why, but I think the idea of controlling the entire computer, the
individual bits and bytes, appealed to me.  At the time, I had been programming
for a couple years and, being young and naïve, I decided I would take on the
challenge of writing my own hobby system.

I had heard that one needed assembly language (at least at some level) to write
such programs.  From here, I searched the Internet (not as simple back then with
my finicky dialup connection) for free x86 assemblers and documentation relating
to them.  I found a couple good ones, and proceeded to write my system.

...but assembly was HARD!  It was a lot of fun to write small optimized blocks
of code (and I still get a good deal of enjoyment from this) but a few years
later and with only meager results (I managed to develop a control program with
a basic command prompt and the beginnings of a file system) I put the project on
hold indefinitely.  I attempted to rewrite the system in C, but constantly got
fed up with linkers and some of the housekeeping inherent to the language.  The
OS I wanted to write was just for fun, and using a language like this would be
sort of like pheasant hunting with a howitzer, to borrow a phrase.

Recently, during my senior year as an undergrad, I took a programming languages
and compilers course.  In it, I was exposed to the tools Flex and Bison and
they instantly became my new favorite software toys.  Prior to this, I had been
thinking of things that I would like in my ideal language, particularly a
simple, albeit low-level one.  After graduating, I was able to implement a
number of these ideas in the compiler for this language that I dubbed, “U”. 
Granted, not all of the features that would make it an ideal language are, or
may ever be, added to it (I'm quite lazy).  But, in the words of Bjarne
Stroustrup, such endeavors constitute a “sterile quest for perfection”.

Some things I wanted U to be:
-----------------------------
* a hobby language - writing a compiler for the sake of writing a compiler
* tightly integrated with x86 assembly -> easy to read / write low-level
	code for real mode
* easy to learn / read overall -> no curly braces, small list of reserved
	words, simple grammar (granted, some of these things are due to my
	inherent laziness in coding it up)
* education -> maybe it can be helpful for learning low-level programming,
	assembly, and how compilers work in general

Some things that it's not meant to be / isn't:
----------------------------------------------
* well tested and suited for production code
* portable (it's not C)
* completely elegant and efficient (take a look at my source files - you'll
	understand)

At any rate, feel free to play around with the language.  I can't promise it's
bug free (in fact, I know it's not), but hopefully it will get improved further
over time.  Like Larry Wall, I reserve the right to be its "benevolent
dictator", but feel free to make suggestions and use the source code for your
own projects.  I'm not releasing it under any particular license, but let it be
known that the source code should be used AT YOUR OWN RISK and I do request that
the philosophy of "don't be evil" be followed.  It should finally be noted that
small portions of the code (mostly in the parsetree and symboltable files) were
written by Dr. Brian Turnquist at Bethel University and I have him to thank for
the excellent intro course he taught on compiler construction.

With that, here's some sparse documentation.  Happy coding!
-Rob


--------------------------------------------------------------------------------
Building
--------------------------------------------------------------------------------
To make a working executable, you will need the following installed on your
development machine:
* flex (http://flex.sourceforge.net)
* bison (http://gnu.org/software/bison)
* a C compiler (I used GCC on an Ubuntu box)

In theory, other compilers and platforms should work but I haven't done
extensive testing.

To build the compiler, change to the directory containing the source code and
type "make".  This will build an executable using the included Makefile.

--------------------------------------------------------------------------------
Compiler
--------------------------------------------------------------------------------
The U compiler compiles source code to a single output file containing Intel x86
opcodes.  These can then be assembled to executable machine code with assemblers
such as FASM or NASM.

Invoking the compiler:
u [input file] [optional output file] [flags]

Invoking the compiler without any input arguments will present the user with
a help screen outlining the options offered.


--------------------------------------------------------------------------------
Language Features & Examples
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
General Program Structure
--------------------------------------------------------------------------------
Every program in U, like C, contains a main() function that returns 'void'.
Note that instead of opening and closing braces, each function (and other
code blocks in the language) are terminated with the 'end' keyword:
	
	void main()
		// code here
	end

Comments, as in C, are expressed with either single line (//) or multi-line (/*
and */) syntax.  Every program is composed of a collection of functions like the
following example.  Note that the compiler takes multiple passes through the
input files, so header files, prototypes, and function ordering are neither
necessary nor relevant.  Also, 'import' statements can be used outside of
function blocks to import external U source files into the compiler:
	
	// Import some other source files
	import "somefile.u";
	import "anotherfile.u";
	
	/* Main function */
	void main()
		print("Hello, world");
		putc('!');
	end
	
	/* Print function */
	void print(byte[] str)
		// some code for printing the input string . . .
	end
	
	/* Character printing function */
	void putc(byte c)
		// code for printing a single character . . .
	end

--------------------------------------------------------------------------------
Variables and Data Types
--------------------------------------------------------------------------------
U includes 5 data types (in addition to the 'void' type) that can be used to
declare variables within a program:

byte - An 8 bit byte.
word - A 16 bit word.
bool - Data type dedicated to storing boolean (true/false) values.
byte[] - A pointer to an array of bytes.  The pointer itself requires 32 bits
	of memory.
word[] - A pointer to an array of words.  The pointer itself requires 32 bits
	of memory.

Variables are declared and initialized in a manner very similar to C:
	
	void main()
		byte a = 'A';	// store the character 'A' in the variable, a
		byte c;			// declare the variable 'C'
		c = a;			// set c to a's value
		
		// Here we declare a variable called, x, and initialize it to point to
		// the location 10:5 in memory.  In U, the value 10:5 represents the
		// 10th segment in memory with an offset of 5 bytes (seg:off).  This
		// ':' operator can be used this way in other non-constant expressions
		// as well
		byte[] x = 10:5;
		byte[] y;			// declare the byte pointer, y
		y = x;				// set y's pointer value to x's pointer value
		
		// Point str to the string "hello, world!" in memory.  Note that this
		// string is stored in a single location, so modifying it will modify
		// in other locations that point to the identical string.
		byte[] str = "hello, world!";
	end
	
As in C, basic mathematical operators (+, -, *, and /) as well as % (modulus)
are supported.  Increment (++) and decrement (--), as well as other shortcuts
like the '+=' operator do not currently exist in the language.

--------------------------------------------------------------------------------
Conditional Statements
--------------------------------------------------------------------------------
At present, the only conditional control structure supported in U is the if-else
block.  Note that the words 'true' and 'false' are reserved and have their
conventional boolean values:
	
	void main()
		byte a = 'A';
		byte b = 'B';
		
		// simple if-else
		if (a == b)
			// this code shouldn't be executed
		else
			// this code should be executed
		end
		
		// if (without else)
		if (false)
			// never execute this block
		end
		
		// if with else-if blocks
		if (a == 'C')
			// don't execute this
		else if (a == 'B')
			// don't execute this either
		else if (a == 'A')
			// this looks right!
		else
			// an extra condition, just in case
		end
	end

--------------------------------------------------------------------------------
Looping
--------------------------------------------------------------------------------
Currently, the only looping structure supported in U is the 'while' block:
	
	include "someio.u";	
	void main()
		word i = 0;
		
		// print 'X' 10 times
		while (i < 10)
			putc('X');
			i = i + 1;
		end
	end
	

--------------------------------------------------------------------------------
Inline Assembly
--------------------------------------------------------------------------------
One of the relatively unique features of U is that is is designed to support
inline Intel x86 assembly.  At present, only the 'mov' and 'int' calls are
supported (to take advantage of BIOS interrupt calls) but more are planned to be
added as the language is further developed:

	/* Main function */
	void main()
		putc('A');		// print character
		putc(getc());	// print a character that the user types
	end
	
	/* Function that actually prints a character using a BIOS call */
	void putc(byte c)
		asm					// start of assembly block
			mov ah, 0Eh
			mov al, [c]
			int 10h
		end					// end of assembly block
	end
	
	/* Function that gets a character from the keyboard using a BIOS call */
	byte getc()
		byte c;
		asm
			mov ah, 0
			int 16h
			mov [c], al
		end
		return c;
	end
