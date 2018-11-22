
#pragma once

#include <vector>
#include <utility>

#include <fs/core/utils/visitor.hxx>
#include <fs/core/languages/fstrips/language_fwd.hxx>

namespace fs0 { namespace language { namespace fstrips {


////////////////////////////////////////////////////////////
//! Recursively collects all nodes and subnodes in the subtree
//! that hangs from the given node, including itself.
//! Might include repeated nodes.
////////////////////////////////////////////////////////////
std::vector<const LogicalElement*> all_nodes(const LogicalElement& element);

class AllNodesVisitor
    : public Loki::BaseVisitor
    , public Loki::Visitor<Tautology, void, true>
    , public Loki::Visitor<Contradiction, void, true>
    , public Loki::Visitor<AtomicFormula, void, true>
    , public Loki::Visitor<AxiomaticFormula, void, true>
    , public Loki::Visitor<Conjunction, void, true>
    , public Loki::Visitor<Disjunction, void, true>
    , public Loki::Visitor<ExistentiallyQuantifiedFormula, void, true>
	, public Loki::Visitor<UniversallyQuantifiedFormula, void, true>

    , public Loki::Visitor<StateVariable, void, true>
    , public Loki::Visitor<BoundVariable, void, true>
    , public Loki::Visitor<Constant, void, true>
    , public Loki::Visitor<NestedTerm, void, true>
    , public Loki::Visitor<StaticHeadedNestedTerm, void, true>
    , public Loki::Visitor<FluentHeadedNestedTerm, void, true>
    , public Loki::Visitor<UserDefinedStaticTerm, void, true>
    , public Loki::Visitor<AxiomaticTermWrapper, void, true>
    , public Loki::Visitor<AxiomaticTerm, void, true>
    , public Loki::Visitor<AdditionTerm, void, true>
	, public Loki::Visitor<SubtractionTerm, void, true>
	, public Loki::Visitor<MultiplicationTerm, void, true>
    , public Loki::Visitor<DivisionTerm, void, true>
    , public Loki::Visitor<PowerTerm, void, true>
    , public Loki::Visitor<SqrtTerm, void, true>
    , public Loki::Visitor<SineTerm, void, true>
    , public Loki::Visitor<CosineTerm, void, true>
    , public Loki::Visitor<TangentTerm, void, true>
    , public Loki::Visitor<ArcSineTerm, void, true>
    , public Loki::Visitor<ArcCosineTerm, void, true>
    , public Loki::Visitor<ArcTangentTerm, void, true>
    , public Loki::Visitor<ExpTerm, void, true>
    , public Loki::Visitor<MinTerm, void, true>
    , public Loki::Visitor<MaxTerm, void, true>
    , public Loki::Visitor<AbsTerm, void, true>
{
public:
	void Visit(const Tautology& lhs);
	void Visit(const Contradiction& lhs);
	void Visit(const AtomicFormula& lhs);
	void Visit(const AxiomaticFormula& lhs);
	void Visit(const OpenFormula& lhs);
	void Visit(const Conjunction& lhs);
	void Visit(const Disjunction& lhs);
	void Visit(const QuantifiedFormula& lhs);
	void Visit(const ExistentiallyQuantifiedFormula& lhs);
	void Visit(const UniversallyQuantifiedFormula& lhs);

	void Visit(const StateVariable& lhs);
	void Visit(const BoundVariable& lhs);
	void Visit(const Constant& lhs);
	void Visit(const NestedTerm& lhs);
	void Visit(const StaticHeadedNestedTerm& lhs);
	void Visit(const FluentHeadedNestedTerm& lhs);
	void Visit(const UserDefinedStaticTerm& lhs);
	void Visit(const AxiomaticTermWrapper& lhs);
	void Visit(const AxiomaticTerm& lhs);
	void Visit(const AdditionTerm& lhs);
	void Visit(const SubtractionTerm& lhs);
	void Visit(const MultiplicationTerm& lhs);
    void Visit(const DivisionTerm& lhs);
	void Visit(const PowerTerm& lhs);
	void Visit(const SqrtTerm& lhs);
	void Visit(const SineTerm& lhs);
	void Visit(const CosineTerm& lhs);
	void Visit(const TangentTerm& lhs);
	void Visit(const ArcSineTerm& lhs);
	void Visit(const ArcCosineTerm& lhs);
	void Visit(const ArcTangentTerm& lhs);
	void Visit(const ExpTerm& lhs);
	void Visit(const MinTerm& lhs);
	void Visit(const MaxTerm& lhs);
    void Visit(const AbsTerm& lhs);

	std::vector<const LogicalElement*> _result;
};



////////////////////////////////////////////////////////////
//! Returns a vector with all the subformulae involved in the current formula
////////////////////////////////////////////////////////////
std::vector<const Formula*> all_formulae(const Formula& element);

////////////////////////////////////////////////////////////
//! A small helper - returns a vector with all the atomic formulae involved in the current formula
////////////////////////////////////////////////////////////
std::vector<const AtomicFormula*> all_atoms(const Formula& element);

////////////////////////////////////////////////////////////
//! A small helper - returns a vector with all the relational formulae involved in the current formula
////////////////////////////////////////////////////////////
std::vector<const RelationalFormula*> all_relations(const Formula& element);


////////////////////////////////////////////////////////////
//! Returns a list (possibly with repetitions) of all (sub-)terms that descend from the given formula or term.
////////////////////////////////////////////////////////////
std::vector<const Term*> all_terms(const LogicalElement& element);

////////////////////////////////////////////////////////////
//! Returns a list (possibly with repetitions) of all state variables that descend from the given formula or term.
////////////////////////////////////////////////////////////
std::vector<const StateVariable* > all_state_variables( const LogicalElement& element );


////////////////////////////////////////////////////////////
//! The level of nestedness of a given formula
////////////////////////////////////////////////////////////
unsigned nestedness(const LogicalElement& element);


class NestednessVisitor
    : public Loki::BaseVisitor
    , public Loki::Visitor<Tautology, void, true>
    , public Loki::Visitor<Contradiction, void, true>
    , public Loki::Visitor<AtomicFormula, void, true>
    , public Loki::Visitor<Disjunction, void, true>
    , public Loki::Visitor<Conjunction, void, true>
    , public Loki::Visitor<ExistentiallyQuantifiedFormula, void, true>
    , public Loki::Visitor<UniversallyQuantifiedFormula, void, true>

