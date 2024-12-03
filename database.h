#pragma once
#include <string>
#include <sstream>
#include <algorithm>
#include <map>


namespace db {
	class Database {
		std::map<std::size_t, std::string> table_A;
		std::map<std::size_t, std::string> table_B;
		std::string res_str;

		void insert(std::stringstream& ss) noexcept;
		void truncate(std::stringstream& ss) noexcept;
		void intersection() noexcept;
		void symmetric_difference() noexcept;
		void print(std::stringstream& ss) noexcept;

	public:
		Database() {}
		~Database() {}
		std::string query(std::string&& q) noexcept;
	};
}
