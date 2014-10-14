#ifndef STR_H
#define STR_H

#define MAX_STRING_COUNT 10
#define MAX_STRING_SIZE 10

class Str{
private:
	int length;
public:
	char* text;

	Str();

	~Str();

	bool allocStr();

	void freeStr();

	void setText(char* text, int length);

	int getText(char* buffer, int buffer_size=0);

	int getLength();

	bool operator==(const Str &str) const;

	void operator=(const char* const &text);

	void operator=(const Str &str);
};





#endif