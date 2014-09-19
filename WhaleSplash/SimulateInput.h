#include <vector>

class CInputArray
{
private:
	std::vector<INPUT> m_inputs;

public:
	int Size()
	{
		return m_inputs.size();
	}

	INPUT* ToArray()
	{
		return &(m_inputs.front());
	}

	void AddKeyDown(WORD keyCode)
	{
		INPUT input;
		input.type = INPUT_KEYBOARD;
		input.ki.wVk = LOBYTE(keyCode);
		input.ki.wScan = 0;
		input.ki.dwFlags = 0;
		input.ki.time = 0;
		input.ki.dwExtraInfo = NULL;

		m_inputs.insert(m_inputs.end(), input);
	}

	void AddKeyUp(WORD keyCode)
	{
		INPUT input;
		input.type = INPUT_KEYBOARD;
		input.ki.wVk = LOBYTE(keyCode);
		input.ki.wScan = 0;
		input.ki.dwFlags = KEYEVENTF_KEYUP;
		input.ki.time = 0;
		input.ki.dwExtraInfo = NULL;

		m_inputs.insert(m_inputs.end(), input);
	}

	void AddKeyPress(WORD keyCode)
	{
		AddKeyDown(keyCode);
		AddKeyUp(keyCode);
	}

	void AddCharacter(TCHAR character)
	{
		UINT16 scanCode = character;

		INPUT down;
		down.type = INPUT_KEYBOARD;
		down.ki.wVk = 0;
		down.ki.wScan = scanCode;
		down.ki.dwFlags = KEYEVENTF_UNICODE;
		down.ki.time = 0;
		down.ki.dwExtraInfo = NULL;

		INPUT up;
		up.type = INPUT_KEYBOARD;
		up.ki.wVk = 0;
		up.ki.wScan = scanCode;
		up.ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_UNICODE;
		up.ki.time = 0;
		up.ki.dwExtraInfo = NULL;

		if ( (scanCode & 0xFF00) == 0xE000 )
		{
			down.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
			up.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
		}

		m_inputs.insert(m_inputs.end(), down);
		m_inputs.insert(m_inputs.end(), up);
	}

	void AddCharacters(LPCTSTR characters)
	{
		int nLength = _tcslen(characters);
		for ( int i = 0; i < nLength; i++ )
		{
			AddCharacter(characters[i]);
		}
	}
};


class CSimulateInput
{
private:
	UINT SendInput(CInputArray& input)
	{
		return ::SendInput(input.Size() , input.ToArray(), sizeof(INPUT));
	}

public:
	BOOL Block()
	{
		return ::BlockInput(TRUE);
	}

	BOOL Unblock()
	{
		return ::BlockInput(FALSE);
	}

	void KeyPress(WORD keyCode)
	{
		CInputArray input;
		input.AddKeyPress(keyCode);

		SendInput(input);
	}

	void SendText(LPCTSTR text)
	{
		CInputArray input;
		input.AddCharacters(text);

		SendInput(input);
	}
};