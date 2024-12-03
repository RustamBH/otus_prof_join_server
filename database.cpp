#include "database.h"



void db::Database::insert(std::stringstream& ss) noexcept {
	std::string table;
	ss >> table;
	std::size_t id;
	ss >> id;
	std::string name;
	ss >> name;
	res_str = "OK\n";
	if ((table != "A" && table != "B") || ss.fail()) {
		res_str = "Error arguments INSERT\n";
	}
	if (table == "A") {
		if (!table_A.emplace(id, name).second) {
			res_str = "Error id duplicate\n";
		}
	}
	if (table == "B") {
		if (!table_B.emplace(id, name).second) {
			res_str = "Error id duplicate\n";
		}
	}
}


void db::Database::truncate(std::stringstream& ss) noexcept {
	std::string table;
	ss >> table;
	if (table == "A") {
		table_A.clear();
		res_str = "OK\n";
	}
	else if (table == "B") {
		table_B.clear();
		res_str = "OK\n";
	}
	else { res_str = "Error table name TRUNCATE\n"; }
}


void db::Database::intersection() noexcept {
	res_str.clear();
	auto it_a = table_A.begin();
	auto it_b = table_B.begin();
	std::size_t a_id;
	std::size_t b_id;
	while (it_a != table_A.end() && it_b != table_B.end()) {
		std::size_t a_id = it_a->first;
		std::size_t b_id = it_b->first;
		if (a_id > b_id) {
			++it_b;
		}
		else if (a_id < b_id) {
			++it_a;
		}
		else {
			res_str += std::to_string(a_id) + "," + it_a->second + "," + it_b->second + "\n";
			++it_a;
			++it_b;
		}
	}
	res_str += "OK\n";
}


void db::Database::symmetric_difference() noexcept {
	res_str.clear();
	auto it_a = table_A.begin();
	auto it_b = table_B.begin();
	std::size_t a_id = it_a->first;
	std::size_t b_id = it_b->first;
	while (it_a != table_A.end() && it_b != table_B.end()) {
		a_id = it_a->first;
		b_id = it_b->first;
		if (b_id < a_id) {
			res_str += std::to_string(b_id) + "," + "," + it_b->second + "\n";
			++it_b;
		}
		else if (a_id < b_id) {
			res_str += std::to_string(a_id) + "," + it_a->second + "," + "\n";
			++it_a;
		}
		else {
			++it_a;
			++it_b;
		}
	}

	if (it_a != table_A.end()) {
		while (it_a != table_A.end()) {
			a_id = it_a->first;
			res_str += std::to_string(a_id) + "," + it_a->second + "," + "\n";
			++it_a;
		}
	}
	else if (it_b != table_B.end()) {
		while (it_b != table_B.end()) {
			b_id = it_b->first;
			res_str += std::to_string(b_id) + "," + "," + it_b->second + "\n";
			++it_b;
		}
	}
	else {}
	res_str += "OK\n";
}

std::string db::Database::query(std::string&& q) noexcept {
	std::stringstream ss(q);
	std::string command;
	ss >> command;
	if (command == "INSERT") { insert(ss); }
	else if (command == "TRUNCATE") { truncate(ss); }
	else if (command == "PRINT") { print(ss); } // show table data
	else if (command == "INTERSECTION") { intersection(); }
	else if (command == "SYMMETRIC_DIFFERENCE") { symmetric_difference(); }
	else { res_str = "ERR_COMMAND!TRY_AGAIN!\n"; }
	return std::move(res_str);
}

void db::Database::print(std::stringstream& ss) noexcept {
	res_str.clear();
	std::string table;
	ss >> table;
	if (table == "A") {
		std::for_each(table_A.begin(), table_A.end(), [this](auto it) {
			res_str += std::to_string(it.first) + "," + it.second + "\n";
			});
		res_str += "OK\n";
	}
	else if (table == "B") {
		std::for_each(table_B.begin(), table_B.end(), [this](auto it) {
			res_str += std::to_string(it.first) + "," + it.second + "\n";
			});
		res_str += "OK\n";
	}
}
