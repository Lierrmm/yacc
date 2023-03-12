#pragma once
#include <utils/nt.hpp>
#include <launcher/launcher.hpp>

class loader final
{
public:
	explicit loader(launcher::mode mode);

	FARPROC load(const utils::nt::library& module, const std::string& buffer) const;

	void set_import_resolver(const std::function<void*(const std::string&, const std::string&)>& resolver);

	FARPROC load_library(const std::string& filename) const;

private:
	launcher::mode mode_;
	std::function<void*(const std::string&, const std::string&)> import_resolver_;

	static void load_section(const utils::nt::library& target, const utils::nt::library& source,
	                         IMAGE_SECTION_HEADER* section);
	void load_sections(const utils::nt::library& target, const utils::nt::library& source) const;
	void load_imports(const utils::nt::library& target, const utils::nt::library& source) const;
	void load_tls(const utils::nt::library& module, const utils::nt::library& source) const;
};