    , public Loki::Visitor<StateVariable, void, true>
    , public Loki::Visitor<BoundVariable, void, true>
    , public Loki::Visitor<Constant, void, true>
    , public Loki::Visitor<StaticHeadedNestedTerm, void, true>
    , public Loki::Visitor<FluentHeadedNestedTerm, void, true>
    , public Loki::Visitor<UserDefinedStaticTerm, void, true>
    , public Loki::Visitor<AxiomaticTermWrapper, void, true>
    , public Loki::Visitor<AxiomaticTerm, void, true>
    , public Loki::Visitor<AdditionTerm, void, true>
	, public Loki::Visitor<SubtractionTerm, void, true>
	, public Loki::Visitor<MultiplicationTerm, void, true>
{
public:
	void Visit(const Tautology& lhs) { _result = 0; }
	void Visit(const Contradiction& lhs) { _result = 0; }
	void Visit(const AtomicFormula& lhs);
	void Visit(const OpenFormula& lhs);
	void Visit(const Disjunction& lhs);
	void Visit(const Conjunction& lhs);
	void Visit(const ExistentiallyQuantifiedFormula& lhs);
	void Visit(const UniversallyQuantifiedFormula& lhs);

	void Visit(const StateVariable& lhs) { _result = 0; }
	void Visit(const BoundVariable& lhs) { _result = 1; }
	void Visit(const Constant& lhs) { _result = 0; }
	void Visit(const StaticHeadedNestedTerm& lhs);
	void Visit(const FluentHeadedNestedTerm& lhs);
	void Visit(const UserDefinedStaticTerm& lhs);
	void Visit(const AxiomaticTermWrapper& lhs);
	void Visit(const AxiomaticTerm& lhs);
	void Visit(const AdditionTerm& lhs);
	void Visit(const SubtractionTerm& lhs);
	void Visit(const MultiplicationTerm& lhs);


	unsigned _result;
};



////////////////////////////////////////////////////////////
//! Returns true if the element is flat, i.e. is a state variable or a constant
////////////////////////////////////////////////////////////
unsigned flat(const Term& element);

class FlatVisitor
    : public Loki::BaseVisitor
    , public Loki::Visitor<StateVariable, void, true>
    , public Loki::Visitor<BoundVariable, void, true>
    , public Loki::Visitor<Constant, void, true>
    , public Loki::Visitor<StaticHeadedNestedTerm, void, true>
    , public Loki::Visitor<FluentHeadedNestedTerm, void, true>
    , public Loki::Visitor<UserDefinedStaticTerm, void, true>
    , public Loki::Visitor<AxiomaticTermWrapper, void, true>
    , public Loki::Visitor<AxiomaticTerm, void, true>
    , public Loki::Visitor<AdditionTerm, void, true>
    , public Loki::Visitor<SubtractionTerm, void, true>
    , public Loki::Visitor<MultiplicationTerm, void, true>
{
public:
	void Visit(const StateVariable& lhs) { _result = true; }
	void Visit(const BoundVariable& lhs) { _result = true; }
	void Visit(const Constant& lhs) { _result = true; }
	void Visit(const StaticHeadedNestedTerm& lhs) { _result = false; }
	void Visit(const FluentHeadedNestedTerm& lhs) { _result = false; }
	void Visit(const UserDefinedStaticTerm& lhs);
	void Visit(const AxiomaticTermWrapper& lhs);
	void Visit(const AxiomaticTerm& lhs);
	void Visit(const AdditionTerm& lhs);
	void Visit(const SubtractionTerm& lhs);
	void Visit(const MultiplicationTerm& lhs);

	bool _result;
};



////////////////////////////////////////////////////////////
//! Returns the type of the term
////////////////////////////////////////////////////////////
unsigned type(const Term& element);

class TypeVisitor
    : public Loki::BaseVisitor
    , public Loki::Visitor<StateVariable, void, true>
    , public Loki::Visitor<BoundVariable, void, true>
    , public Loki::Visitor<Constant, void, true>
    , public Loki::Visitor<StaticHeadedNestedTerm, void, true>
    , public Loki::Visitor<FluentHeadedNestedTerm, void, true>
    , public Loki::Visitor<UserDefinedStaticTerm, void, true>
    , public Loki::Visitor<AxiomaticTermWrapper, void, true>
    , public Loki::Visitor<AxiomaticTerm, void, true>
    , public Loki::Visitor<AdditionTerm, void, true>
    , public Loki::Visitor<SubtractionTerm, void, true>
    , public Loki::Visitor<MultiplicationTerm, void, true>
{
public:
	void Visit(const StateVariable& lhs);
	void Visit(const BoundVariable& lhs);
	void Visit(const Constant& lhs) { throw std::runtime_error("Unimplemented"); }
	void Visit(const StaticHeadedNestedTerm& lhs);
	void Visit(const FluentHeadedNestedTerm& lhs);
	void Visit(const UserDefinedStaticTerm& lhs);
	void Visit(const AxiomaticTermWrapper& lhs);
	void Visit(const AxiomaticTerm& lhs);
	void Visit(const AdditionTerm& lhs);
	void Visit(const SubtractionTerm& lhs);
	void Visit(const MultiplicationTerm& lhs);


	//! The index of the type
	unsigned _result;
};



////////////////////////////////////////////////////////////
//! Returns the bounds of the term
////////////////////////////////////////////////////////////
std::pair<int, int> bounds(const Term& element);
std::pair<int, int> type_based_bounds(const Term& element);

class BoundVisitor
    : public Loki::BaseVisitor
    , public Loki::Visitor<StateVariable, void, true>
    , public Loki::Visitor<BoundVariable, void, true>
    , public Loki::Visitor<Constant, void, true>
    , public Loki::Visitor<StaticHeadedNestedTerm, void, true>
    , public Loki::Visitor<FluentHeadedNestedTerm, void, true>
    , public Loki::Visitor<UserDefinedStaticTerm, void, true>
    , public Loki::Visitor<AxiomaticTermWrapper, void, true>
    , public Loki::Visitor<AxiomaticTerm, void, true>
    , public Loki::Visitor<AdditionTerm, void, true>
    , public Loki::Visitor<SubtractionTerm, void, true>
    , public Loki::Visitor<MultiplicationTerm, void, true>
{
public:
	void Visit(const StateVariable& lhs);
	void Visit(const BoundVariable& lhs);
	void Visit(const Constant& lhs);
	void Visit(const StaticHeadedNestedTerm& lhs);
	void Visit(const FluentHeadedNestedTerm& lhs);
	void Visit(const UserDefinedStaticTerm& lhs);
	void Visit(const AxiomaticTermWrapper& lhs);
	void Visit(const AxiomaticTerm& lhs);
	void Visit(const AdditionTerm& lhs);
	void Visit(const SubtractionTerm& lhs);
	void Visit(const MultiplicationTerm& lhs);


	std::pair<int, int> _result;
};


} } } // namespaces
