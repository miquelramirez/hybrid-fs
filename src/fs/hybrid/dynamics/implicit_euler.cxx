#include <fs/hybrid/dynamics/implicit_euler.hxx>
#include <lapkt/tools/logging.hxx>
#include <fs/core/utils/config.hxx>

namespace fs0 { namespace dynamics { namespace integrators {

    unsigned ImplicitEuler::_max_iterations = 50;

    ImplicitEuler::ImplicitEuler() {
        tmp = nullptr;
    }

    void
    ImplicitEuler::operator()( const State& s, const std::vector<DifferentialEquation>& f_expr, State& next, double H, double factor ) const {
        const double base_duration = H / factor;

        if ( tmp == nullptr ) {
            tmp = std::make_shared<State>(next);
        }
        else {
            for ( VariableIdx x = 0; x < next.numAtoms(); x++ )
                tmp->__set( x, next.getValue(x));
        }

        std::vector<Atom>   f_un( f_expr.size(), Atom(INVALID_VARIABLE, object_id::INVALID));
        std::vector<Atom>   un( f_expr.size(), Atom(INVALID_VARIABLE, object_id::INVALID));

        while ( H > 0.0 ) {
            double h = std::min( base_duration, H  );
            evaluate_derivatives( next, f_expr, f_un );
            for ( unsigned i = 0; i < f_expr.size(); i++ ) {
                //! Euler method step
                //! u_{n+1} = u_{n} + h f(u_{n})
                float f_i = fs0::value<float>(next.getValue( f_expr[i]._affected ));
                float f_un_i = fs0::value<float>(f_un[i].getValue());
                float un1 = f_i + h * f_un_i;
                un[i] = Atom( f_expr[i]._affected, make_object(un1) );
            }

            //! un is the value of step u(n)
            unsigned    iterations = 0;
            float       max_error;

            do {
                max_error = 0.0;
                for ( unsigned i = 0; i < f_expr.size(); i++ ) {
                    tmp->__set( f_expr[i]._affected, un[i].getValue());
                }
                evaluate_derivatives( *tmp, f_expr, f_un );
                for ( unsigned i = 0; i < f_expr.size(); i++ ) {
                    //! Euler method step
                    //! u_{n+1} = u_{n} + h f(u_{n+1})
                    float f_i = fs0::value<float>(next.getValue( f_expr[i]._affected ));
                    float f_un_i = fs0::value<float>(f_un[i].getValue());
                    float un1 = (float)f_i + h * f_un_i;
                    float prev_un_i = fs0::value<float>(un[i].getValue());
                    max_error = std::max( max_error, std::fabs(un1 - prev_un_i ));
                    un[i] = Atom( f_expr[i]._affected, make_object(un1) );
                }
                iterations++;
            } while( (max_error > 1e-5) && (iterations < _max_iterations));
            for ( unsigned i = 0; i < f_expr.size(); i++ ) {
                next.__set( f_expr[i]._affected, un[i].getValue());
            }
            H -= h;
            LPT_DEBUG( "integrator", "Implicit Euler integration step");
            LPT_DEBUG( "integrator", "Iterations: " << iterations << " Final Delta: " << max_error  );
        }

    }

}}} // namespaces
