
#pragma once

#include <fs/core/constraints/direct/constraint.hxx>


namespace fs0 {

class ProblemInfo;
class UnaryDirectEffect;
class BinaryDirectEffect;
class GroundAction;
class DirectEffect;


/**
 * A UnaryDomainBoundsConstraint applies to a relevant variable X and checks that
 * the application of a certain action effect upon it produces a value for another variable Y
 * which does not violate the allowed bounds for Y.
 *
 * As an example, if X is of type int[0..10] and we have an action effect X := f(Y), we will want to
 * place a UnaryDomainBoundsConstraint upon Y which checks that 0 <= f(Y) <= 10.
 */
class UnaryDomainBoundsConstraint : public UnaryDirectConstraint {
protected:
	const ProblemInfo& _problemInfo;

	const UnaryDirectEffect* _effect;

public:
	UnaryDomainBoundsConstraint(const UnaryDirectEffect* effect, const ProblemInfo& problemInfo);

	virtual ~UnaryDomainBoundsConstraint() {};

	bool isSatisfied(const object_id& o) const override;
	
	DirectConstraint* compile(const ProblemInfo& problemInfo) const override;
	
	std::ostream& print(std::ostream& os) const override;
};


/**
 * A BinaryScopedEffect is like a UnaryDomainBoundsConstraint but works for arity-2 effects.
 *
 * An example would be an effect X := X - Y, where we will want to place a binary constraint
 * that makes sure that X-Y remains within the bounds of the variable X.
 */
class BinaryDomainBoundsConstraint : public BinaryDirectConstraint {
protected:
	const ProblemInfo& _problemInfo;

	const BinaryDirectEffect* _effect;

public:
	BinaryDomainBoundsConstraint(const BinaryDirectEffect* effect, const ProblemInfo& problemInfo);

	virtual ~BinaryDomainBoundsConstraint() {};

	bool isSatisfied(const object_id& o1, const object_id& o2) const override;
	
	DirectConstraint* compile(const ProblemInfo& problemInfo) const override;
	
	std::ostream& print(std::ostream& os) const override;
};

//! For every action, if any of its effects affects a bounded-domain variable, we want to place
//! an "automatic" "bounds constraint" upon the variables which are relevant to the effect,
//! so that they won't generate an out-of-bounds value.
class BoundsConstraintsGenerator {
public:
	//! Generates the necessary bound-constraints for the given effects (if any), and adds them to the given constraints vector
	static void generate(const GroundAction& action, const std::vector<const DirectEffect*>& effects, std::vector<DirectConstraint*>& constraints);
};

} // namespaces
