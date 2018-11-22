
#pragma once

//! Forward declarations of the FSTRIPS namespace

namespace fs0 { namespace language { namespace fstrips {

class LogicalElement;

class Axiom;

// Formulas
class Formula;
class Tautology;
class Contradiction;
class AtomicFormula;
class Conjunction;
class Disjunction;
class Negation;
class QuantifiedFormula;
class ExistentiallyQuantifiedFormula;
class UniversallyQuantifiedFormula;
class ExternallyDefinedFormula;
class AxiomaticFormula;
class OpenFormula;
class AtomConjunction;
class RelationalFormula;
class EQAtomicFormula;
class NEQAtomicFormula;
class LTAtomicFormula;
class LEQAtomicFormula;
class GTAtomicFormula;
class GEQAtomicFormula;


// Terms
class Term;
class StateVariable;
class BoundVariable;
class Constant;
class NestedTerm;
class StaticHeadedNestedTerm;
class FluentHeadedNestedTerm;
class UserDefinedStaticTerm;
class AxiomaticTermWrapper;
class AxiomaticTerm;
class ArithmeticTerm;
class AdditionTerm;
class SubtractionTerm;
class MultiplicationTerm;
class DivisionTerm;
class PowerTerm;
class SqrtTerm;
class SineTerm;
class CosineTerm;
class TangentTerm;
class ArcSineTerm;
class ArcCosineTerm;
class ArcTangentTerm;
class ExpTerm;
class MinTerm;
class MaxTerm;
class AbsTerm;

// Other

class Metric;

} } } // namespaces

namespace fs = fs0::language::fstrips;
