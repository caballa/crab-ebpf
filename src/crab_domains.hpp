#pragma once

#include <crab/config.h>
#include <crab/domains/combined_congruences.hpp>
#include <crab/domains/intervals.hpp>
#include <crab/domains/dis_intervals.hpp>
#include <crab/domains/wrapped_interval_domain.hpp>
#include <crab/domains/sparse_dbm.hpp>                      
#include <crab/domains/split_dbm.hpp>
#include <crab/domains/split_oct.hpp>
#include <crab/domains/boxes.hpp>                      
#include <crab/domains/apron_domains.hpp>
#include <crab/domains/elina_domains.hpp>                
#include <crab/domains/term_equiv.hpp>
#include <crab/domains/generic_abstract_domain.hpp>
#include "array_expansion.hpp"
#include <crab/domains/array_adaptive.hpp>
#include <crab/support/debug.hpp>
#include <crab/types/varname_factory.hpp>
#include <crab/types/linear_constraints.hpp> 

#include "crab_common.hpp"

#define USE_ARRAY_ADAPTIVE

namespace crab {
namespace domain_impl {
using namespace crab::domains; 
  
struct DBMGraphParams {
  using Wt = safe_i64;
  using graph_t = AdaptGraph<Wt>;
};
  
// Numerical domains over integers
using z_interval_domain_t = ikos::interval_domain<ikos::z_number,varname_t>;
using z_ric_domain_t = numerical_congruence_domain<z_interval_domain_t>;
using z_dbm_domain_t  = sparse_dbm_domain<ikos::z_number,varname_t,DBMGraphParams>;
using z_sdbm_domain_t = split_dbm_domain<ikos::z_number,varname_t,DBMGraphParams>;
using z_soct_domain_t = split_oct_domain<ikos::z_number,varname_t>;  
using z_boxes_domain_t = boxes_domain<ikos::z_number,varname_t>;
using z_dis_interval_domain_t = dis_interval_domain<ikos::z_number, varname_t >;
using z_box_apron_domain_t = apron_domain<ikos::z_number,varname_t,apron_domain_id_t::APRON_INT>;
using z_oct_apron_domain_t = apron_domain<ikos::z_number,varname_t,apron_domain_id_t::APRON_OCT>;
using z_pk_apron_domain_t = apron_domain<ikos::z_number,varname_t,apron_domain_id_t::APRON_PK>;
using z_zones_elina_domain_t = elina_domain<ikos::z_number,varname_t,elina_domain_id_t::ELINA_ZONES>;
using z_oct_elina_domain_t = elina_domain<ikos::z_number,varname_t,elina_domain_id_t::ELINA_OCT>;
using z_pk_elina_domain_t = elina_domain<ikos::z_number,varname_t,elina_domain_id_t::ELINA_PK>;
using z_term_domain_t = term_domain<term::TDomInfo<ikos::z_number,varname_t,z_interval_domain_t>>;
using z_term_dbm_t = term_domain<term::TDomInfo<ikos::z_number,varname_t,z_sdbm_domain_t> >;
using z_term_dis_int_t = term_domain<term::TDomInfo<ikos::z_number,varname_t,z_dis_interval_domain_t>>;
using z_num_domain_t = reduced_numerical_domain_product2<z_term_dis_int_t,z_sdbm_domain_t,
							 reduced_product_impl::term_dbm_params>;
using z_num_boxes_domain_t = reduced_numerical_domain_product2<z_boxes_domain_t,z_sdbm_domain_t>;
using z_wrapped_interval_domain_t = wrapped_interval_domain<ikos::z_number, varname_t>;
  
// Array domain
#ifdef USE_ARRAY_ADAPTIVE
template<typename Dom>  
using array_domain = array_adaptive_domain<Dom>;
#else
/// Deprecated: used as baseline.
/// The boxes domain still needs it
template<typename Dom>
using array_domain = array_expansion_domain<Dom>;    
#endif 
} 
}
