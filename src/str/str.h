#ifndef STR_H
#define STR_H

#include "config.h"

#ifndef MAX_STRING_COUNT
#define MAX_STRING_COUNT 100 // test with 3.
#endif
#ifndef MAX_STRING_SIZE
#define MAX_STRING_SIZE 10
#endif

class Str{
private:
	int length;
public:
	char* text;

	Str();

	~Str();

	// Makes text point to a char array and indicates that its occupied
	// returns false in case there are no more strings to point.
	bool allocStr();

	// Set free string pointed.
	void freeStr();

	// writes text (parameter) on Str text.
	// returns true in success, false otherwise.
	bool setText(const char* text, int length);

	// writes text on buffer and returns text lenght on int;
	int getText(char* buffer, int buffer_size=0);

	// returns text length;
	int getLength();

	// bool operator==(const Str &str) const;

	// bool operator==(const char* &str) const;


};





#endif