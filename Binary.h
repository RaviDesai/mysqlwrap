#pragma once

#include <string>

class Binary {
public:
	Binary();
	Binary(size_t bufferSize);
	Binary(unsigned char *buffer, size_t bufferLength, size_t bufferSize);
	Binary(Binary &copy);
	virtual ~Binary();
	Binary &operator=(const Binary &equal);

	void ResizeBuffer(size_t newSize);
	void AssignDataToBuffer(unsigned char *data, size_t dataLength);
	void SubsumeBuffer(unsigned char *data, size_t dataLength, size_t bufferLength);
	void SubsumeBuffer(Binary &binary);
	void ClearBuffer();

	unsigned char *Buffer() const;
	size_t BufferLength() const;
	size_t BufferSize() const;	
private:
	unsigned char *_buffer;
	size_t _bufferLength;
	size_t _bufferSize;
};

bool operator==(const Binary &left, const Binary &right);
