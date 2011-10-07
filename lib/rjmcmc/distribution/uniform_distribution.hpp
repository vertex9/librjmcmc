#ifndef __UNIFORM_DISTRIBUTION_HPP__
#define __UNIFORM_DISTRIBUTION_HPP__

#include "rjmcmc/random.hpp"
#include <boost/random/uniform_smallint.hpp>
// boost::math::uniform is not used as it is linked to real values rather than discrete integral values

namespace rjmcmc {

    // 1_(n\in[a,b])/(b-a+1)
    class uniform_distribution {
    public:
        typedef double real_type;
        typedef int    int_type;
        typedef boost::uniform_smallint<int_type> rand_distribution_type;
        typedef boost::variate_generator<rjmcmc::mt19937_generator&,rand_distribution_type> variate_generator_type;

        uniform_distribution(int_type a, int_type b)
            : m_variate(rjmcmc::random(),rand_distribution_type(a,b))
            , m_pdf(real_type(1)/(b-a+1)) {}

        // new/old
        real_type pdf_ratio(int_type n0, int_type n1) const
        {
            assert(pdf(n0)>0);
            return (m_variate.distribution().min() <= n1 && n1 <= m_variate.distribution().max() );
        }

        real_type pdf(int_type n) const
        {
            return m_pdf * (m_variate.distribution().min() <= n && n <= m_variate.distribution().max() );
        }

        inline int_type operator()() const { return m_variate(); }

    private:
        mutable // because boost::poisson_distribution::operator()(Engine&) is non-const
        variate_generator_type m_variate;
        real_type m_pdf;
    };

}; // namespace rjmcmc

#endif // __UNIFORM_DISTRIBUTION_HPP__