/*********************                                                        */
/*! \file sygus_grammar_cons.h
 ** \verbatim
 ** Top contributors (to current version):
 **   Andrew Reynolds
 ** This file is part of the CVC4 project.
 ** Copyright (c) 2009-2017 by the authors listed in the file AUTHORS
 ** in the top-level source directory) and their institutional affiliations.
 ** All rights reserved.  See the file COPYING in the top-level source
 ** directory for licensing information.\endverbatim
 **
 ** \brief class for constructing inductive datatypes that correspond to
 ** grammars that encode syntactic restrictions for SyGuS.
 **/

#include "cvc4_private.h"

#ifndef __CVC4__THEORY__QUANTIFIERS__SYGUS_GRAMMAR_CONS_H
#define __CVC4__THEORY__QUANTIFIERS__SYGUS_GRAMMAR_CONS_H

#include "theory/quantifiers_engine.h"

namespace CVC4 {
namespace theory {
namespace quantifiers {

/** utility for constructing datatypes that correspond to syntactic restrictions,
* and applying the deep embedding from Section 4 of Reynolds et al CAV 2015.
*/
class CegGrammarConstructor
{
public:
  CegGrammarConstructor( QuantifiersEngine * qe );
  ~CegGrammarConstructor(){}
  /** convert node n based on deep embedding (Section 4 of Reynolds et al CAV 2015) */
  Node process( Node q, std::map< Node, Node >& templates, std::map< Node, Node >& templates_arg );
  /** is the syntax restricted? */
  bool isSyntaxRestricted() { return d_is_syntax_restricted; }
  /** does the syntax allow ITE expressions? */
  bool hasSyntaxITE() { return d_has_ite; }
  // make the default sygus datatype type corresponding to builtin type range
  //   bvl is the set of free variables to include in the grammar
  //   fun is for naming
  //   extra_cons is a set of extra constant symbols to include in the grammar
  static TypeNode mkSygusDefaultType( TypeNode range, Node bvl, const std::string& fun, std::map< TypeNode, std::vector< Node > >& extra_cons );
  // make the default sygus datatype type corresponding to builtin type range
  static TypeNode mkSygusDefaultType( TypeNode range, Node bvl, const std::string& fun ){
    std::map< TypeNode, std::vector< Node > > extra_cons;
    return mkSygusDefaultType( range, bvl, fun, extra_cons );
  }
  // make the sygus datatype type that encodes the solution space (lambda templ_arg. templ[templ_arg]) where templ_arg
  // has syntactic restrictions encoded by sygus type templ_arg_sygus_type
  //   bvl is the set of free variables to include in the frammar
  //   fun is for naming
  static TypeNode mkSygusTemplateType( Node templ, Node templ_arg, TypeNode templ_arg_sygus_type, Node bvl, const std::string& fun );
private:
  /** reference to quantifier engine */
  QuantifiersEngine * d_qe;
  /** is the syntax restricted? */
  bool d_is_syntax_restricted;
  /** does the syntax allow ITE expressions? */
  bool d_has_ite;
  /** collect terms */
  void collectTerms( Node n, std::map< TypeNode, std::vector< Node > >& consts );
  /** convert node n based on deep embedding (Section 4 of Reynolds et al CAV 2015) */
  Node convertToEmbedding( Node n, std::map< Node, Node >& synth_fun_vars );
  //---------------- grammar construction
  // helper for mkSygusDefaultGrammar (makes unresolved type for mutually recursive datatype construction)
  static TypeNode mkUnresolvedType(const std::string& name, std::set<Type>& unres);
  // make the builtin constants for type type that should be included in a sygus grammar
  static void mkSygusConstantsForType( TypeNode type, std::vector<CVC4::Node>& ops );
  // collect the list of types that depend on type range
  static void collectSygusGrammarTypesFor( TypeNode range, std::vector< TypeNode >& types, std::map< TypeNode, std::vector< DatatypeConstructorArg > >& sels );
  // helper function for function mkSygusDefaultGrammar
  //   collects a set of mutually recursive datatypes "datatypes" corresponding to encoding type "range" to SyGuS
  //   unres is used for the resulting call to mkMutualDatatypeTypes
  static void mkSygusDefaultGrammar( TypeNode range, Node bvl, const std::string& fun, std::map< TypeNode, std::vector< Node > >& extra_cons, 
                                     std::vector< CVC4::Datatype >& datatypes, std::set<Type>& unres );
  // helper function for mkSygusTemplateType
  static TypeNode mkSygusTemplateTypeRec( Node templ, Node templ_arg, TypeNode templ_arg_sygus_type, Node bvl, 
                                          const std::string& fun, unsigned& tcount );
  //---------------- end grammar construction
};

} /* namespace CVC4::theory::quantifiers */
} /* namespace CVC4::theory */
} /* namespace CVC4 */

#endif
