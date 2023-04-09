#include "Keyboard.h"


bool Keyboard::KeyIsPressed(unsigned char keycode) {
	return keystates[keycode];
}

Keyboard::Event Keyboard::ReadKey() {
	if (keybuffer.size() > 0u) {
		Keyboard::Event e = keybuffer.front();
		keybuffer.pop();
		return e;
	}
	else {
		return Keyboard::Event();
	}
}

bool Keyboard::KeyIsEmpty() {
	return keybuffer.empty();
}

char Keyboard::ReadChar() {
	if (charbuffer.size() > 0u) {
		unsigned char charcode = charbuffer.front();
		charbuffer.pop();
		return charcode;
	}

	return 0;
}

bool Keyboard::CharIsEmpty() {
	return charbuffer.empty();
}

void Keyboard::FlushKey() {
	keybuffer = std::queue<Event>();
}

void Keyboard::FlushChar() {
	charbuffer = std::queue<char>();
}

void Keyboard::Flush() {
	FlushKey();
	FlushChar();
}

void Keyboard::EnableAutorepeat() {
	autorepeatEnabled = true;
}

void Keyboard::DisableAutorepeat() {
	autorepeatEnabled = false;
}

bool Keyboard::AutorepeatIsEnabled() {
	return autorepeatEnabled;
}

void Keyboard::OnKeyPressed(unsigned char keycode) {
	keystates[keycode] = true;
	keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, keycode));
	TrimBuffer(keybuffer);
}

void Keyboard::OnKeyReleased(unsigned char keycode) {
	keystates[keycode] = false;
	keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));
	TrimBuffer(keybuffer);
}

void Keyboard::OnChar(char character) {
	charbuffer.push(character);
	TrimBuffer(charbuffer);
}

void Keyboard::ClearState() {
	keystates.reset();
}

template<typename T>
static void Keyboard::TrimBuffer(std::queue<T>& buffer) {
	while (buffer.size() > bufferSize) {
		buffer.pop();
	}
}