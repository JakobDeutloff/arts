#pragma once

#include <ostream>
#include <unordered_map>

#include "isotopologues.h"
#include "lbl_temperature_model.h"
#include "species.h"

namespace lbl::linemixing {
struct species_data {
  temperature::data scaling{temperature::model_type::T0, {0}};
  temperature::data beta{temperature::model_type::T0, {0}};
  temperature::data lambda{temperature::model_type::T0, {0}};
  temperature::data collisional_distance{temperature::model_type::T0, {0}};
  Numeric mass{1};

  [[nodiscard]] Numeric Q(const Rational J,
                          const Numeric T,
                          const Numeric T0,
                          const Numeric energy) const;

  [[nodiscard]] Numeric Omega(const Numeric T,
                              const Numeric T0,
                              const Numeric other_mass,
                              const Numeric energy_x,
                              const Numeric energy_xm2) const;
};  // species_data

using species_data_map = std::unordered_map<Species::Species, species_data>;

//! FIXME: Should behave as an unordered map of unordered maps, but isn't one because we don't understand pybind11
struct isot_map {
  std::unordered_map<Species::IsotopeRecord, species_data_map> data{};

  species_data_map& operator[](const Species::IsotopeRecord& key) {
    return data[key];
  }
  [[nodiscard]] auto find(const Species::IsotopeRecord& key) const {
    return data.find(key);
  }
  [[nodiscard]] auto begin() noexcept { return data.begin(); }
  [[nodiscard]] auto end() noexcept { return data.end(); }
  [[nodiscard]] auto begin() const noexcept { return data.begin(); }
  [[nodiscard]] auto end() const noexcept { return data.end(); }
  [[nodiscard]] auto cbegin() const noexcept { return data.cbegin(); }
  [[nodiscard]] auto cend() const noexcept { return data.cend(); }
  void clear() noexcept { data.clear(); }
  void reserve(const size_t n) { data.reserve(n); }
  [[nodiscard]] std::size_t size() const noexcept { return data.size(); }
  [[nodiscard]] bool empty() const noexcept { return data.empty(); }

  friend std::ostream& operator<<(std::ostream&, const isot_map&);
};  // isot_map
}  // namespace lbl::linemixing

using LinemixingEcsData = lbl::linemixing::isot_map;
