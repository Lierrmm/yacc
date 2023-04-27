#pragma once

class UIScript : public module
{
public:
	void post_load() override;
	void pre_destroy() override;

	class Token
	{
	public:
		Token() : token(nullptr) {};
		Token(const char** args) : token(nullptr) { this->parse(args); };
		Token(const Token& obj) { this->token = obj.token; };

		template<typename T> T get();
		bool isValid();

	private:
		char* token;

		void parse(const char** args);
	};

	typedef void(Callback)(Token token);
	typedef void(CallbackRaw)();

	static void Add(const std::string& name, utils::string::Slot<Callback> callback);
	static void AddOwnerDraw(int ownerdraw, utils::string::Slot<CallbackRaw> callback);

private:
	//static void OwnerDrawHandleKeyStub(int ownerDraw, int flags, float* special, int key);
	static bool RunMenuScript(const char* name, const char** args);
	static void RunMenuScriptStub();

	static std::unordered_map<std::string, utils::string::Slot<Callback>> UIScripts;
	static std::unordered_map<int, utils::string::Slot<CallbackRaw>> UIOwnerDraws;
};