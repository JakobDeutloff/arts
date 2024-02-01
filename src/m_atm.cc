#include <algorithm>
#include <iomanip>
#include <iterator>
#include <memory>
#include <variant>

#include "atm.h"
#include "compare.h"
#include "configtypes.h"
#include "debug.h"
#include "enums.h"
#include "igrf13.h"
#include "interp.h"
#include "interpolation.h"
#include "isotopologues.h"
#include "matpack_data.h"
#include "mc_interp.h"
#include "operators.h"
#include "quantum_numbers.h"
#include "species_tags.h"
#include "xml_io.h"

void atmospheric_fieldTopOfAtmosphere(AtmField &atmospheric_field,
                                      const Numeric &top_of_atmosphere) {
  atmospheric_field.top_of_atmosphere = top_of_atmosphere;
}

void atmospheric_fieldInit(AtmField &atmospheric_field,
                           const Numeric &top_of_atmosphere,
                           const String &default_isotopologue) {
  atmospheric_field = AtmField{default_isotopologue};
  atmospheric_fieldTopOfAtmosphere(atmospheric_field, top_of_atmosphere);
}

void atmospheric_pointInit(AtmPoint &atmospheric_point,
                           const String &default_isotopologue) {
  atmospheric_point = AtmPoint{default_isotopologue};
}

namespace detail {
/** Tries to read a file as if it were some type T
 *
 * Assigns the value of the read to the atmospheric_field at key_val
 *
 * @tparam T The type
 * @param atmospheric_field As WSV
 * @param key_val A key value
 * @param filename A filename
 * @return true If everything went well
 * @return false If everything went wrong
 */
template <typename T>
bool try_read(AtmField &atmospheric_field,
              const Atm::KeyVal &key_val,
              const String &filename) {
  try {
    T v;
    xml_read_from_file(filename, v);
    std::visit([&](auto &key) { atmospheric_field[key] = v; }, key_val);

    // Return success state
    return true;
  } catch (...) {
    //! CONTROL FLOW --- ARTS CANNOT READ VARIADICALLY

    // We must clean the atmospheric_field so it can work again in the future
    std::visit([&](auto &key) { atmospheric_field.erase(key); }, key_val);

    // Return failure state
    return false;
  }
}

/** Wraps try_read for multiple types
 *
 * The atmospheric_field is updated, the order of short-circuiting is as
 * given by the types, i.e., the execution is (T1 or T2 or T3 or ...)
 *
 * @tparam T... The types
 * @param atmospheric_field As WSV
 * @param key_val A key value
 * @param filename A filename
 * @return true If everything went well
 * @return false If everything went wrong
 */
template <typename... T>
bool try_reading(AtmField &atmospheric_field,
                 const Atm::KeyVal &key_val,
                 const String &filename) {
  return (try_read<T>(atmospheric_field, key_val, filename) or ...);
}

void atmospheric_fieldAddCustomDataFileImpl(
    AtmField &atmospheric_field,
    const Atm::KeyVal &key_val,
    const String &filename,
    const Atm::Extrapolation &extrapolation) {
  const bool ok =
      try_reading<GriddedField3, Numeric>(atmospheric_field, key_val, filename);

  ARTS_USER_ERROR_IF(not ok,
                     "The file ",
                     std::quoted(filename),
                     " cannot be understood as atmospheric field data.\n"
                     "Please make sure that the file exists, that it is "
                     "possible to read the file, and\n"
                     "that its type is one that can be handled by "
                     "ARTS atmospheric fields")

  atmospheric_field[key_val].alt_low = extrapolation;
  atmospheric_field[key_val].lat_low = extrapolation;
  atmospheric_field[key_val].lon_low = extrapolation;
  atmospheric_field[key_val].alt_upp = extrapolation;
  atmospheric_field[key_val].lat_upp = extrapolation;
  atmospheric_field[key_val].lon_upp = extrapolation;
}
}  // namespace detail

