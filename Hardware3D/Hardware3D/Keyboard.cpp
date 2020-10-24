﻿#include "Keyboard.h"

bool Keyboard::KeyIsPressed(unsigned char keycode) const noexcept
{
	return keyStates[keycode];
}

Keyboard::Event Keyboard::ReadKey() noexcept
{
	if (keyBuffer.empty()) return Keyboard::Event();
	

	const Keyboard::Event e = keyBuffer.front();
	keyBuffer.pop();
	return e;
}

bool Keyboard::KeyIsEmpty() const noexcept
{
	return keyBuffer.empty();
}
char Keyboard::ReadChar() noexcept
{
	if (charBuffer.empty()) return 0;
	
	unsigned char charCode = charBuffer.front();
	charBuffer.pop();
	return charCode;
}

bool Keyboard::CharIsEmpty() const noexcept
{
	return charBuffer.empty();
}

void Keyboard::FlushKey() noexcept
{
	keyBuffer = std::queue<Event>();
}

void Keyboard::FlushChar() noexcept
{
	charBuffer = std::queue<char>();
}

void Keyboard::Flush() noexcept
{
	FlushKey();
	FlushChar();
}

void Keyboard::EnableAutorepeat() noexcept
{
	autorepeatEnabled = true;
}

void Keyboard::DisableAutorepeat() noexcept
{
	autorepeatEnabled = false;
}

bool Keyboard::AutorepeatIsEnabled() const noexcept
{
	return autorepeatEnabled;
}

void Keyboard::OnKeyPressed(unsigned char keycode) noexcept
{
	keyStates[keycode] = true;
	keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));
	TrimBuffer(keyBuffer);
}

void Keyboard::OnKeyReleased(unsigned char keycode) noexcept
{
	keyStates[keycode] = false;
	keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));
	TrimBuffer(keyBuffer);
}

void Keyboard::OnChar(char character) noexcept
{
	charBuffer.push(character);
	TrimBuffer(charBuffer);
}

void Keyboard::ClearState() noexcept
{
	keyStates.reset();
}

template <typename T>
void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept
{
	while (buffer.size() > bufferSize)
	{
		buffer.pop();
	}
}
