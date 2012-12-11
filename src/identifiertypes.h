/*
 * Identifier types.
 */

#pragma once

typedef enum identifier_type_tag
{
	IT_VOID,
    IT_BYTE,
    IT_WORD,  
   	IT_BOOL,
   	IT_BYTEP,    
    IT_WORDP 
} identifier_type;

char* getTypeString(identifier_type t);

