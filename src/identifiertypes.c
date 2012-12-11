#include "identifiertypes.h"

// Get string for an identifier type
char* getTypeString(identifier_type t)
{
	switch (t)
	{
		case IT_VOID:
			return "IT_VOID";
		case IT_BYTE:
			return "IT_BYTE";
		case IT_WORD:
			return "IT_WORD";
		case IT_BOOL:
			return "IT_BOOL";
		case IT_BYTEP:
			return "IT_BYTEP";
		case IT_WORDP:
			return "IT_WORDP";
		default:
			return "UKNOWN TYPE";
	}
}

