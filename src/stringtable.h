/*
 * Table for storing string literals.
 */

typedef struct
{
	int cap;
	int size;
	char** strings;
} string_table;

// Create a string table
string_table* CreateStringTable();

// Free string table
void FreeStringTable(string_table* table);

// Add a string
void AddString(string_table* table, char* str);

// Lookup the index of a string, returns -1 if not found
int LookupString(string_table* table, char* str);


