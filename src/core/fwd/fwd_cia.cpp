#include "fwd_cia.h"

#include <arts_constexpr_math.h>
#include <physics_funcs.h>

#include <algorithm>
#include <functional>
#include <numeric>

#include "cia.h"
#include "debug.h"

namespace fwd::cia {
full::single::single(Numeric p,
                     Numeric t,
                     Numeric VMR1,
                     Numeric VMR2,
                     CIARecord* cia,
                     Numeric extrap,
                     Index robust)
    : scl(VMR1 * VMR2 * Math::pow2(number_density(p, t))),
      T(t),
      extrapol(extrap),
      ignore_errors(robust),
      ciarecords(cia) {}

Complex full::single::at(Numeric f) const {
  return scl * ciarecords->Extract(f, T, extrapol, ignore_errors);
}

void full::single::at(ExhaustiveComplexVectorView abs, const Vector& fs) const {
  std::transform(
      fs.begin(),
      fs.end(),
      abs.begin(),
      abs.begin(),
      [this](const Numeric& f, const Complex& s) { return s + at(f); });
}

ComplexVector full::single::at(const Vector& fs) const {
  ComplexVector abs(fs.size());
  at(abs, fs);
  return abs;
}

void full::adapt() {
  models.resize(0);
  models.reserve(ciarecords->size());
  for (CIARecord& data : *ciarecords) {
    const Numeric VMR1 = atm->operator[](data.Species(0));
    const Numeric VMR2 = atm->operator[](data.Species(1));

    models.emplace_back(
        atm->pressure, atm->temperature, VMR1, VMR2, &data, extrap, robust);
  }
}

full::full(std::shared_ptr<AtmPoint> atm_,
           std::shared_ptr<ArrayOfCIARecord> cia,
           Numeric extrap_,
           Index robust_)
    : atm(std::move(atm_)),
      ciarecords(std::move(cia)),
      extrap(extrap_),
      robust(robust_) {
  adapt();
}

Complex full::operator()(Numeric f) const {
  return std::transform_reduce(
      models.begin(), models.end(), Complex{}, std::plus<>{}, [f](auto& mod) {
        return mod.at(f);
      });
}

void full::operator()(ExhaustiveComplexVectorView abs, const Vector& fs) const {
  for (auto& mod : models) {
    mod.at(abs, fs);
  }
}

ComplexVector full::operator()(const Vector& fs) const {
  ComplexVector abs(fs.size());
  operator()(abs, fs);
  return abs;
}

void full::set_extrap(Numeric extrap_) {
  extrap = extrap_;
  adapt();
}

void full::set_robust(Index robust_) {
  robust = robust_;
  adapt();
}

void full::set_model(std::shared_ptr<ArrayOfCIARecord> cia) {
  ciarecords = std::move(cia);
  adapt();
}

void full::set_atm(std::shared_ptr<AtmPoint> atm_) {
  atm = std::move(atm_);
  adapt();
}
}  // namespace fwd::cia