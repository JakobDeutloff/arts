#include <numeric>

#include <Faddeeva/Faddeeva.hh>

#include "lin_alg.h"
#include "linefunctions.h"
#include "linemixing.h"
#include "minimize.h"
#include "physics_funcs.h"


namespace Absorption::LineMixing {
EquivalentLines::EquivalentLines(const ComplexMatrix& W,
                                 const Vector& pop,
                                 const Vector& dip) noexcept :
  val(pop.nelem(), 0), str(pop.nelem(), 0) {
  const Index n = pop.nelem();
  
  // Compute values
  ComplexMatrix V(n, n);
  
  // Main computations
  diagonalize(V, val, W);
  
  // Do the matrix forward multiplication
  for (Index i=0; i<n; i++) {
    for (Index j=0; j<n; j++) {
      str[i] += dip[j] * V(j, i);
    }
  }
  
  // Do the matrix backward multiplication
  auto& invV=V.inv();
  for (Index i=0; i<n; i++) {
    Complex z(0, 0);
    for (Index j=0; j<n; j++) {
      z += pop[j] * dip[j] * invV(i, j);
    }
    str[i] *= z;
  }
}


PopulationAndDipole::PopulationAndDipole(const Numeric T,
                                         const AbsorptionLines& band,
                                         const SpeciesAuxData::AuxType& partition_type,
                                         const ArrayOfGriddedField1& partition_data) noexcept :
  pop(band.NumLines()), dip(band.NumLines()) {
  const Index N = band.NumLines();
  
  const Numeric QT = single_partition_function(T, partition_type, partition_data);
  const Numeric QT0 = single_partition_function(band.T0(), partition_type, partition_data);
  const Numeric ratiopart = QT0 / QT;
  
  for (Index i=0; i<N; i++) {
    const Numeric pop0 = (band.g_upp(i) / QT0) * boltzman_factor(band.T0(), band.E0(i));
    pop[i] = pop0 * ratiopart * boltzman_ratio(T, band.T0(), band.E0(i));
    dip[i] = std::sqrt(band.I0(i)/(pop0 * band.F0(i) * (1-stimulated_emission(band.T0(), band.F0(i)))));
  }
}


ArrayOfIndex PopulationAndDipole::sort(const AbsorptionLines& band) noexcept {
  const Index N = pop.nelem();
  
  // List that starts as [0, 1, ... N-2, N-1]
  ArrayOfIndex out(N);
  std::iota(out.begin(), out.end(), 0);
  
  // Strength
  Vector s(N);
  for (Index i=0; i<N; i++) {
    s[i] = band.F0(i) * pop[i] * Constant::pow2(dip[i]);
  }
  
  for (Index i=0; i<N; i++) {
    for (Index j=i+1; j<N; j++) {
      if (s[j] > s[i]) {
        std::swap(out[i], out[j]);
        std::swap(dip[i], dip[j]);
        std::swap(pop[i], pop[j]);
        std::swap(s[i], s[j]);
      }
    }
  }
  
  return out;
}


void PopulationAndDipole::sort(const ArrayOfIndex& presorting) noexcept {
  const Index N = presorting.size();
  Vector dipcopy(dip), popcopy(pop);
  for (Index i=0; i<N; i++) {
    dip[i] = dipcopy[presorting[i]];
    pop[i] = popcopy[presorting[i]];
  }
}

namespace Makarov2020etal {
/*! Compute the rotational energy of ground-state O2
 * 
 * If the template argument evaluates true, the erot<false>(1, 0)
 * energy is removed from the output of erot<false>(N, J).
 * 
 * @param[in] N Main rotational number
 * @param[in] J Main rotational number plus spin
 * @return Rotational energy in Joule
 */
template<bool rescale_pure_rotational=true> constexpr
Numeric erot(const Rational N, const Rational j=-1) {
  const Rational J = j<0 ? N : j;
  
  if constexpr (rescale_pure_rotational) {
    return erot<false>(N, J) - erot<false>(1, 0);
  } else {
    using Conversion::mhz2joule;
    using Constant::pow2;
    using Constant::pow3;
    
    constexpr Numeric B0=43100.4425e0;
    constexpr Numeric D0=.145123e0;
    constexpr Numeric H0=3.8e-08;
    constexpr Numeric xl0=59501.3435e0;
    constexpr Numeric xg0=-252.58633e0;
    constexpr Numeric xl1=0.058369e0;
    constexpr Numeric xl2=2.899e-07;
    constexpr Numeric xg1=-2.4344e-04;
    constexpr Numeric xg2=-1.45e-09;
    
    const Numeric XN = Numeric(N);
    const Numeric XX = XN * (XN + 1);
    const Numeric xlambda=xl0+xl1*XX+xl2*pow2(XX);
    const Numeric xgama=xg0+xg1*XX+xg2*pow2(XX);
    const Numeric C1=B0*XX-D0*pow2(XX)+H0*pow3(XX);
    
    if (J < N) {
      if (N == 1) {  // erot<false>(1, 0)
        return mhz2joule(C1 - (xlambda+B0*(2.*XN-1.)+xgama*XN));
      } else {
        return mhz2joule(C1 - (xlambda+B0*(2.*XN-1.)+xgama*XN) + std::sqrt(pow2(B0*(2.*XN-1.))+pow2(xlambda)-2.*B0*xlambda));
      }
    } else if (J > N) {
      return mhz2joule(C1 - (xlambda-B0*(2.*XN+3.)-xgama*(XN+1.)) - std::sqrt(pow2(B0*(2.*XN+3.))+pow2(xlambda)-2.*B0*xlambda));
    } else {
      return mhz2joule(C1);
    }
  }
}


/*! Returns the adiabatic factor
 * 
 * @param[in] N Main rotational number
 * @param[in] T The temperature
 * @param[in] m1 Mass of one species in atomic mass units
 * @param[in] m2 Mass of the other species in atomic mass units
 * @return The adiabatic factor
 */
constexpr Numeric Omega(const Rational N,
                        const Numeric T,
                        const Numeric m1,
                        const Numeric m2) {
  using Constant::h;
  using Constant::k;
  using Constant::pi;
  using Constant::h_bar;
  using Constant::pow2;
  
  // Constant from Makarov etal 2020
  constexpr Numeric dc = Conversion::angstrom2meter(0.61);
  
  // Constants for the expression
  constexpr Numeric fac = 8 * k / (Constant::m_u * pi);
  
  // nb. Only N=J considered???
  const Numeric en = erot(N);
  const Numeric enm2 = erot(N-2);
  const Numeric wnnm2 = (en - enm2) / h_bar;
  
  const Numeric mu = 1 / m1 + 1 / m2;
  const Numeric v_bar_pow2 = fac*T*mu;
  const Numeric tauc_pow2 = pow2(dc) / v_bar_pow2;
  
  return 1.0 / pow2(1 + 1.0/24.0 * pow2(wnnm2) * tauc_pow2);
};


/*! Returns the basis rate
 * 
 * @param[in] N Main rotational number
 * @param[in] T The temperature
 * @return The basis rate
 */
Numeric Q(const Rational N, const Numeric T) {
  using Conversion::kelvin2joule;
  
  // Constants from Makarov etal 2020
  constexpr Numeric lambda =  0.39;
  constexpr Numeric beta = 0.567;
  
  // nb. Only N=J considered???
  return Numeric(2*N + 1) / pow(N * (N+1), lambda) * std::exp(-beta * erot(N) / kelvin2joule(T));
};


/*! Returns the reduced dipole
 * 
 * @param[in] Ju Main rotational number with spin of the upper level
 * @param[in] Jl Main rotational number with spin of the lower level
 * @param[in] N Main rotational number of both levels
 * @return The reduced dipole
 */
Numeric zero_dipole(const Rational Ju, const Rational Jl, const Rational N) {
  return (iseven(Jl + N) ? 1 : -1) * sqrt(6 * (2*Jl + 1) * (2*Ju + 1)) * wigner6j(1, 1, 1, Ju, Jl, N);
};

/*! Computes the off-diagonal elements of the relaxation matrix
 * following Makarov etal 2020.
 * 
 * @param[in,out] W The imaginary part of the relaxation matrix
 * @param[in] T The temperature
 * @param[in] band The absorption band
 * @param[in] sorting The sorting of the band
 * @param[in] mass_self The mass of the self-species
 * @param[in] mass_other The mass of the colliding species
 */
void relaxation_matrix_offdiagonal(MatrixView W,
                                   const Numeric T,
                                   const AbsorptionLines& band,
                                   const ArrayOfIndex& sorting,
                                   const Numeric& mass_self,
                                   const Numeric& mass_other)
{
  using Conversion::kelvin2joule;
  
  const Index n = band.NumLines();
  
  Vector dip0(n);
  for (Index i=0; i<n; i++) {
    dip0[i] = std::abs(zero_dipole(band.UpperQuantumNumber(sorting[i], QuantumNumberType::J),
                                   band.LowerQuantumNumber(sorting[i], QuantumNumberType::J),
                                   band.UpperQuantumNumber(sorting[i], QuantumNumberType::N)));
    
    for (Index j=0; j<n; j++) {
      if (i == j) continue;
      
      // Select upper quantum number
      const bool ihigh = band.E0(sorting[i]) > band.E0(sorting[j]);
      const Index k = ihigh ? i : j;
      const Index l = ihigh ? j : i;
      
      // Quantum numbers
      const Rational Jk = band.UpperQuantumNumber(sorting[k], QuantumNumberType::J);
      const Rational Jl = band.UpperQuantumNumber(sorting[l], QuantumNumberType::J);
      const Rational Nk = band.UpperQuantumNumber(sorting[k], QuantumNumberType::N);
      const Rational Nl = band.UpperQuantumNumber(sorting[l], QuantumNumberType::N);
      const Rational Jk_p = band.LowerQuantumNumber(sorting[k], QuantumNumberType::J);
      const Rational Jl_p = band.LowerQuantumNumber(sorting[l], QuantumNumberType::J);
      
      // Makarov 2013 symbol with modifications:
      //    1) Squared scale
      //    2) Squared 3J-symbol
      //    3) Using the updated 2020 constants
      // These are modified after reading the original code
      Numeric sum = 0;
      const Numeric scale = Numeric((2*Nk + 1) * (2*Nl + 1)) * sqrt((2*Jk + 1) * (2*Jl + 1) * (2*Jk_p + 1) * (2* Jl_p + 1));
      const auto [L0, L1] = wigner_limits(wigner3j_limits<3>(Nl, Nk), {Rational(2), 100000});
      for (Rational L=L0; L<L1; L+=2) {
        const Numeric sgn = iseven(Jk + Jl + L + 1) ? 1 : -1;
        const Numeric a = Constant::pow2(wigner3j(Nl, Nk, L, 0, 0, 0));
        const Numeric b = wigner6j(L, Jk, Jl, 1, Nl, Nk);
        const Numeric c = wigner6j(L, Jk_p, Jl_p, 1, Nl, Nk);
        const Numeric d = wigner6j(L, Jk, Jl, 1, Jl_p, Jk_p);
        sum += sgn * a * b * c * d * Q(L, T) / Omega(L, T, mass_self, mass_other);
      }
      sum *= scale * Omega(Nk, T, mass_self, mass_other);
      
      // Add to W and rescale to upwards element by the populations
      W(l, k) = sum;
      W(k, l) = sum * std::exp((erot(Nl, Jl) - erot(Nk, Jk)) / Conversion::kelvin2joule(T));
    }
  }
  
  // Transpose?  Why is this transpose required?
  for (Index i=0; i<n; i++) {
    for (Index j=0; j<i; j++) {
      swap(W(i, j), W(j, i));
    }
  }
  
  // Sum rule correction
  for (Index i=0; i<n; i++) {
    Numeric sumlw = 0.0;
    Numeric sumup = 0.0;
    
    for (Index j=0; j<n; j++) {
      if (j > i) {
        sumlw += std::abs(dip0[j]) * W(j, i);
      } else {
        sumup += std::abs(dip0[j]) * W(j, i);
      }
    }
    
    const Rational Ji = band.UpperQuantumNumber(sorting[i], QuantumNumberType::J);
    const Rational Ni = band.UpperQuantumNumber(sorting[i], QuantumNumberType::N);
    for (Index j=i+1; j<n; j++) {
      const Rational Jj = band.UpperQuantumNumber(sorting[j], QuantumNumberType::J);
      const Rational Nj = band.UpperQuantumNumber(sorting[j], QuantumNumberType::N);
      if (sumlw == 0) {
        W(j, i) = 0.0;
        W(i, j) = 0.0;
      } else {
        W(j, i) *= - sumup / sumlw;
        W(i, j) = W(j, i) * std::exp((erot(Ni, Ji) - erot(Nj, Jj)) / Conversion::kelvin2joule(T));
      }
    }
  }
}
}  // Makarov2020etal


/*! Computes the Error Corrected Sudden relaxation matrix for a single species
 * 
 * The band population type is used to select type of off-diagonal element computations
 * 
 * @param[in] band The absorption band
 * @param[in] sorting The sorting of the band
 * @param[in] T The temperature
 * @param[in] P The pressure
 * @param[in] species_mass The mass of the colliding species
 * @param[in] species_pos The index position of the colliding species among the band broadeners
 * @return The single species relaxation matrix
 */
ComplexMatrix single_species_ecs_relaxation_matrix(const AbsorptionLines& band,
                                                   const ArrayOfIndex& sorting,
                                                   const Numeric T,
                                                   const Numeric P,
                                                   const Numeric species_mass,
                                                   const Index species_pos) {
  const Index N = band.NumLines();
  
  // Allocate the matrix
  ComplexMatrix W(N, N, 0);
  
  // Fill diagonal keeping track of the reshuffle (sorting)
  for (Index I=0; I<N; I++) {
    const Index i = sorting[I];
    auto shape = band.ShapeParameters(i, T, P, species_pos);
    W(I, I) = Complex(shape.D0, shape.G0);  // nb. This must be fixed if SDVP ???
  }
  
  // Set the off-diagonal part of the matrix for this broadener
  switch (band.Population()) {
    case PopulationType::ByMakarovFullRelmat:
      Makarov2020etal::relaxation_matrix_offdiagonal(W.imag(), T, band, sorting, band.SpeciesMass(), species_mass);
      break;
    default:
      throw std::runtime_error("Bad type [developer error: do not reach here]");
      break;
  }
  
  return W;
}


/*! Computes the Error Corrected Sudden relaxation matrix
 * 
 * @param[in] T The temperature
 * @param[in] P The pressure
 * @param[in] vmrs The VMRs of all broadeners of the absorption band
 * @param[in] mass The mass of all broadeners of the absorption band
 * @param[in] band The absorption band
 * @param[in] sorting The sorting of the band
 * @param[in] frenorm The renormalization of frequency
 * @return The relaxation matrix
 */
ComplexMatrix ecs_relaxation_matrix(const Numeric T,
                                    const Numeric P,
                                    const Vector& vmrs,
                                    const Vector& mass,
                                    const AbsorptionLines& band,
                                    const ArrayOfIndex& sorting,
                                    const Numeric frenorm) {
  const Index N = band.NumLines();
  const Index M = vmrs.nelem();
  
  // Create output
  ComplexMatrix W(N, N, 0);
  
  // Loop over all the broadeners
  for (Index k=0; k<M; k++) {
    // Create temporary
    const ComplexMatrix Wtmp = single_species_ecs_relaxation_matrix(band, sorting, T, P, mass[k], k);
    
    // Sum up all atmospheric components
    for (Index i=0; i<N; i++) {
      for (Index j=0; j<N; j++) {
        W(i, j) += vmrs[k] * Wtmp(i, j);
      }
    }
  }
  
  // Deal with line frequency and its re-normalization
  for (Index i=0; i<N; i++) {
    W(i, i) += band.F0(sorting[i]) - frenorm;
  }
  
  return W;
}


/*! Returns sorted population distribtions and dipoles and the original sorting
 * 
 * @param[in] T The temperature
 * @param[in] band The absorption band
 * @param[in] partition_type The type of partition function data
 * @param[in] partition_data The partition function data
 * @return {sorting, sorted population distribtions and dipoles}
 */
std::pair<ArrayOfIndex, PopulationAndDipole> sorted_population_and_dipole(const Numeric T,
                                                                          const AbsorptionLines& band,
                                                                          const SpeciesAuxData::AuxType& partition_type,
                                                                          const ArrayOfGriddedField1& partition_data) {
  PopulationAndDipole tp(T, band, partition_type, partition_data);
  return {tp.sort(band), tp};
}


/*! Returns pre-sorted population distribtions and dipoles
 * 
 * @param[in] T The temperature
 * @param[in] presorting Changes positions from [0 ... N-1] to presorting positions
 * @param[in] band The absorption band
 * @param[in] partition_type The type of partition function data
 * @param[in] partition_data The partition function data
 * @return Pre-sorted  population distribtions and dipoles
 */
PopulationAndDipole presorted_population_and_dipole(const Numeric T,
                                                    const ArrayOfIndex& presorting,
                                                    const AbsorptionLines& band,
                                                    const SpeciesAuxData::AuxType& partition_type,
                                                    const ArrayOfGriddedField1& partition_data) {
  PopulationAndDipole tp(T, band, partition_type, partition_data);
  tp.sort(presorting);
  return tp;
}


ComplexVector ecs_absorption(const Numeric T,
                             const Numeric P,
                             const Numeric this_vmr,
                             const Vector& vmrs,
                             const Vector& mass,
                             const Vector& f_grid,
                             const AbsorptionLines& band,
                             const SpeciesAuxData::AuxType& partition_type,
                             const ArrayOfGriddedField1& partition_data) {
  constexpr Numeric sq_ln2pi = Constant::sqrt_ln_2 / Constant::sqrt_pi;
  
  // Weighted center of the band
  const Numeric frenorm = band.F_mean();
  
  // Band Doppler broadening constant
  const Numeric GD_div_F0 = Linefunctions::DopplerConstant(T, band.SpeciesMass());
  
  // Sorted population
  const auto [sorting, tp] = sorted_population_and_dipole(T, band, partition_type, partition_data);
  
  // Relaxation matrix
  const ComplexMatrix W = ecs_relaxation_matrix(T, P, vmrs, mass, band, sorting, frenorm);
  
  // Equivalent lines computations
  const EquivalentLines eqv(W, tp.pop, tp.dip);
  
  // Absorption of this band
  ComplexVector absorption(f_grid.nelem(), 0);
  for (Index i=0; i<band.NumLines(); i++) {
    const Numeric gamd = GD_div_F0 * (eqv.val[i].real() + frenorm);
    const Numeric cte = Constant::sqrt_ln_2 / gamd;
    for (Index iv=0; iv<f_grid.nelem(); iv++) {
      const Complex z = (eqv.val[i] + frenorm - f_grid[iv]) * cte;
      const Complex w = Faddeeva::w(z);
      absorption[iv] += eqv.str[i] * w / gamd;
    }
  }
  
  // Adjust by frequency and number density
  const Numeric numdens = this_vmr * number_density(P, T);
  for (Index iv=0; iv<f_grid.nelem(); iv++) {
    const Numeric f = f_grid[iv];
    const Numeric fact = f * (1 - stimulated_emission(T, f));
    absorption[iv] *= fact * numdens * sq_ln2pi;
  }
  
  return absorption;
}


ComplexVector ecs_absorption_with_zeeman_perturbations(const Numeric T,
                                                       const Numeric H,
                                                       const Numeric P,
                                                       const Numeric this_vmr,
                                                       const Vector& vmrs,
                                                       const Vector& mass,
                                                       const Vector& f_grid,
                                                       const Zeeman::Polarization zeeman_polarization,
                                                       const AbsorptionLines& band,
                                                       const SpeciesAuxData::AuxType& partition_type,
                                                       const ArrayOfGriddedField1& partition_data)  {
  constexpr Numeric sq_ln2pi = Constant::sqrt_ln_2 / Constant::sqrt_pi;
  
  // Weighted center of the band
  const Numeric frenorm = band.F_mean();
  
  // Band Doppler broadening constant
  const Numeric GD_div_F0 = Linefunctions::DopplerConstant(T, band.SpeciesMass());
  
  // Sorted population
  const auto [sorting, tp] = sorted_population_and_dipole(T, band, partition_type, partition_data);
  
  // Relaxation matrix
  const ComplexMatrix W = ecs_relaxation_matrix(T, P, vmrs, mass, band, sorting, frenorm);
  
  // Equivalent lines computations
  const EquivalentLines eqv(W, tp.pop, tp.dip);
  
  // Absorption of this band
  ComplexVector absorption(f_grid.nelem(), 0);
  for (Index i=0; i<band.NumLines(); i++) {
    // Zeeman lines if necessary
    const Index nz = band.ZeemanCount(i, zeeman_polarization);
    for (Index j=0; j<nz; j++) {
      const Numeric Sz = band.ZeemanStrength(i, zeeman_polarization, j);
      const Numeric dzeeman = H * band.ZeemanSplitting(i, zeeman_polarization, j);
      
      const Numeric gamd = GD_div_F0 * (eqv.val[i].real() + frenorm + dzeeman);
      const Numeric cte = Constant::sqrt_ln_2 / gamd;
      for (Index iv=0; iv<f_grid.nelem(); iv++) {
        const Complex z = (eqv.val[i] + frenorm + dzeeman - f_grid[iv]) * cte;
        const Complex w = Faddeeva::w(z);
        absorption[iv] += Sz * eqv.str[i] * w / gamd;
      }
    }
  }
  
  // Adjust by frequency and number density
  const Numeric numdens = this_vmr * number_density(P, T);
  for (Index iv=0; iv<f_grid.nelem(); iv++) {
    const Numeric f = f_grid[iv];
    const Numeric fact = f * (1 - stimulated_emission(T, f));
    absorption[iv] *= fact * numdens * sq_ln2pi;
  }
  
  return absorption;
}


/*! Computes the Rosenkranz first order perturbation
 * 
 * @param[in] dip Reduced dipoles
 * @param[in] W The relaxation matrix
 * @param[in] band The absorption band
 * @return First order coefficients
 */
Vector RosenkranzY(const Vector& dip,
                   const ConstMatrixView& W,
                   const AbsorptionLines& band) {
  const Index N = dip.nelem();
  
  // Output
  Vector Y(N, 0);
  
  // Rosenkranz coefficients
  for (Index k=0; k<N; k++) {
    for (Index j=0; j<N; j++) {
      if (k == j) continue;
      else Y[k] += 2 * std::abs(dip[j] / dip[k]) * W(j, k) / (band.F0(k) - band.F0(j));
    }
  }
  
  return Y;
}


/*! Computes the Rosenkranz second order real perturbation
 * 
 * @param[in] dip Reduced dipoles
 * @param[in] W The relaxation matrix
 * @param[in] band The absorption band
 * @return Second order real coefficients
 */
Vector RosenkranzG(const Vector& dip,
                   const ConstMatrixView& W,
                   const AbsorptionLines& band) {
  const Index N = dip.nelem();
  
  // Output
  Vector G(N, 0);
  
  // Rosenkranz coefficients
  for (Index k=0; k<N; k++) {
    for (Index j=0; j<N; j++) {
      if (k == j) continue;
      else {
        G[k] += W(k, j) * W(j, k) / Constant::pow2(band.F0(j) - band.F0(k));
        G[k] += Constant::pow2(std::abs(dip[j] / dip[k]) * W(j, k) / (band.F0(j) - band.F0(k)));
        G[k] += 2 * std::abs(dip[j] / dip[k]) * W(j, k) * W(k, k) / Constant::pow2(band.F0(j) - band.F0(k));
        for (Index l=0; l<N; l++) {
          if (l == k or l == j) continue;
          else {
            G[k] -= 2 * std::abs(dip[j] / dip[k]) * W(j, l) * W(l, k) / ((band.F0(j) - band.F0(k)) * (band.F0(l) - band.F0(k)));
          }
        }
      }
    }
  }
  
  return G;
}


/*! Computes the Rosenkranz second order imaginary perturbation
 * 
 * @param[in] dip Reduced dipoles
 * @param[in] W The relaxation matrix
 * @param[in] band The absorption band
 * @return Second order imaginary coefficients
 */
Vector RosenkranzDV(const Vector& dip,
                    const ConstMatrixView& W,
                    const AbsorptionLines& band) {
  const Index N = dip.nelem();
  
  // Output
  Vector DV(N, 0);
  
  // Rosenkranz coefficients
  for (Index k=0; k<N; k++) {
    for (Index j=0; j<N; j++) {
      if (k == j) continue;
      else DV[k] += W(k, j) * W(j, k) / (band.F0(j) - band.F0(k));
    }
  }
  
  return DV;
}


//! Class to order the data of linemixing
struct RosenkranzAdaptation {
  ArrayOfMatrix Y;   // size N, M ; Y  per species for lines at temperatures
  ArrayOfMatrix G;   // size N, M ; G  per species for lines at temperatures
  ArrayOfMatrix DV;  // size N, M ; DV per species for lines at temperatures
  