void atmospheric_fieldAddCustomDataFile(AtmField &atmospheric_field,
                                        const String &atmospheric_key,
                                        const String &filename,
                                        const String &extrapolation_type) {
  detail::atmospheric_fieldAddCustomDataFileImpl(
      atmospheric_field,
      Atm::toKeyOrThrow(atmospheric_key),
      filename,
      Atm::toExtrapolationOrThrow(extrapolation_type));
}

void atmospheric_fieldAddCustomDataFile(AtmField &atmospheric_field,
                                        const QuantumIdentifier &nlte_key,
                                        const String &filename,
                                        const String &extrapolation_type) {
  detail::atmospheric_fieldAddCustomDataFileImpl(
      atmospheric_field,
      Atm::KeyVal{nlte_key},
      filename,
      Atm::toExtrapolationOrThrow(extrapolation_type));
}

void atmospheric_fieldAddCustomDataFile(AtmField &atmospheric_field,
                                        const ArrayOfSpeciesTag &spec_key,
                                        const String &filename,
                                        const String &extrapolation_type) {
  detail::atmospheric_fieldAddCustomDataFileImpl(
      atmospheric_field,
      Atm::KeyVal{spec_key.Species()},
      filename,
      Atm::toExtrapolationOrThrow(extrapolation_type));
}

void atmospheric_fieldAddField(AtmField &atmospheric_field,
                               const String &filename,
                               const Index &set_top_of_atmosphere) {
  AtmField atmospheric_field_other;
  xml_read_from_file(filename, atmospheric_field_other);
  for (auto &key : atmospheric_field_other.keys()) {
    atmospheric_field[key] = atmospheric_field_other[key];
  }

  if (set_top_of_atmosphere)
    atmospheric_field.top_of_atmosphere =
        atmospheric_field_other.top_of_atmosphere;
}

void atmospheric_fieldRead(AtmField &atmospheric_field,
                           const ArrayOfArrayOfSpeciesTag &abs_species,
                           const String &basename,
                           const Numeric &top_of_atmosphere,
                           const Index &read_tp,
                           const Index &read_mag,
                           const Index &read_wind,
                           const Index &read_specs,
                           const Index &read_nlte) {
  using enum Atm::Key;

  // Fix filename
  String tmp_basename = basename;
  if (basename.length() && basename[basename.length() - 1] != '/')
    tmp_basename += ".";

  // Reset and initialize
  atmospheric_fieldInit(atmospheric_field, top_of_atmosphere, "None");

  if (read_tp) {
    for (auto &key : {t, p}) {
      const String file_name{var_string(tmp_basename, key, ".xml")};
      atmospheric_fieldAddField(atmospheric_field, file_name, 0);
    }
  }

  if (read_mag) {
    for (auto &key : {mag_u, mag_v, mag_w}) {
      const String file_name{var_string(tmp_basename, key, ".xml")};
      atmospheric_fieldAddField(atmospheric_field, file_name, 0);
    }
  }

  if (read_wind) {
    for (auto &key : {wind_u, wind_v, wind_w}) {
      const String file_name{var_string(tmp_basename, key, ".xml")};
      atmospheric_fieldAddField(atmospheric_field, file_name, 0);
    }
  }

  if (read_specs) {
    for (auto &spec : abs_species) {
      const String file_name{
          var_string(tmp_basename, toShortName(spec.Species()), ".xml")};
      atmospheric_fieldAddField(atmospheric_field, file_name, 0);
    }
  }

  if (read_nlte) {
    const String file_name{var_string(tmp_basename, "nlte.xml")};
    atmospheric_fieldAddField(atmospheric_field, file_name, 0);
  }
}

