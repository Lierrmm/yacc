#include <std_include.hpp>
#include <loader/module_loader.hpp>

#include "game/game.hpp"

#include <utils/string.hpp>
#include <utils/hook.hpp>

#include "ui_script.hpp"

std::unordered_map<std::string, utils::string::Slot<UIScript::Callback>> UIScript::UIScripts;
std::unordered_map<int, utils::string::Slot<UIScript::CallbackRaw>> UIScript::UIOwnerDraws;

template<> int UIScript::Token::get()
{
	if (this->isValid())
	{
		return atoi(this->token);
	}

	return 0;
}

template<> const char* UIScript::Token::get()
{
	if (this->isValid())
	{
		return this->token;
	}

	return "";
}

template<> std::string UIScript::Token::get()
{
	return this->get<const char*>();
}

bool UIScript::Token::isValid()
{
	return (this->token && this->token[0]);
}

void UIScript::Token::parse(const char** args)
{
	if (args)
	{
		this->token = game::native::Com_Parse(args);
	}
}

void UIScript::Add(const std::string& name, utils::string::Slot<UIScript::Callback> callback)
{
	UIScript::UIScripts[name] = callback;
}

void UIScript::AddOwnerDraw(int ownerdraw, utils::string::Slot<UIScript::CallbackRaw> callback)
{
	UIScript::UIOwnerDraws[ownerdraw] = callback;
}

bool UIScript::RunMenuScript(const char* name, const char** args)
{
	if (UIScript::UIScripts.find(name) != UIScript::UIScripts.end())
	{
		UIScript::UIScripts[name](UIScript::Token(args));
		return true;
	}

	return false;
}

//void UIScript::OwnerDrawHandleKeyStub(int ownerDraw, int flags, float* special, int key)
//{
//	if (key == 200 || key == 201) //mouse buttons
//	{
//		for (auto i = UIScript::UIOwnerDraws.begin(); i != UIScript::UIOwnerDraws.end(); ++i)
//		{
//			if (i->first == ownerDraw)
//			{
//				i->second();
//			}
//		}
//	}
//
//	Utils::Hook::Call<void(int, int, float*, int)>(0x4F58A0)(ownerDraw, flags, special, key);
//}

__declspec(naked) void UIScript::RunMenuScriptStub()
{
	__asm
	{
		/*mov eax, esp
		add eax, 4h*/
		//mov edx, [ eax + 5Ch ]   // UIScript name
		//mov eax, [ eax + 0Ch ] // UIScript args

		mov eax, esp
		add eax, 58h
		mov edx, eax           // UIScript name
		mov eax, [ebp + 08h] // UIScript args

		push eax
		push edx
		call UIScript::RunMenuScript
		add esp, 8h

		test al, al
		jz continue

		//// if returned
		pop     edi
		pop     esi
		pop     ebx
		mov     esp, ebp
		pop     ebp
		retn

		continue:
		mov eax, 54039Fh
			jmp eax
	}
}

void UIScript::post_load()
{
	// Install handler
	utils::hook::RedirectJump(0x5402C5, UIScript::RunMenuScriptStub);
}

void UIScript::pre_destroy()
{
	UIScript::UIScripts.clear();
	//UIScript::UIOwnerDraws.clear();
}

REGISTER_MODULE(UIScript);