  /*! Initialization by sizes
   * 
   * @param[in] N Number of lines
   * @param[in] M Number of temperatures
   * @param[in] S Number of broadening species
   */
  RosenkranzAdaptation(Index N, Index M, Index S) : Y(S, Matrix(N, M)), G(S, Matrix(N, M)), DV(S, Matrix(N, M)) {}
};


/*! Computes the Rosenkranz adaptation
 * 
 * This function does not work properly and using it will result in
 * bad parameters
 * 
 * @param[in] band The absorption band
 * @param[in] temperatures The temperature grid for fitting parameters upon
 * @param[in] mass The mass of all broadeners of the absorption band
 * @param[in] partition_type The type of partition function data
 * @param[in] partition_data The partition function data
 * @return Rosenkranz line mixing parameters
 */
RosenkranzAdaptation ecs_rosenkranz_approximation(const AbsorptionLines& band,
                                                  const Vector& temperatures,
                                                  const Vector& mass,
                                                  const SpeciesAuxData::AuxType& partition_type,
                                                  const ArrayOfGriddedField1& partition_data) {
  const Index N = band.NumLines();
  const Index M = temperatures.nelem();
  const Index S = band.NumBroadeners();
  
  // Need sorting to put weak lines last, but we need the sorting constant or the output jumps
  const ArrayOfIndex sorting = sorted_population_and_dipole(band.T0(), band, partition_type, partition_data).first;
  
  RosenkranzAdaptation out(N, M, S);
  
  for (Index i=0; i<M; i++) {
    for (Index j=0; j<S; j++) {
      const Numeric T = temperatures[i];
      
      // Use pre-sort on the population and dipole to make the curves smooth
      const auto tp = presorted_population_and_dipole(T, sorting, band, partition_type, partition_data);
      
      // Relaxation matrix at T0 sorting at T
      const ComplexMatrix W = single_species_ecs_relaxation_matrix(band, sorting, T, 1, mass[j], j);
      
      // Unsort the output
      const Vector Y = RosenkranzY(tp.dip, W.imag(), band);
      const Vector G = RosenkranzG(tp.dip, W.imag(), band);
      const Vector DV = RosenkranzDV(tp.dip, W.imag(), band);
      for (Index k=0; k<N; k++) {
        out.Y[j](sorting[k], i) = Y[k];
        out.G[j](sorting[k], i) = G[k];
        out.DV[j](sorting[k], i) = DV[k];
      }
    }
  }
  
  return out;
}


Index ecs_rosenkranz_adaptation(AbsorptionLines& band,
                                const Vector& temperatures,
                                const Vector& mass,
                                const SpeciesAuxData::AuxType& partition_type,
                                const ArrayOfGriddedField1& partition_data) {
  const Index N = band.NumLines();
  const Index S = band.NumBroadeners();
  
  const auto lmdata = ecs_rosenkranz_approximation(band, temperatures, mass, partition_type, partition_data);
  
  for (Index iN=0; iN<N; iN++) {
    for (Index iS=0; iS<S; iS++) {
      auto& lineshapemodel = band.AllLines()[iN].LineShape()[iS];
      const ConstVectorView Y  = lmdata.Y[ iS](iN, joker);
      const ConstVectorView G  = lmdata.G[ iS](iN, joker);
      const ConstVectorView D = lmdata.DV[iS](iN, joker);
      const Numeric sX2 = modelparameterFirstExponent(lineshapemodel.G0());
      
      // Best fits and success status
      auto [found_y, yc] = Minimize::curve_fit<Minimize::T4>(temperatures, Y, band.T0(), 1 * sX2);
      auto [found_g, gc] = Minimize::curve_fit<Minimize::T4>(temperatures, G, band.T0(), 2 * sX2);
      auto [found_d, dc] = Minimize::curve_fit<Minimize::T4>(temperatures, D, band.T0(), 2 * sX2);
      
      // Any false in any loop and the function fails so it must leave because we cannot set ByLTE population type
      if (not (found_d and found_g and found_y)) return EXIT_FAILURE;
      
      // Update parameters
      lineshapemodel.Y()  = LineShape::ModelParameters(LineShape::TemperatureModel::T4, yc[0], yc[1], yc[2]);
      lineshapemodel.G()  = LineShape::ModelParameters(LineShape::TemperatureModel::T4, gc[0], gc[1], gc[2]);
      lineshapemodel.DV() = LineShape::ModelParameters(LineShape::TemperatureModel::T4, dc[0], dc[1], dc[2]);
    }
  }
  
  // If we reach here, we have to set the band population type to LTE
  band.Population(Absorption::PopulationType::ByLTE);
  
  return EXIT_SUCCESS;
}
}  // Absorption::LineMixing