void atmospheric_fieldSave(const AtmField &atmospheric_field,
                           const String &basename,
                           const String &filetype,
                           const Index &no_clobber) {
  const auto ftype = string2filetype(filetype);

  // Fix filename
  String tmp_basename = basename;
  if (basename.length() && basename[basename.length() - 1] != '/')
    tmp_basename += ".";

  const auto keys = atmospheric_field.keys();

  //
  ArrayOfSpecies specs{};
  ArrayOfIndex nspecs{};

  //
  AtmField nlte;
  nlte.top_of_atmosphere = atmospheric_field.top_of_atmosphere;

  for (auto &key : keys) {
    if (std::holds_alternative<QuantumIdentifier>(key)) {
      nlte[key] = atmospheric_field[key];
    } else {
      String keyname;
      if (std::holds_alternative<Species::Species>(key)) {
        auto *spec_key = std::get_if<Species::Species>(&key);
        auto spec = *spec_key;

        keyname = toString(spec);
        if (auto ptr = std::find(specs.begin(), specs.end(), spec);
            ptr == specs.end()) {
          specs.emplace_back(spec);
          nspecs.emplace_back(1);
        } else {
          const auto pos = std::distance(specs.begin(), ptr);
          nspecs[pos]++;
          keyname += var_string(".", nspecs[pos]);
        }
      } else if (std::holds_alternative<Atm::Key>(key)) {
        keyname = toString(*std::get_if<Atm::Key>(&key));
      } else {
        ARTS_ASSERT(false, "Failed to account for key type")
      }

      AtmField out;
      out.top_of_atmosphere = atmospheric_field.top_of_atmosphere;
      out[key] = atmospheric_field[key];
      const String filename{var_string(tmp_basename, keyname, ".xml")};
      xml_write_to_file(filename, out, ftype, no_clobber);
    }
  }

  if (nlte.nnlte()) {
    const String filename{var_string(tmp_basename, "nlte.xml")};
    xml_write_to_file(filename, nlte, ftype, no_clobber);
  }
}

void atmospheric_fieldAddGriddedData(AtmField &atmospheric_field,
                                     const String &key,
                                     const GriddedField3 &data,
                                     const String &extrapolation_type) {
  auto &fld = atmospheric_field[Atm::toKeyOrThrow(key)] = data;

  const auto extrapolation = Atm::toExtrapolationOrThrow(extrapolation_type);

  fld.alt_low = extrapolation;
  fld.lat_low = extrapolation;
  fld.lon_low = extrapolation;
  fld.alt_upp = extrapolation;
  fld.lat_upp = extrapolation;
  fld.lon_upp = extrapolation;
}

void atmospheric_fieldAddGriddedData(AtmField &atmospheric_field,
                                     const ArrayOfSpeciesTag &key,
                                     const GriddedField3 &data,
                                     const String &extrapolation_type) {
  auto &fld = atmospheric_field[key.Species()] = data;

  const auto extrapolation = Atm::toExtrapolationOrThrow(extrapolation_type);

  fld.alt_low = extrapolation;
  fld.lat_low = extrapolation;
  fld.lon_low = extrapolation;
  fld.alt_upp = extrapolation;
  fld.lat_upp = extrapolation;
  fld.lon_upp = extrapolation;
}

void atmospheric_fieldAddGriddedData(AtmField &atmospheric_field,
                                     const QuantumIdentifier &key,
                                     const GriddedField3 &data,
                                     const String &extrapolation_type) {
  auto &fld = atmospheric_field[key] = data;

  const auto extrapolation = Atm::toExtrapolationOrThrow(extrapolation_type);

  fld.alt_low = extrapolation;
  fld.lat_low = extrapolation;
  fld.lon_low = extrapolation;
  fld.alt_upp = extrapolation;
  fld.lat_upp = extrapolation;
  fld.lon_upp = extrapolation;
}

void atmospheric_fieldAddNumericData(AtmField &atmospheric_field,
                                     const String &key,
                                     const Numeric &data) {
  atmospheric_field[Atm::toKeyOrThrow(key)] = data;
}

void atmospheric_fieldAddNumericData(AtmField &atmospheric_field,
                                     const ArrayOfSpeciesTag &key,
                                     const Numeric &data) {
  atmospheric_field[key.Species()] = data;
}

void atmospheric_fieldAddNumericData(AtmField &atmospheric_field,
                                     const QuantumIdentifier &key,
                                     const Numeric &data) {
  atmospheric_field[key] = data;
}

