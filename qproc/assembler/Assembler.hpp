#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include <cstdint>
#include <fstream>
#include <map>
#include <string>
#include <utility>
#include <vector>

class Assembler
{
public:
	Assembler(std::string source_path, std::string dest_path);
	~Assembler();
	
	void assemble();
	
private:
	template <class Key, class Value>
	bool is_present_in_map(const std::map<Key, Value> *m, const Key key);
	
	void reserve_space_for_label();
	void put_labels_in_reserved_spaces();
	void swap_endianness(int32_t* value);
	void error(std::string msg);
	void write_program();
	
	std::ifstream input;
	std::string dest_path;
	
	std::vector<uint8_t> program;
	
	const int32_t INITIAL_ADDRESS = -1;
	std::map<std::string, std::pair<std::vector<size_t>, int32_t>> labels;
	/*
	 * std::string - label's name;
	 * std::vector - vector of label usages' locations;
	 * int32_t - label declaration's address
	*/
};

#endif // ASSEMBLER_H
