#pragma once
#include <vector>
#include <string>
#include <cinttypes>
#include <functional>
#include <memory>

class Command {
public:
	typedef const std::vector<std::wstring>& Arguments;
	typedef std::int32_t exit_code;
	static constexpr exit_code success = EXIT_SUCCESS;
	static constexpr exit_code failure = EXIT_FAILURE;
	
	Command(
		const std::wstring& name,
		std::function<exit_code(Arguments)> main,
		const std::wstring& description = std::wstring(),
		const std::wstring& help = std::wstring());
	
private:
	friend exit_code Main(Arguments arguments);
	static exit_code main(Arguments arguments);
	static std::unique_ptr<std::vector<Command>> _commands;
	
	const std::wstring _name;
	std::function<exit_code(Arguments)> _main;
	const std::wstring _description;
	const std::wstring _help;
};