void atmospheric_fieldIGRF(AtmField &atmospheric_field,
                           const Time &time,
                           const Index &parsafe) {
  using namespace IGRF;

  //! We need explicit planet-size as IGRF requires the radius
  //! This is the WGS84 version of that, with radius of equator and pole
  static constexpr Vector2 ell{6378137., 6356752.314245};

  //! This struct deals with the computations, it's internally saving a mutable
  //! state
  struct res {
    mutable Numeric u{}, v{}, w{};
    mutable Numeric alt{}, lat{}, lon{};
    const Time t;

    res(const Time &x) : t(x) {}

    void comp(Numeric al, Numeric la, Numeric lo) const {
      if (alt != al or lat != la or lo != lon) {
        alt = al;
        lat = la;
        lon = lo;

        auto f = compute(Vector{alt}.reshape(1, 1, 1), {lat}, {lon}, t, ell);
        u = f.u(0, 0, 0);
        v = f.v(0, 0, 0);
        w = f.w(0, 0, 0);
      }
    }

    Numeric get_u(Numeric z, Numeric la, Numeric lo) const {
      comp(z, la, lo);
      return u;
    }

    Numeric get_v(Numeric z, Numeric la, Numeric lo) const {
      comp(z, la, lo);
      return v;
    }

    Numeric get_w(Numeric z, Numeric la, Numeric lo) const {
      comp(z, la, lo);
      return w;
    }
  };

  /** Different methods for parallel and non-parallel safe versions
   * In the paralell safe version each field component holds a copy
   * of the res-struct.  In the unsafe version, the res-struct is
   * shared between the fields.
   *
   * The parallel version is thus safe because it is doing all its
   * computations locally but the non-parallel version is 3X faster
   * because it saves the state of the query for, e.g., u to be
   * reused by v and w (with no regards for order) */
  if (parsafe == 0) {
    const std::shared_ptr igrf_ptr = std::make_shared<res>(time);
    atmospheric_field[Atm::Key::mag_u] = Atm::FunctionalData{
        [ptr = igrf_ptr](Numeric h, Numeric lat, Numeric lon) {
          return ptr->get_u(h, lat, lon);
        }};
    atmospheric_field[Atm::Key::mag_v] = Atm::FunctionalData{
        [ptr = igrf_ptr](Numeric h, Numeric lat, Numeric lon) {
          return ptr->get_v(h, lat, lon);
        }};
    atmospheric_field[Atm::Key::mag_w] = Atm::FunctionalData{
        [ptr = igrf_ptr](Numeric h, Numeric lat, Numeric lon) {
          return ptr->get_w(h, lat, lon);
        }};
  } else {
    atmospheric_field[Atm::Key::mag_u] = Atm::FunctionalData{
        [cpy = res(time)](Numeric h, Numeric lat, Numeric lon) {
          return res{cpy}.get_u(h, lat, lon);
        }};
    atmospheric_field[Atm::Key::mag_v] = Atm::FunctionalData{
        [cpy = res(time)](Numeric h, Numeric lat, Numeric lon) {
          return res{cpy}.get_v(h, lat, lon);
        }};
    atmospheric_field[Atm::Key::mag_w] = Atm::FunctionalData{
        [cpy = res(time)](Numeric h, Numeric lat, Numeric lon) {
          return res{cpy}.get_w(h, lat, lon);
        }};
  }
}

enum class atmospheric_fieldHydrostaticPressureDataOptions : char {
  Lat,
  Lon,
  Hypsometric,
  Hydrostatic,
};

template <atmospheric_fieldHydrostaticPressureDataOptions... input_opts>
struct atmospheric_fieldHydrostaticPressureData {
  using enum atmospheric_fieldHydrostaticPressureDataOptions;
  static constexpr std::array<atmospheric_fieldHydrostaticPressureDataOptions,
                              sizeof...(input_opts)>
      opts{input_opts...};
  static constexpr bool do_lat = std::ranges::any_of(opts, Cmp::eq(Lat));
  static constexpr bool do_lon = std::ranges::any_of(opts, Cmp::eq(Lon));

