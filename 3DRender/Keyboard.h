#pragma once
#include <queue>
#include <bitset>

class Keyboard {

	friend class Window;

public:
	class Event
	{
	public:
		enum class Type
		{
			Press,
			Release,
			Invalid
		};
	private:
		Type type;
		unsigned char code;
	public:
		Event() : type(Type::Invalid), code(0u) {}
		Event(Type type, unsigned char code) : type(type), code(code){}
		bool IsPress(){
			return type == Type::Press;
		}
		bool IsRelease(){
			return type == Type::Release;
		}
		unsigned char GetCode(){
			return code;
		}
	};
public:
	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;

	// key event stuff
	bool KeyIsPressed(unsigned char keycode);
	Event ReadKey();
	bool KeyIsEmpty();
	void FlushKey();
	// char event stuff
	char ReadChar();
	bool CharIsEmpty();
	void FlushChar();
	void Flush();
	// autorepeat control
	void EnableAutorepeat();
	void DisableAutorepeat();
	bool AutorepeatIsEnabled();
private:
	void OnKeyPressed(unsigned char keycode);
	void OnKeyReleased(unsigned char keycode);
	void OnChar(char character);
	void ClearState();
	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer);
private:
	static constexpr unsigned int nKeys = 256u;
	static constexpr unsigned int bufferSize = 16u;
	bool autorepeatEnabled = false;
	std::bitset<nKeys> keystates;
	std::queue<Event> keybuffer;
	std::queue<char> charbuffer;
};
