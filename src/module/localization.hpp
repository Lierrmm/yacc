#pragma once

class localization final : public module
{
public:
	void post_load() override;
	void pre_destroy() override;

	static void Set(const std::string& key, const std::string& value);
	static const char* Get(const char* key);

	static void SetTemp(const std::string& key, const std::string& value);
	static void ClearTemp();

private:
	static std::recursive_mutex LocalizeMutex;
	static std::unordered_map<std::string, game::native::LocalizeEntry*> LocalizeMap;
	static std::unordered_map<std::string, game::native::LocalizeEntry*> TempLocalizeMap;
	static game::native::dvar_t* UseLocalization;

	static void __stdcall SetStringStub(const char* key, const char* value, bool isEnglish);
	static void LoadLanguageStrings(bool forceEnglish);
	static void SELoadLanguageStub();
};