  using LatLag = my_interp::Lagrange<Index{do_lat}>;
  using LonLag = my_interp::Lagrange<Index{do_lon},
                                     false,
                                     my_interp::GridType::Cyclic,
                                     my_interp::cycle_m180_p180>;

  template <atmospheric_fieldHydrostaticPressureDataOptions X>
  static constexpr bool is = std::ranges::any_of(opts, Cmp::eq(X));

  Tensor3 grad_p;
  Tensor3 pre;
  Vector alt;
  Vector lat;
  Vector lon;

  static Numeric step(Numeric p, Numeric h, Numeric d) {
    if constexpr (is<Hypsometric>) {
      return p * std::exp(-h * d);
    } else if constexpr (is<Hydrostatic>) {
      return std::max<Numeric>(0, p * (1.0 - h * d));
    }
  }

  atmospheric_fieldHydrostaticPressureData(Tensor3 in_grad_p,
                                           const GriddedField2 &pre0,
                                           Vector in_alt)
      : grad_p(std::move(in_grad_p)),
        pre(grad_p),  // Init sizes
        alt(std::move(in_alt)),
        lat(pre0.grid<0>()),
        lon(pre0.grid<1>()) {
    pre[0] = pre0.data;
    for (Index i = 1; i < alt.nelem(); i++) {
      for (Index j = 0; j < lat.nelem(); j++) {
        for (Index k = 0; k < lon.nelem(); k++) {
          const Numeric h = alt[i] - alt[i - 1];
          const Numeric p0 = pre(i - 1, j, k);
          const Numeric d0 = grad_p(i - 1, j, k);

          pre(i, j, k) = step(p0, h, d0);
        }
      }
    }
  }

  [[nodiscard]] std::pair<Index, Numeric> find_alt(Numeric al) const {
    auto i = std::distance(alt.begin(), std::ranges::upper_bound(alt, al));
    i -= (i == alt.size());
    while (i > 0 and alt[i] > al) {
      i--;
    }

    return {i, al - alt[i]};
  }

  [[nodiscard]] std::pair<Numeric, Numeric> level(Index alt_ind,
                                                  Numeric la,
                                                  Numeric lo) const {
    const LatLag latlag(0, la, lat);
    const LonLag lonlag(0, lo, lon);
    const auto iw = interpweights(latlag, lonlag);
    const Numeric p = interp(pre[alt_ind], iw, latlag, lonlag);
    const Numeric d = interp(grad_p[alt_ind], iw, latlag, lonlag);
    return {p, d};
  }

  Numeric operator()(Numeric al, Numeric la, Numeric lo) const {
    const auto [i, h] = find_alt(al);
    const auto [p, d] = level(i, la, lo);
    return step(p, h, d);
  }
};

ENUMCLASS(HydrostaticPressureOption,
          char,
          HydrostaticEquation,
          HypsometricEquation)

