/*
 * String utility functions.
 */

#pragma once

int streq(char* a, char* b);

const char* regStr(int reg);

void strToLower(char* str);

char* getBasename(char* filePath);

char* directoryPath(char* filePath);

char* outputPath(char* filePath);

int bintoint(char* binStr);


