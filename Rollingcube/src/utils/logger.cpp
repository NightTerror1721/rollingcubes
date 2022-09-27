#include "logger.h"

#include <string>
#include <chrono>

#include "io_utils.h"


namespace logger::detail
{
	static std::streambuf* const default_out_buf = std::cout.rdbuf();
	static std::streambuf* const default_err_buf = std::cerr.rdbuf();
	static std::streambuf* const default_in_buf = std::cin.rdbuf();




	static void redirect_out(const std::string_view& newfile)
	{
		std::ofstream file = std::ofstream(std::string(newfile));
		if (file)
		{
			std::cout.rdbuf(file.rdbuf());
			std::freopen(newfile.data(), "w", stdout);
		}
	}

	static void redirect_err(const std::string_view& newfile)
	{
		std::ofstream file = std::ofstream(std::string(newfile));
		if (file)
		{
			std::cerr.rdbuf(file.rdbuf());
			std::freopen(newfile.data(), "w", stderr);
		}
	}

	static void redirect_in(const std::string_view& newfile)
	{
		std::ifstream file = std::ifstream(std::string(newfile));
		if (file)
		{
			std::cin.rdbuf(file.rdbuf());
			std::freopen(newfile.data(), "r", stderr);
		}
	}


	static inline std::ostream& buffer(level lv) { return lv >= level::warn ? std::cerr : std::cout; }

	static constexpr const char* level_name(level lv)
	{
		using enum level;
		switch (lv)
		{
			case debug: return "DEBUG";
			case info: return "INFO";
			case warn: return "WARN";
			case error: return "ERROR";
			case fatal: return "FATAL";
			default: return "UNKNOWN";
		}
	}

	static std::string get_time()
	{
		auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::tm* timemark = std::localtime(&time);
		if (timemark == nullptr)
			return {};
		
		std::stringstream ss;
		ss << std::put_time(timemark, "%c %z");
		return ss.str();
	}
}



namespace logger
{
	void log(level log_level, std::string_view msg)
	{
		if (log_level >= system_level)
		{

			detail::buffer(log_level)
				<< "[" << detail::level_name(log_level) << "]"
				<< "[" << detail::get_time() << "]"
				<< ": " << msg << std::endl;
		}
	}
}