void atmospheric_fieldHydrostaticPressure(
    AtmField &atmospheric_field,
    const NumericTernaryOperator &gravity_operator,
    const GriddedField2 &p0,
    const Vector &alts,
    const Numeric &fixed_specific_gas_constant,
    const Numeric &fixed_atm_temperature,
    const String &hydrostatic_option) {
  ARTS_ASSERT(false, "Fix isotopologues")
  using enum atmospheric_fieldHydrostaticPressureDataOptions;
  using enum HydrostaticPressureOption;

  const Vector &lats = p0.grid<0>();
  const Vector &lons = p0.grid<1>();

  const Index nalt = alts.size();
  const Index nlat = lats.size();
  const Index nlon = lons.size();

  ARTS_USER_ERROR_IF(nalt < 1 or nlat < 1 or nlon < 1,
                     "Must have at least 1-sized alt, lat, and lon grids")

  ARTS_USER_ERROR_IF(
      p0.grid_names[0] not_eq "Latitude" or
          p0.grid_names[1] not_eq "Longitude" or not p0.check(),
      "Bad gridded field, must have right size.\n"
      "Must also have \"Latitude\" as first grid and \"Longitude\" as second grid.\n"
      "Field:\n",
      p0)

  const bool has_def_t = fixed_atm_temperature > 0;
  const bool has_def_r = fixed_specific_gas_constant > 0;

  ARTS_USER_ERROR_IF(
      not has_def_t and not atmospheric_field.contains(Atm::Key::t),
      "atmospheric_field lacks temperature and no default temperature given")

  ARTS_USER_ERROR_IF(
      not has_def_r and atmospheric_field.nspec() == 0,
      "atmospheric_field lacks species and no default specific gas constant given")

  const Tensor3 scale_factor = [&]() {
    Tensor3 scl(nalt, nlat, nlon);
    for (Index i = 0; i < nalt; i++) {
      for (Index j = 0; j < nlat; j++) {
        for (Index k = 0; k < nlon; k++) {
          const Numeric al = alts[i];
          const Numeric la = lats[j];
          const Numeric lo = lons[k];

          const Numeric g = gravity_operator(al, la, lo);
          const AtmPoint atmospheric_point{atmospheric_field.at(al, la, lo)};

          const Numeric inv_specific_gas_constant =
              has_def_r ? 1.0 / fixed_specific_gas_constant
                        : (1e-3 * atmospheric_point.mean_mass() / Constant::R);
          const Numeric inv_temp = has_def_t
                                       ? 1.0 / fixed_atm_temperature
                                       : 1.0 / atmospheric_point.temperature;

          // Partial rho, no pressure
          scl(i, j, k) = g * inv_specific_gas_constant * inv_temp;
        }
      }
    }

    return scl;
  }();

  switch (toHydrostaticPressureOptionOrThrow(hydrostatic_option)) {
    case HypsometricEquation:
      if (nlon > 1 and nlat > 1) {
        atmospheric_field[Atm::Key::p] = Atm::FunctionalData{
            atmospheric_fieldHydrostaticPressureData<Hypsometric, Lat, Lon>(
                scale_factor, p0, alts)};
      } else if (nlat > 1) {
        atmospheric_field[Atm::Key::p] = Atm::FunctionalData{
            atmospheric_fieldHydrostaticPressureData<Hypsometric, Lat>(
                scale_factor, p0, alts)};
      } else if (nlon > 1) {
        atmospheric_field[Atm::Key::p] = Atm::FunctionalData{
            atmospheric_fieldHydrostaticPressureData<Hypsometric, Lon>(
                scale_factor, p0, alts)};
      } else {
        atmospheric_field[Atm::Key::p] = Atm::FunctionalData{
            atmospheric_fieldHydrostaticPressureData<Hypsometric>(
                scale_factor, p0, alts)};
      }
      break;

    case HydrostaticEquation:
      if (nlon > 1 and nlat > 1) {
        atmospheric_field[Atm::Key::p] = Atm::FunctionalData{
            atmospheric_fieldHydrostaticPressureData<Hydrostatic, Lat, Lon>(
                scale_factor, p0, alts)};
      } else if (nlat > 1) {
        atmospheric_field[Atm::Key::p] = Atm::FunctionalData{
            atmospheric_fieldHydrostaticPressureData<Hydrostatic, Lat>(
                scale_factor, p0, alts)};
      } else if (nlon > 1) {
        atmospheric_field[Atm::Key::p] = Atm::FunctionalData{
            atmospheric_fieldHydrostaticPressureData<Hydrostatic, Lon>(
                scale_factor, p0, alts)};
      } else {
        atmospheric_field[Atm::Key::p] = Atm::FunctionalData{
            atmospheric_fieldHydrostaticPressureData<Hydrostatic>(
                scale_factor, p0, alts)};
      }
      break;

    case FINAL:
      ARTS_ASSERT(false, "Cannot understand option: ", hydrostatic_option)
  }
}