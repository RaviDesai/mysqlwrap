#include "Binary.h"
#include <iostream>

using namespace std;

Binary::Binary() {
	_bufferLength = 0;
	_bufferSize = 0;
	_buffer = NULL;
}

Binary::Binary(size_t bufferSize) {
	_bufferLength = 0;
	_bufferSize = bufferSize;
	_buffer = (unsigned char *)malloc(_bufferSize);
}

Binary::Binary(unsigned char *buffer, size_t bufferLength, size_t bufferSize) {
	_bufferSize = bufferSize;
	if (bufferLength > bufferSize) {
		_bufferSize = bufferLength;
	}
	_bufferLength = bufferLength;
	_buffer = buffer;
}

Binary::Binary(Binary &copy) {
	_buffer = NULL;
	_bufferLength = 0;
	_bufferSize = 0;
	SubsumeBuffer(copy);
}

Binary &Binary::operator=(const Binary &equal) {
	if (_bufferSize < equal._bufferSize) {
		if (_buffer != NULL) {
			free(_buffer);
		}
		_bufferSize = equal._bufferSize;
		_buffer = (unsigned char *)malloc(_bufferSize);
	}
	_bufferLength = equal._bufferLength;
	if (_bufferLength > 0) {
		memcpy(_buffer, equal._buffer, _bufferLength);
	}
	return *this;
}

void Binary::ClearBuffer() {
	if (_buffer != NULL) { 
		free(_buffer);
		_buffer = NULL;
	}
	_bufferSize = 0;
	_bufferLength = 0;
}

Binary::~Binary() {
	ClearBuffer();
}

size_t Binary::BufferLength() const {
	return _bufferLength;
}

size_t Binary::BufferSize() const {
	return _bufferSize;
}

unsigned char *Binary::Buffer() const {
	return _buffer;
}

void Binary::ResizeBuffer(size_t newSize) {
	if (_bufferLength > newSize) return;

	unsigned char *newBuffer = (unsigned char *)malloc(newSize);
	if (_buffer != NULL)
	{
		if (_bufferLength > 0) {
			memcpy(newBuffer, _buffer, _bufferLength);
		}
		free(_buffer);
	}

	_bufferSize = newSize;
	_buffer = newBuffer;
}

void Binary::AssignDataToBuffer(unsigned char *data, size_t dataLength) { 
	if (_bufferSize < dataLength) {
		_bufferLength = 0;
		ResizeBuffer(dataLength);
	}

	_bufferLength = dataLength;
	memcpy(_buffer, data, _bufferLength);
}

void Binary::SubsumeBuffer(unsigned char *data, size_t dataLength, size_t bufferSize) {
	if (_buffer != NULL) {
		free(_buffer);
	}
	_bufferSize = bufferSize;
	_bufferLength = dataLength;
	_buffer = data;
}

void Binary::SubsumeBuffer(Binary &binary) {
	SubsumeBuffer(binary._buffer, binary._bufferLength, binary._bufferSize);
	binary._buffer = NULL;
	binary._bufferSize = 0;
	binary._bufferLength = 0;
}

bool operator==(const Binary &left, const Binary &right) {
	unsigned char* leftBuffer = left.Buffer();
	unsigned char* rightBuffer = right.Buffer();
	size_t leftLen = left.BufferLength();
	size_t rightLen = right.BufferLength();

	if (leftLen != rightLen) return false;

	for (size_t i = 0; i < leftLen; i++) {
		if (leftBuffer[i] != rightBuffer[i]) return false;
	}
	return true;
}
