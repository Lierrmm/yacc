#pragma once
class stringtable final : public module
{
public:

	class AbstractFile
	{
	public:
		virtual ~AbstractFile() {};

		virtual bool exists() = 0;
		virtual std::string getName() = 0;
		virtual std::string& getBuffer() = 0;
	};

	class FFile : public AbstractFile
	{
	public:
		FFile() {};
		FFile(const std::string& file) : filePath(file) { this->read(); };

		bool exists() override { return !this->buffer.empty(); };
		std::string getName() override { return this->filePath; };
		std::string& getBuffer() override { return this->buffer; };

	private:
		std::string filePath;
		std::string buffer;

		void read();
	};

	void post_load() override;

private:
	static std::unordered_map<std::string, game::native::StringTable*> StringTableMap;

	static game::native::StringTable* LoadObject(std::string filename);
};