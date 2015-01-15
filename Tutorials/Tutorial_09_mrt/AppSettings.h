#pragma once

#include <string>

template <typename T>
void IncrementSetting(T& setting, int maxValue)
{
	int val = static_cast<int>(setting);
	val = (val + 1) % maxValue;
	setting = static_cast<T>(val);
}

class DisplayMode
{
public:
	enum Settings{
		FirstPass,
		FinalPass,
		NumSettings
	};
	static const wchar_t Key = 'D';
	static Settings Value;
	static std::wstring ToString()
	{
		static const std::wstring Names[NumSettings] =
		{
			L"FirstPass targets",
			L"FinalPass target"
		};

		std::wstring text = L"Disaply Mode(D): ";
		text += Names[Value];
		return text;
	}
	static void Increment()
	{
		IncrementSetting(Value, NumSettings);
	}
};