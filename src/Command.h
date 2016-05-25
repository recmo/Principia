#include <vector>
#include <string>
#include <cinttypes>
#include <functional>

class Command {
public:
	
	Command(
		const std::wstring& name,
		std::function<std::int32_t(const std::vector<std::wstring>&)> main,
		const std::wstring& description = std::wstring(),
		const std::wstring& help = std::wstring());
	
private:
	friend std::int32_t Main(const std::vector<std::wstring>& arguments);
	static std::int32_t main(const std::vector<std::wstring>& arguments);
	
	static std::vector<Command> _commands;
	const std::wstring _name;
	std::function<std::int32_t(const std::vector<std::wstring>&)> _main;
	const std::wstring _description;
	const std::wstring _help;
};
