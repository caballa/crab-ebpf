#pragma once

#include <crab/config.h>
#include <crab/common/types.hpp>
#include <crab/common/debug.hpp>
#include <crab/cfg/cfg.hpp>
#include <crab/cfg/cfg_bgl.hpp>
#include <crab/cg/cg.hpp>
#include <crab/cg/cg_bgl.hpp> 
#include <crab/cfg/var_factory.hpp>

#include <crab/domains/linear_constraints.hpp> 
#include <crab/domains/intervals.hpp>
#include <crab/domains/dis_intervals.hpp>
#include <crab/domains/wrapped_interval_domain.hpp>
#include <crab/domains/sparse_dbm.hpp>                      
#include <crab/domains/split_dbm.hpp>
#include <crab/domains/split_oct.hpp>
#include <crab/domains/boxes.hpp>
#ifdef APRON_DOMAINS
#include <crab/domains/apron_domains.hpp>
#endif
#ifdef ELINA_DOMAINS
#include <crab/domains/elina_domains.hpp>
#endif 
#include <crab/domains/term_equiv.hpp>
#include <crab/domains/array_expansion.hpp>
#include <crab/domains/array_adaptive.hpp>
#include <crab/domains/var_packing_domain.hpp>

//#define USE_ARRAY_ADAPTIVE

namespace crab {

  namespace cfg_impl {

    /// BEGIN MUST BE DEFINED BY CRAB CLIENT
    // A variable factory based on strings
    typedef cfg::var_factory_impl::str_variable_factory variable_factory_t;
    typedef typename variable_factory_t::varname_t varname_t;

    // CFG basic block labels
    typedef std::string basic_block_label_t;
    template<> inline std::string get_label_str(std::string e) 
    { return e; }
    /// END MUST BE DEFINED BY CRAB CLIENT    


    /// To define CFG over integers
    typedef cfg::cfg<basic_block_label_t, varname_t, ikos::z_number> z_cfg_t;
    typedef cfg::cfg_ref<z_cfg_t> z_cfg_ref_t;
    typedef cfg::cfg_rev<z_cfg_ref_t> z_cfg_rev_t;
    typedef z_cfg_t::basic_block_t z_basic_block_t;
    typedef ikos::variable<ikos::z_number, varname_t> z_var;
    typedef ikos::linear_expression<ikos::z_number, varname_t> z_lin_t;
    typedef ikos::linear_constraint<ikos::z_number, varname_t> z_lin_cst_t;
  }
  
  namespace cg_impl {
    /// To define CG over integers
    typedef cg::call_graph<cfg_impl::z_cfg_ref_t> z_cg_t;
    typedef cg::call_graph_ref<z_cg_t> z_cg_ref_t;
  }

  namespace domain_impl {
    
    using namespace crab::cfg_impl;
    using namespace crab::domains; 

    typedef pointer_constraint<ikos::variable<z_number, varname_t> > z_ptr_cst_t;
    typedef linear_constraint_system<ikos::z_number, varname_t> z_lin_cst_sys_t;
    typedef interval<ikos::z_number> z_interval_t;

    struct DBMGraphParams {
      enum { chrome_dijkstra = 1 };
      enum { widen_restabilize = 1 };
      enum { special_assign = 1 };
      enum { close_bounds_inline = 0 };
      typedef safe_i64 Wt;
      typedef AdaptGraph<Wt> graph_t;
    };
    
    // Numerical domains over integers
    using z_interval_domain_t = interval_domain<ikos::z_number,varname_t>;
    using z_ric_domain_t = numerical_congruence_domain<z_interval_domain_t>;
    using z_dbm_domain_t  = SparseDBM<ikos::z_number,varname_t,DBMGraphParams>;
    using z_sdbm_domain_t = SplitDBM<ikos::z_number,varname_t,DBMGraphParams>;
    using z_soct_domain_t = split_oct_domain<ikos::z_number,varname_t,DBMGraphParams>;
    using z_pack_sdbm_domain_t = var_packing_domain<z_sdbm_domain_t>;    
    using z_pack_soct_domain_t = var_packing_domain<z_soct_domain_t>;
    using z_boxes_domain_t = boxes_domain<ikos::z_number,varname_t>;
    using z_dis_interval_domain_t = dis_interval_domain<ikos::z_number, varname_t >;
#ifdef APRON_DOMAINS
    using z_oct_apron_domain_t = apron_domain<ikos::z_number,varname_t,apron_domain_id_t::APRON_OCT>;
    using z_pack_oct_apron_domain_t = var_packing_domain<z_oct_apron_domain_t>;        
    using z_pk_apron_domain_t = apron_domain<ikos::z_number,varname_t,apron_domain_id_t::APRON_PK>;
#endif
#ifdef ELINA_DOMAINS    
    using z_zones_elina_domain_t = elina_domain<ikos::z_number,varname_t,elina_domain_id_t::ELINA_ZONES>;
    using z_oct_elina_domain_t = elina_domain<ikos::z_number,varname_t,elina_domain_id_t::ELINA_OCT>;
    using z_pack_zones_elina_domain_t = var_packing_domain<z_zones_elina_domain_t>;    
    using z_pack_oct_elina_domain_t = var_packing_domain<z_oct_elina_domain_t>;
    using z_pk_elina_domain_t = elina_domain<ikos::z_number,varname_t,elina_domain_id_t::ELINA_PK>;
#endif     
    using z_term_domain_t = term_domain<term::TDomInfo<ikos::z_number,varname_t,z_interval_domain_t>>;
    using z_term_dbm_t = term_domain<term::TDomInfo<ikos::z_number,varname_t,z_sdbm_domain_t> >;
    using z_term_dis_int_t = term_domain<term::TDomInfo<ikos::z_number,varname_t,z_dis_interval_domain_t>>;
    using z_num_domain_t = reduced_numerical_domain_product2<z_term_dis_int_t,z_sdbm_domain_t,
							     reduced_product_impl::term_dbm_params>;
    using z_num_boxes_domain_t = reduced_numerical_domain_product2<z_boxes_domain_t,z_sdbm_domain_t>;
    // machine arithmetic domains
    using z_wrapped_interval_domain_t = wrapped_interval_domain<ikos::z_number, varname_t>;

    // Array domain
    #ifdef USE_ARRAY_ADAPTIVE
    template<typename Dom>
    using array_domain = array_adaptive_domain<Dom, array_adaptive_impl::NoSmashableParams>;
    #else
    template<typename Dom>
    using array_domain = array_expansion_domain<Dom>;    
    #endif 
  } 
}
