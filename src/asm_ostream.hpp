#pragma once

#include <ostream>
#include <variant>
#include <functional>

#include "asm_syntax.hpp"
#include "asm_cfg.hpp"

inline pc_t label_to_pc(Label label) {
    try {
        return boost::lexical_cast<pc_t>(label);
    } catch(const boost::bad_lexical_cast &) {
        throw std::invalid_argument(std::string("Cannot convert ") + label + " to pc_t");
    }
}

using LabelTranslator = std::function<std::string(Label)>;

inline std::function<int16_t(Label)> label_to_offset(pc_t pc) {
    return [=](Label label) {
        return label_to_pc(label) - pc - 1;
    };
}

inline LabelTranslator label_to_offset_string(pc_t pc) {
    return [=](Label label) {
        int16_t target = label_to_offset(pc)(label);
        return std::string(target > 0 ? "+" : "") + std::to_string(target);
    };
}

void print(const InstructionSeq& prog, std::ostream& out);
void print(const InstructionSeq& insts, std::string outfile);
void print(const InstructionSeq& prog);

void print(const Cfg& cfg, bool nondet, std::ostream& out);
void print(const Cfg& cfg, bool nondet, std::string outfile);
void print(const Cfg& cfg, bool nondet);

void print_dot(const Cfg& cfg, std::ostream& out);
void print_dot(const Cfg& cfg, std::string outfile);
void print_dot(const Cfg& cfg);

std::ostream& operator<<(std::ostream& os, Instruction const& ins);
std::string to_string(Instruction const& ins);
std::string to_string(Instruction const& ins, LabelTranslator labeler);

std::ostream& operator<<(std::ostream& os, Bin::Op op);
std::ostream& operator<<(std::ostream& os, Condition::Op op);

inline std::ostream& operator<<(std::ostream& os, Imm imm) { return os << (int32_t)imm.v; }
inline std::ostream& operator<<(std::ostream& os, Reg const& a) { return os << "r" << (int)a.v; }
inline std::ostream& operator<<(std::ostream& os, Value const& a) { 
    if (std::holds_alternative<Imm>(a))
        return os << std::get<Imm>(a);
    return os << std::get<Reg>(a);
}

inline std::ostream& operator<<(std::ostream& os, Undefined const& a) { return os << (Instruction)a; }
inline std::ostream& operator<<(std::ostream& os, LoadMapFd const& a) { return os << (Instruction)a; }
inline std::ostream& operator<<(std::ostream& os, Bin const& a) { return os << (Instruction)a; }
inline std::ostream& operator<<(std::ostream& os, Un const& a) { return os << (Instruction)a; }
inline std::ostream& operator<<(std::ostream& os, Call const& a) { return os << (Instruction)a; }
inline std::ostream& operator<<(std::ostream& os, Exit const& a) { return os << (Instruction)a; }
inline std::ostream& operator<<(std::ostream& os, Jmp const& a) { return os << (Instruction)a; }
inline std::ostream& operator<<(std::ostream& os, Packet const& a) { return os << (Instruction)a; }
inline std::ostream& operator<<(std::ostream& os, Mem const& a) { return os << (Instruction)a; }
inline std::ostream& operator<<(std::ostream& os, LockAdd const& a) { return os << (Instruction)a; }
inline std::ostream& operator<<(std::ostream& os, Assume const& a) { return os << (Instruction)a; }
inline std::ostream& operator<<(std::ostream& os, Assert const& a) { return os << (Instruction)a; }
