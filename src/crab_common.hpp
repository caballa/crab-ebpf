#pragma once

#include <crab/support/debug.hpp>
#include <crab/types/varname_factory.hpp>
#include <crab/cfg/cfg.hpp>
#include <crab/cfg/cfg_bgl.hpp>
#include <crab/cg/cg.hpp>
#include <crab/cg/cg_bgl.hpp> 


namespace crab {
namespace cfg_impl {
    /// BEGIN MUST BE DEFINED BY CRAB CLIENT
    // A variable factory based on strings
    using variable_factory_t = var_factory_impl::str_variable_factory;
    using varname_t = typename variable_factory_t::varname_t;
    using basic_block_label_t = std::string;
    //template<> inline std::string get_label_str(std::string e) { return e; }
    /// END MUST BE DEFINED BY CRAB CLIENT    
}
} // end namespace crab

using crab::cfg_impl::varname_t;
using crab::cfg_impl::basic_block_label_t;
using crab::cfg_impl::variable_factory_t;

/// CFG over integers
using cfg_t         = crab::cfg::cfg<basic_block_label_t, varname_t, ikos::z_number>;
using basic_block_t = cfg_t::basic_block_t;
using lin_exp_t = ikos::linear_expression<ikos::z_number, varname_t>;


namespace crab{
template<>
class variable_name_traits<std::string> {
public:
  static std::string to_string(std::string varname) {
    return varname;
  }
};
  
template<>
class basic_block_traits<basic_block_t> {
public:
  static std::string to_string(const basic_block_label_t &bbl) {
    return bbl;
  }
};
} // end namespace crab
