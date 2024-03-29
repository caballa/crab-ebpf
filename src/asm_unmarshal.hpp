#pragma once

#include <istream>
#include <variant>
#include <optional>
#include <vector>
#include <string>

#include "asm_syntax.hpp"
#include "spec_type_descriptors.hpp"

/** Translate a sequence of eBPF instructions (elf binary format) to a sequence
 *  of Instructions.
 * 
 *  \param raw_prog is the input program to parse.
 *  \param notes is where errors and warnings are written to.
 *  \return a sequence of instruction if successful, an error string otherwise.
 */
std::variant<InstructionSeq, std::string> unmarshal(raw_program raw_prog, std::vector<std::vector<std::string>>& notes);
std::variant<InstructionSeq, std::string> unmarshal(raw_program raw_prog);
