#include "workspace_variables.h"

std::unordered_map<std::string, WorkspaceVariableInternalRecord>
internal_workspace_variables() {
  std::unordered_map<std::string, WorkspaceVariableInternalRecord> wsv_data;

  wsv_data["absorption_bands"] = {
      .desc = R"--(Bands of absorption lines for LBL calculations.
)--",
      .type = "AbsorptionBands"};

  wsv_data["aa_grid"] = {.desc = R"--(Azimuthal angle grid.

The azimutal angle grid, on which the *cloudbox_field* is stored. 
This grid is used for RT calculations inside the cloudbox, 
therefore one has to define it if the cloudbox is activated by 
the flag *cloudbox_on*. Furthermore the zenith angle grid is also used
for RT calculations of clear-sky *spectral_radiance_field*.
The grid must be sorted in increasing order, with no repetitions.

Usage:      Set by the user.

Unit:       degrees
)--",
                         .type = "Vector"};

  wsv_data["aa_index"] = {
      .desc = R"--(Azimuth angle index for scattering calculations.

This variable is used in methods used for computing scattering
properties. 
It holds the information about the azimuth angles for which the 
scattering calculations are done.  The angles used for computing 
scattering properties of particles can be different from that used 
for radiative transfer calculation. 

Usage:    Method output.
)--",
      .type = "Index"};

  wsv_data["abs_cia_data"] = {
      .desc = R"--(HITRAN Collision Induced Absorption (CIA) Data.

This variable holds HITRAN CIA data (binary absorption
cross-sections). The data itself is described in: Richard, C. et al.
(2012), New section of the HITRAN database: Collision-induced
absorption (CIA), J. Quant. Spectrosc. Radiat. Transfer, 113,
1276-1285, doi:10.1016/j.jqsrt.2011.11.004.

The binary absorption cross-sections have to be multiplied with the
densities of both molecules to get absorption coefficients.

Dimensions:

The outer array dimension in the ArrayOfArrayOfCIARecord is the same
as that of *abs_species*. There will be CIA data only for those
species that contain a CIA tag, for all other species it will be
empty. The inner array dimension corresponds to the number of CIA tags
for this species (there could be for example both N2-N2 and N2-H2) in
the same species.

The CIA *abs_species* tags are described in *abs_speciesSet*.

Each individual CIARecord holds the complete information from one
HITRAN CIA file. For the given pair of molecules A HITRAN CIA data
file can hold several datasets (data for different temperatures but
fixed frequency range).

Units:

 - Frequencies: Hz
 - Binary absorption cross-sections: m^5*molecule^-2
)--",
      .type = "ArrayOfCIARecord"};

  wsv_data["abs_f_interp_order"] = {
      .desc = R"--(Frequency interpolation order for absorption lookup table.

The interpolation order to use when interpolating the absorption
lookup table in frequency. This is in particular needed for
calculations with Doppler shift, so that absorption is interpolated to
the shifted frequency grid. One is linear interpolation, two
quadratic, and so on.

As a special case, order 0 in this particular case means no
interpolation. In that case f_grid must match exactly the grid inside
the lookup table. This is the global default value.
)--",
      .type = "Index",
      .default_value = Index{0}};

  wsv_data["abs_hitran_relmat_data"] = {
      .desc = R"--(HITRAN line mixing data to compute the relaxation matrix.

This variable holds HITRAN line mixing data
as per J. Lamouroux, L. Realia, X. Thomas, et al., J.Q.S.R.T. 151 (2015), 88-96

It is used for absorption bands with these population tags:
 - ByHITRANFullRelmat
 - ByHITRANRosenkranzRelmat
)--",
      .type = "HitranRelaxationMatrixData"};

  wsv_data["abs_lines"] = {.desc = R"--(A list of spectral line data.
)--",
                           .type = "ArrayOfAbsorptionLines"};

  wsv_data["abs_lines_per_species"] = {
      .desc = R"--(A list of spectral line data for each tag.

Dimensions: [*abs_species*.nelem()][Depends on how many bands there are in *abs_lines*]
)--",
      .type = "ArrayOfArrayOfAbsorptionLines"};

  wsv_data["abs_lookup"] = {.desc = R"--(An absorption lookup table.

It holds an absorption lookup table, as well as all information that
is necessary to use the table to extract absorption. Extraction
routines are implemented as member functions. 

It has quite a complicated structure. For details see the Arts User
Guide section \"The gas absorption lookup table\" or the source code
documentation in gas_abs_lookup.h.
)--",
                            .type = "GasAbsLookup"};

  wsv_data["abs_nls"] = {
      .desc = R"--(Nonlinear species for absorption lookup table generation.

A list of absorption species that should be treated non-linearly.
This means that the H2O VMR should be varied when calculating the
lookup table for those species.

A typical example is for this to containt the Rosenkranz full
absorption model species for water vapor and oxygen 
([\"H2O-PWR98\", \"O2-PWR93\"]).

See user guide and online documentation of ``abs_lookupCalc``
for more details and usage examples.
)--",
      .type = "ArrayOfArrayOfSpeciesTag"};

  wsv_data["abs_nls_pert"] = {
      .desc =
          R"--(Fractional perturbations for the nonlinear species in the absorption
lookup table.

This is a vector of fractional perturbations that should contain 1
(the unperturbed reference profile). A value of 0 may lead to error
messages from some absorption routines, so a possible content for this
variable is: [1e-24, 1, 2].
(This is similar to *abs_t_pert*, but multiplicative, not additive.)
)--",
      .type = "Vector"};

  wsv_data["abs_nls_interp_order"] = {
      .desc =
          R"--(The interpolation order to use when interpolating absorption between
the H2O values given by *abs_nls_pert*.

This is used by methods extracting absorption coefficients
from the lookup table, and by methods setting up
parameters for lookup table generation.

Note that the number of points used in the interpolation scheme is
interpolation order + 1 (e.g., two for first order interpolation).
)--",
      .type = "Index",
      .default_value = Index{5}};

  wsv_data["abs_p_interp_order"] = {
      .desc = R"--(The interpolation order to use when interpolating absorption
between pressure levels.

This is used by methods extracting absorption coefficients
from the lookup table, and by methods
setting up parameters for lookup table generation.

Note that the number of points used in the interpolation scheme is
interpolation order + 1 (e.g., two for first order interpolation).
)--",
      .type = "Index",
      .default_value = Index{5}};

  wsv_data["abs_t_pert"] = {
      .desc = R"--(Temperature perturbations for the absorption lookup table.

This is a vector containing temperature perturbations (in Kelvin) that
should be added to the reference temperature profile. (Similar to
*abs_nls_pert*, but additive, not multiplicative.) Should normally
contain 0, to include the reference profile itself. Example content:
[-5, 0, 5].
)--",
      .type = "Vector"};

  wsv_data["abs_t_interp_order"] = {
      .desc =
          R"--(The interpolation order to use when interpolating absorption between
the temperature values given by *abs_t_pert*.

This is used by methods
extracting absorption coefficients from the lookup table, and by
methods setting up parameters for lookup table generation.

Note that the number of points used in the interpolation scheme is
interpolation order + 1 (e.g., two for first order interpolation).
)--",
      .type = "Index",
      .default_value = Index{7}};

  wsv_data["abs_lookup_is_adapted"] = {
      .desc = R"--(Flag to indicate whether *abs_lookupAdapt* has already been
called.

Values:
 - 0=false - 1=true.
)--",
      .type = "Index"};

  wsv_data["abs_species"] = {.desc = R"--(Tag groups for gas absorption.

This is an array of arrays of SpeciesTag tag definitions. It defines the
available tag groups for the calculation of scalar gas absorption
coefficients.  See online documentation of method *abs_speciesSet* for
more detailed information how tag groups work and some examples.
)--",
                             .type = "ArrayOfArrayOfSpeciesTag"};

  wsv_data["abs_vec"] = {.desc = R"--(Total absorption vector.

This variable contains the absorption coefficient vector which
is used in the RTE calculation. It is the physical absorption which
includes particle absorption for all considered scattering elements as
well as gaseous absorption for all selected gaseous species.
The vector is calculated by *opt_prop_bulkCalc*
The dimension of the variable adapts to ``stokes_dim``.

See ARTS user guide (AUG) for further information. Use the index to find
where this variable is discussed. The variable is listed as a subentry
to \"workspace variables\".

Usage:      Output of *opt_prop_bulkCalc* 

Unit:       m^2 FIXME: really m2? not 1/m?

Dimensions: [f_grid, stokes_dim]
)--",
                         .type = "StokvecVector"};

  wsv_data["abs_vec_spt"] = {
      .desc = R"--(Absorption vectors of the scattering elements.

This variable contains the elements of the absorption vector of the
individual scattering elements. It is calculated in the agenda 
*spt_calc_agenda*.

See ARTS user guide (AUG) for further information.

Usage:      Input and Output of the method abs_vec_sptCalc

Unit:        m^2 FIXME: really m2? not 1/m?

Dimensions: [number of scattering elements, stokes_dim]
)--",
      .type = "ArrayOfStokvecVector"};

  wsv_data["agenda_array_index"] = {
      .desc = R"--(Index of the current agenda in *ArrayOfAgenda*.

This is set during the execution of an agenda from an *ArrayOfAgenda*.
It indicates the index of the current agenda inside the array.

Unit:  Integer value.
)--",
      .type = "Index"};

  wsv_data["antenna_dim"] = {
      .desc = R"--(The dimensionality of the antenna pattern (1-2).

A dimensionality of 1 means that only the respons variation in the
zenith direction is considered. The provided respons shall then be the
integrated in the azimuth direction. For 2D, the respons of the
antenna has both a zenith and azimuth variation.

Usage: Set by the user.

Unit:  Integer value [1-2].
)--",
      .type = "Index"};

  wsv_data["antenna_response"] = {.desc = R"--(The antenna pattern/response.

This WSV describes the antenna response as a function of polarisation
(pol), frequencue (f), zenith angle (za) and azimuth angle (aa).

Polarisation dimension: If this dimension has size 1, the data are
applied for all polarisations of concern. The data are otherwise used
in sequential order. This signifies that, in general, the first
polarisation \"layer\" corresponds to the first stokes dimension
etc. An exception is if a polarisation rotation has been applied. In
any case, it is up to the user to ensure that polarisations are
consistently defined.

Frequency dimension: If this dimension has size 1, the data are
applied for all frequencies of concern. The given frequency must be
inside the frequency range of concern. A linear interpolation is
otherwise applied.

Zenith angle dimension: This dimension must always have a size >= 2
The response outside covered grid range is treated as zero. If
*antenna_dim* equals 1, the data should correspond to the response
integrated in the azimuthal direction.

Azimuth angle dimension: If *antenna_dim* equals 1, this dimension
must have size 1. A size >= 2 is otherwise required. The response
outside covered grid range is treated as zero.

Usage: Set by the user.

Unit: The unit of the actual response is 1/sr. Properly normalised
a 4pi integration shall shall give 1.

Dimensions: 

- NamedGriddedField3:

  - ArrayOfString field_names[N_pol]
  - Vector f_grid[N_f]
  - Vector za_grid[N_za]
  - Vector aa_grid[N_aa]
  - Tensor4 data[N_pol][N_f][N_za][N_aa]
)--",
                                  .type = "NamedGriddedField3"};

  wsv_data["atm_field"] = {.desc =
                               R"--(An atmospheric field in ARTS.

The atmospheric field defines the altitude of the top-of-the-atmosphere,
as well as the variables that are required for the radiative transfer
calculations along a path through the atmosphere.

This atmospheric field may exist on a regular grid but can also be
free-form. Some methods require the atmospheric field to be on a regular
grid, so we provide INSERT-METHOD-NAME-HERE to regularize the field grid.

The atmospheric field may, but does not have to, consist of the following:
    Temperature         - Kelvin
    Pressure            - Pascal
    Wind                - Meters per second
    Magnetic Field      - Tesla
    Species content     - See user guide for relevant species
    NLTE ratios         - Unitless [pure-style] OR Kelvin [vibrational-style]
)--",
                           .type = "AtmField"};

  wsv_data["atm_point"] = {.desc =
                               R"--(An atmospheric point in ARTS.

The atmospheric point consists of all the relevant atmospheric field data
at a discrete point in the atmosphere.  It is often extracted from an *AtmField*
at a single altitude-latitude-longitude but may of course be generated manually.

See *atm_field* for the data that may be available in the atmospheric point.
)--",
                           .type = "AtmPoint"};

  wsv_data["atmfields_checked"] = {
      .desc = R"--(OK-flag for atmospheric grids and (physical) fields.

The variable flags that clear-sky part of the atmosphere is
defined in formally correct way. Example on problems captured
include that the size of an atmospheric fields does not match the
length of the atmospheric grids, and physically incorrect data such
as negative temperatures.

Note that ``z_field`` is not covered by this variable, it is instead
treated to be part of the geometrical considerations where the ok-flag
is denoted as *atmgeom_checked*. The cloudbox is covered by
*cloudbox_checked*.

Shall be set by *atmfields_checkedCalc*. See that WSMs for treated
WSVs. Only the value 1 is taken as OK.
)--",
      .type = "Index",
      .default_value = Index{0}};

  wsv_data["atmgeom_checked"] = {
      .desc = R"--(OK-flag for the geometry of the model atmosphere.

The variable flags that reference ellipsoid, the surfae and ``z_field``
contain formally correct values. Includes for example, that ``z_field``
holds strictly increasing values at each geographical position.

See also *atmfields_checked*.

Shall be set by ``atmgeom_checkedCalc``. Only the value 1 is taken
as OK.
)--",
      .type = "Index",
      .default_value = Index{0}};

  wsv_data["atm_fields_compact"] = {
      .desc = R"--(A compact set of atmospheric fields on a common set of grids.

Data is supposed to contain basic atmsopheric fields for a RT
calculation, i.e., temperature, altitude, and gas VMRs. It can
furthermore contain fields describing scattering species like mass
content, mass flux, number density of diverse scattering species.

VMR fields are unitless, scattering species fields are supposed to be
in SI units (i.e. kg/m3 for mass contents, kg/m2/s for mass flux,
1/m3 for number densities).

The data are stored in a *GriddedField4*.

The first field in the matrix (i.e., first matrix column) has to be
atmospheric pressure. Apart from this, the order of the fields is
free. Field content (apart from pressure) is identified by their
given field name tag. Furthermore, absorption species (e.g. VMR)
fields and scattering species fields are related to *abs_species*
and *scat_species* entries, respectively, by their field name tags.
The tags must exhibit the following structure:

0) species identifier:
   Fields, supposed to be sorted into ``vmr_field``, must be headed the
   tag 'abs_species'. Names of scattering species fields likewise must
   be headed by the 'scat_species' tag. Temperature and altitude
   fields do not hold any heading tag.
1) species name:
   The (core) name of the field: 'T' for temperature, 'z' for
   altitude, the absorption species name (e.g. 'H2O, 'O3', etc.) for
   absorption species, the scattering species name (e.g. 'IWC') for
   scattering species. For scattering species, this part is matched
   against the scattering species name part of the *scat_species*
   tags.
2) field type:
   This has to be given for scattering species only, indicating the
   type of the scattering species fields, i.e. 'mass_density',
   'mass_flux', 'number_density', 'mean_mass'.

Dashes ('-') serve as delimiter, separating the elements of each
field name tag.

Usage: Used inside batch calculations, to hold successive atmospheric
states from an *ArrayOfGriddedField4*.

Dimensions: 

- GriddedField4:

  - ArrayOfString field_names[N_fields]
  - Vector p_grid[N_p]
  - Vector lat_grid[N_lat]
  - Vector lon_grid[N_lon]
  - Tensor4 data[N_fields][N_p][N_lat][N_lon]
)--",
      .type = "GriddedField4"};

  wsv_data["avk"] = {.desc = R"--(Averaging kernel matrix.

This matrix is the partial derivative of the retrieved state vector
with respect to the measurement vector (``y``).

Usage: Used and set by inversion methods.
)--",
                     .type = "Matrix"};

  wsv_data["backend_channel_response"] = {
      .desc = R"--(The response of each backend channel.

The response is given as an *ArrayOfGriddedField1*. The grid consists of
relative frequencies. These relative frequencies are added to 
*f_backend* to obtain the absolute frequency for each response value.
The actual data are the response at each frequency grid point.

There are here two options. If the array has length 1, the same
response is applied for all channels. Accordingly, this assumes that
all channels have the same response function. The second option is to
specify the response for each channel seperately. This signifies that
the *backend_channel_response* array has either 1 or n elements, where
n is the length of *f_backend*

Usage: Set by the user.

Size:

- Array[N_ch]

  - GriddedField1

    - [N_f]
    - [N_f]
)--",
      .type = "ArrayOfGriddedField1"};

  wsv_data["backend_channel_response_multi"] = {
      .desc =
          R"--(As *backend_channel_response* but describes an instrument with
muliple mixer/receiver chains.

See *f_backend_multi* for when to use this variable and size
constraints.

Usage: Set by the user.
)--",
      .type = "ArrayOfArrayOfGriddedField1"};

  wsv_data["batch_atm_fields_compact"] = {
      .desc = R"--(An array of compact atmospheric states.

This is used to hold a set of *atm_fields_compact* for batch
calculations. For further information see *atm_fields_compact*.
)--",
      .type = "ArrayOfGriddedField4"};

  wsv_data["band_identifiers"] = {
      .desc = R"--(An array of identifiers for bands.

Used by line mixing calculations to identify which bands to match to the
line database.
)--",
      .type = "ArrayOfQuantumIdentifier"};

  wsv_data["batch_cloudbox_limits"] = {
      .desc = R"--(An array of *cloudbox_limits*.

This is used to hold a set of *cloudbox_limits* for batch
calculations.
)--",
      .type = "ArrayOfArrayOfIndex"};

  wsv_data["batch_pnd_fields"] = {.desc = R"--(An array of compact pnd states.

This is used to hold a set of 1D *pnd_field* for batch
calculations.
)--",
                                  .type = "ArrayOfTensor4"};

  wsv_data["channel2fgrid_indexes"] = {
      .desc = R"--(Definition of backend frequency response, link to *f_grid*.

The WSV is used to describe the frequency response of backend channels
together with the accompanying WSV *channel2fgrid_weights*.

This WSV links each channel to the elements of *f_grid*. In short it
lists what elements of *f_grid* that are relevant for each channel.

More precisely, the first dimension gives the number of output channels.
Each ArrayOfIndex gives the index of the values in *f_grid* associated
with the channel of concern. For a pure double-sideband receiver, where
there is one monochromatic frequency per passband, this argument could
look like: [[0,5],[1,4],[2,3],[7,8],[7,8]].
)--",
      .type = "ArrayOfArrayOfIndex"};

  wsv_data["channel2fgrid_weights"] = {
      .desc =
          R"--(Definition of backend frequency response, weighting of *f_grid*.

The WSV is used to describe the frequency response of backend channels
together with the accompanying WSV *channel2fgrid_indexes*.

This WSV shall have excatly the same sizes as *channel2fgrid_indexes*.
Each element gives the weight to be assigned to the associated
monochromatic frequency.
)--",
      .type = "ArrayOfVector"};

  wsv_data["cloudbox_checked"] = {
      .desc = R"--(OK-flag for variables associated with the cloudbox.

This variable flags that cloudbox variables are defined in a formally
and practically correct way. For example, that there is sufficient
space between the cloudbox and edges of the model atmosphere (for
2D and 3D). Pure clear-sky variables are covered by
*atmfields_checked* (and *atmgeom_checked*).

Relevant checks are performed by ``cloudbox_checkedCalc``. Only the
value 1 is taken as OK.
)--",
      .type = "Index",
      .default_value = Index{0}};

  wsv_data["cloudbox_field"] = {
      .desc = R"--(The spectral radiance field inside the cloudbx.

This variable is used to store the radiance field inside the cloud
box, probably determined by a scattering solver method.

That is, this variable matches *spectral_radiance_field* but holds
a field that is restricted to the cloud box.

Unit: W / (m^2 Hz sr) for each Stokes component.

Size:
 [ f_grid,
 p_grid, 
 lat_grid, 
 lon_grid, 
 za_grid,
 aa_grid,
 stokes_dim ]

Note: For 1D, the size of the latitude, longitude and azimuth
dimension (N_aa) are all 1.
)--",
      .type = "Tensor7"};

  wsv_data["cloudbox_field_mono"] = {
      .desc = R"--(Monochromatic radiation field inside the cloudbox.

This variable is used to store the monochromatic radiation field 
inside the cloudbox which is found by an iterative solution (DOIT).
Refer to AUG for further information.

Usage: Method output. 

Unit: W / (m^2 Hz sr) for each Stokes component.

Size:
       [(cloudbox_limits[1] - cloudbox_limits[0]) +1, 
       (cloudbox_limits[3] - cloudbox_limits[2]) +1, 
       (cloudbox_limits[5] - cloudbox_limits[4]) +1, 
       N_za, N_aa, N_i ]

Note: For 1D, the size of the azimuth angle dimension (N_aa) is
always 1.
)--",
      .type = "Tensor6"};

  wsv_data["cloudbox_field_mono_old"] = {
      .desc = R"--(As *cloudbox_field_mono* but from previous iteration.

This variable is used to store the intensity field inside the
cloudbox while performing the iteration. One has to store the
intensity field of the previous iteration to be able to do the 
convergence test after each iteration.
Refer to AUG for more information.

Usage: Method output. 

Unit: W / (m^2 Hz sr) for each Stokes component.

Size:
      [(cloudbox_limits[1] - cloudbox_limits[0]) +1, 
      (cloudbox_limits[3] - cloudbox_limits[2]) +1, 
      (cloudbox_limits[5] - cloudbox_limits[4]) +1, 
      N_za, N_aa, N_i ]
)--",
      .type = "Tensor6"};

  wsv_data["cloudbox_limits"] = {.desc = R"--(The limits of the cloud box.

This variable defines the extension of the cloud box. The cloud box 
is defined to be rectangular in the used coordinate system, with 
limits exactly at points of the involved grids. This means, for 
example, that the vertical limits of the cloud box are two pressure 
levels. For 2D, the angular extension of the cloud box is between 
two points of the latitude grid, and likewise for 3D but then also 
with a longitude extension between two grid points. The latitude and
longitude limits for the cloud box cannot be placed at the end 
points of the corresponding grid as it must be possible to calculate
the incoming intensity field.

The variable *cloudbox_limits* is an array of index value with
length twice ``atmosphere_dim``. For each dimension there is a lower 
limit and an upper limit. The order of the dimensions is as usual 
pressure, latitude and longitude. The upper limit index must be 
greater then the lower limit index. For example, 
*cloudbox_limits* = [0 5 4 11 4 11] means that cloud box extends
between pressure levels 0 and 5, and latitude and longitude points 4
and 11.

If *cloudbox_on* = 0, the content of this variable is neglected, but
it must be initiated to some dummy values.

See further the ARTS user guide (AUG). Use the index to find where
this variable is discussed. The variable is listed as a subentry to
\"workspace variables\".

Usage: Set by the user, either directly or using a method
checking the extension of scattering particles.

Unit:  Index values.

Size:  [ 2 * atmosphere_dim ]
)--",
                                 .type = "ArrayOfIndex"};

  wsv_data["cloudbox_on"] = {.desc = R"--(Flag to activate the cloud box.

Scattering calculations are confined to a part of the atmosphere
denoted as the cloud box. The extension of the cloud box is given by
*cloudbox_limits*. This variable tells methods if a cloud box is
activated or not. 

See further the ARTS user guide (AUG). Use the index to find where
this variable is discussed. The variable is listed as a subentry to
\"workspace variables\".

Usage: Set by the user.

Unit:  Boolean.
)--",
                             .type = "Index"};

  wsv_data["complex_refr_index"] = {
      .desc = R"--(Complex refractive index (n) data.

The variable works as a lookup-table of complex refractive index.
The matter type (water, ice ...) is unspecified, it is up to the
user to fill the variable with data for the expected matter.
This variable type can be used to describe n of both the surface and
atmospheric particles. For the surface, a dedicated variable exists:
*surface_complex_refr_index*.

The column dimension has always size 2, where the first and second
column holds the real and imaginary part of n, respectively. The row
dimension matches temperature, and the page dimension is frequency.
Both the temperature and frequency dimensions grids are allowed to
have length 1, which is interpreted as n being constant in that
dimension.

When mapping these data to the required frequencies and temperatures
a bi-linear interpolation is applied.

Unit:       -

Dimensions: 
 - Vector f_grid[N_f]
 - Vector T_grid[N_T]
 - Tensor3 data[N_f][N_T][2]
)--",
      .type = "ComplexGriddedField2"};

  wsv_data["covmat_block"] = {
      .desc =
          R"--(Holds matrices used to set blocks in *covmat_sx* and *covmat_se*.

The matrix contained in this block will be added to the blocks in
in *covmat_sx* and *covmat_se* by the corresponding WSMs. Its dimensions
must agree with gridpoints of the correlated retrieval quantities.

Usage:   Used by the retrievalAdd functions.
)--",
      .type = "Sparse"};

  wsv_data["covmat_inv_block"] = {
      .desc =
          R"--(Holds matrices used to set the inverse blocks in *covmat_sx* and *covmat_se*.

The matrix contained in this block will be used as the inverse of the matrix
contained in covmat_block.

Usage:   Used by the retrievalAdd functions.
)--",
      .type = "Sparse"};

  wsv_data["covmat_se"] = {
      .desc = R"--(Covariance matrix for observation uncertainties.

This matrix (Se) describes the uncertainty of the measurement vector (``y``),
and can be writtenn as::

  Se = Seps + Kb * Sb * Kb'

where Seps describes direct measurement errors (such as thermal noise),
Kb is Jacobian for forward model parameters, and Sb describes the uncertainty
of the forwatrd model parameters.

Usage:   Used by inversion methods.

Dimensions: [ y, y ]
)--",
      .type = "CovarianceMatrix"};

  wsv_data["covmat_sx"] = {
      .desc = R"--(Covariance matrix of a priori distribution

This covariance matrix describes the Gaussian a priori distribution
for an OEM retrieval. It is represented using a symmetric block matrix.
covmat_sx can be used in two ways: Either with a block for each retrieval
quantity or with a single block containing the full covariance matrix.

Using a single block for each retrieval quantity has is advantageous for
if the retrieval quantities are assumed to be independent. In this case,
the covariance blocks can be added separately for each quantity and will
allow optimizing matrix multiplications and inverses required for the OEM
calculation.

The other case of using a single-block covariance matrix is supported
for convenience as well.

Usage:   Used by inversion methods.

Dimensions: 
     [ x, x ]
)--",
      .type = "CovarianceMatrix"};

  wsv_data["covmat_so"] = {
      .desc =
          R"--(Covariance matrix describing the retrieval error due to uncertainties of
the observation system.

That is: So = G * Se * G', where G is the gain matrix (*dxdy*).

Usage: Set by the covmat_soCalc workspace method to characterize the error.
of a successful OEM calculation.

Dimensions:
    [x,x]
)--",
      .type = "Matrix"};

  wsv_data["covmat_ss"] = {
      .desc =
          R"--(Covariance matrix describing the retrieval error due to smoothing.

That is: Ss = (A-I) * Sx * (A-I)', where A is the averaging kernel 
matrix (*avk*).

Usage: Set by the covmat_ssCalc workspace method to characterize the.
errors of a successful OEM calculation.

Dimensions:
    [x,x]
)--",
      .type = "Matrix"};

  wsv_data["depolarization_factor"] = {
      .desc = R"--(Depolarization factor for the scattered gas.

The variable accounts for the anisotropy of the scatterer.
It is the ratio of intensities parallel and perpendicular 
to the plan of scattering. A table of measured values is 
given by Penndorf (1957). Some values are: H2=0.02, N2=0.03
O2=0.06, CO2=0.09 and atmospheric air=0.03.
)--",
      .type = "Numeric"};

  wsv_data["disort_aux"] = {
      .desc =
          R"--(Auxilary data to the output of the cloudbox_fieldDisort-Methods.

Different data beside the direct result of Disort
calculations can be obtained by this variable. These auxilary
data are selected by *disort_aux_vars*.

Usage:      Provided by some radiative transfer methods.

Dimensions: [quantity][ f_grid, number of disort levels/layers ]
)--",
      .type = "ArrayOfMatrix"};

  wsv_data["disort_aux_vars"] = {
      .desc = R"--(Selection of quantities for *disort_aux*.

Each element of this string array determines the quantity for the
corresponding element in *disort_aux* (i.e. the quantities
are stored in the order given in *disort_aux_vars*).

The possible choices vary between the Disort methods. See the WSM you select
)--",
      .type = "ArrayOfString",
      .default_value = ArrayOfString{}};

  wsv_data["dpnd_data_dx"] = {.desc = R"--(Partial derivates of *pnd_data*.

The variable gives the particle derivate of *pnd_data* with respect
to the quantities set in *dpnd_data_dx_names*.

Dimensions: [ n_quantities, n_points, n_scattering_elements ]
)--",
                              .type = "Tensor3"};

  wsv_data["dpnd_data_dx_names"] = {
      .desc = R"--(Selection of partial derivatives of *pnd_data*.

This variable tells an element in *pnd_agenda_array* for which
quantities partial derivatives shall be calculated.

Dimensions: [ n_quantities ]
)--",
      .type = "ArrayOfString"};

  wsv_data["dpnd_field_dx"] = {.desc = R"--(Partial derivatives of *pnd_field*.

The variable gives the particle derivative of *pnd_field* with respect
to scattering species variables included in *jacobian_targets*.

The length of this array shall match the size of *jacobian_targets*.
For retrieval quantities that are not scattering species, the matching
Tensor4 is of no relevance and must be set to be empty.

Dimensions: [n_quantities][ n_scattering_elements, n_p, n_lat, n_lon ]
)--",
                               .type = "ArrayOfTensor4",
                               .default_value = ArrayOfTensor4{}};

  wsv_data["dpropmat_clearsky_dx"] = {.desc = R"--( FIXMEDOC
Partial derivative of absorption coefficients.

This contains the partial derivative of absorption coefficients for
one point in the atmosphere (one set of pressure, temperature, zn
magnetic field, and VMR values) with respect to one of the input
parameters.

Dimension: [ n_quantities ] [naa, nza, nf, f(stokes_dim)]

*jacobian_targets* should be used to set the input variable for
partial derivation

Unit: 1/m/jacobian_targets
)--",
                                      .type = "PropmatMatrix"};

  wsv_data["dpsd_data_dx"] = {.desc = R"--(Partial derivates of *psd_data*.

The variable gives the particle derivate of *psd_data* with respect
to the quantities set in *dpnd_data_dx_names*.

Dimensions: [ n_quantities, n_points, n_scattering_elements ]
)--",
                              .type = "Tensor3"};

  wsv_data["dnlte_source_dx"] = {
      .desc =
          R"--(NLTE partial derivatives output is two parts:  S * dB/dx + dS/dx * B.

Dimensions: [ quantities ] [nza, naa, nf, stokes_dim] or [0]

Unit: 1/m/jacobian_targets
)--",
      .type = "StokvecMatrix"};

  wsv_data["doit_conv_flag"] = {.desc = R"--(Flag for the convergence test.

This variable is initialized with 0 inside the method 
``cloudbox_field_monoIterate``.
If after an iteration the convergence test is fulfilled, 1 is 
assigned which means that the iteration is completed. 

Usage: Method output.
)--",
                                .type = "Index"};

  wsv_data["doit_is_initialized"] = {
      .desc = R"--(Flag to determine if *DoitInit* was called.

This flag is checked by *DoitCalc* to make sure that
*DoitInit* was called before.
)--",
      .type = "Index"};

  wsv_data["doit_iteration_counter"] = {
      .desc = R"--(Counter for number of iterations.

This variable holds the number of iterations 
while solving the VRTE using the DOIT method.
)--",
      .type = "Index"};

  wsv_data["doit_scat_field"] = {
      .desc = R"--(Scattered field inside the cloudbox.

This variable holds the value of the scattering integral for all
points inside the cloudbox. For more information refer to AUG.

Usage: Input to ``cloudbox_fieldUpdate...``. 

Unit: W / (m^2 Hz sr) for each Stokes component.

Size:
     [(cloudbox_limits[1] - cloudbox_limits[0]) +1, 
     (cloudbox_limits[3] - cloudbox_limits[2]) +1, 
     (cloudbox_limits[5] - cloudbox_limits[4]) +1, 
     N_za, N_aa, N_i ]
)--",
      .type = "Tensor6"};

  wsv_data["doit_za_grid_opt"] = {.desc = R"--(Optimized zenith angle grid.

Output of the method *doit_za_grid_optCalc*.

Usage:   Output of *doit_za_grid_optCalc*

Unit:    degrees
)--",
                                  .type = "Vector"};

  wsv_data["doit_za_grid_size"] = {
      .desc = R"--(Number of equidistant grid points of the zenith angle grid.

Grid points are defined from 0 to 180 deg, for the scattering
integral calculation.

Usage: Output of *DOAngularGridsSet*.
)--",
      .type = "Index"};

  wsv_data["doit_za_interp"] = {
      .desc = R"--(Flag for interplation method in zenith angle dimension.

 - 0 - linear interpolation 
 - 1 - cubic interpolation 

Usage: Set by user in *doit_za_interpSet*.
)--",
      .type = "Index"};

  wsv_data["dsurface_emission_dx"] = {
      .desc =
          R"--(The derivative of *surface_emission* with respect to quantities
listed in *dsurface_names*.

Usage: Used internally of radiative transfer methods

Dimensions: [dsurface_names][f_grid, stokes_dim]
)--",
      .type = "ArrayOfMatrix"};

  wsv_data["dsurface_names"] = {
      .desc = R"--(Name of surface retrieval quantities.

Usage: Used internally of radiative transfer methods

Dimensions: [retrieval quantity]
)--",
      .type = "ArrayOfString"};

  wsv_data["dsurface_rmatrix_dx"] = {
      .desc =
          R"--(The derivative of *surface_rmatrix* with respect to quantities
listed in *dsurface_names*.

Usage: Used internally of radiative transfer methods

Dimensions: [dsurface_names][surface_los, f_grid, stokes_dim, stokes_dim]
)--",
      .type = "ArrayOfTensor4"};

  wsv_data["dxdy"] = {.desc = R"--(Contribution function (or gain) matrix.

This matrix is the partial derivative of the retrieved state vector
with respect to the measurement vector (``y``).

Usage: Used and set by inversion methods.
)--",
                      .type = "Matrix"};

  wsv_data["ecs_data"] = {.desc = R"--(Error corrected sudden data

Dimensions: [num IDs] [num Species]

It is used for absorption bands with these population tags:
 - ByMakarovFullRelmat 
 - ByRovibLinearDipoleLineMixing
)--",
                          .type = "MapOfErrorCorrectedSuddenData"};

  wsv_data["ecs_data2"] = {.desc = R"--(Error corrected sudden data

Dimensions: [num Isotopologues] [num Species]

Used in line-by-line calculations requiring ECS data.
)--",
                           .type = "LinemixingEcsData",
                           .default_value = LinemixingEcsData{}};

  wsv_data["ext_mat"] = {.desc = R"--(Total extinction matrix.

This variable contains the extinction coefficient matrix, which
is used in the RT calculation in the cloudbox. It is the physical
extinction matrix which includes particle extinction for all chosen
scattering species and gaseous extinction for all chosen gaseous species.

See the ARTS user guide (AUG) for further information. Use the index to
find where this variable is discussed. The variable is listed as a
subentry to \"workspace variables\".

Usage:      Output of *opt_prop_bulkCalc* 

Unit:       m^2 FIXME: really m2? not 1/m?

Dimensions: [f_grid, stokes_dim, stokes_dim]
)--",
                         .type = "PropmatVector"};

  wsv_data["ext_mat_spt"] = {
      .desc = R"--(Extinction matrix for all individual scattering elements.

This variable contains the elements of the extinction matrix of all
individual scattering elements for a given propagation direction. It is
calculated input as well as the output of the agenda *spt_calc_agenda*.

Usage:      Output of *spt_calc_agenda* 

Unit:       m^2 FIXME: really m2? not 1/m?

Dimensions: [number of scattering elements, stokes_dim, stokes_dim]
)--",
      .type = "ArrayOfPropmatVector"};

  wsv_data["file_index"] = {.desc = R"--(Index number for files.

See *WriteXMLIndexed* for further information.

Usage:   Input to *WriteXMLIndexed* and *ReadXMLIndexed*.
)--",
                            .type = "Index"};

  wsv_data["f_backend"] = {
      .desc =
          R"--(The frequency position of each backend (spectrometer) channel.

Usage: Set by the user. 

Unit:  Hz
)--",
      .type = "Vector"};

  wsv_data["f_backend_multi"] = {
      .desc = R"--(As *f_backend* but describes an instrument with muliple
mixer/receiver chains.

This variable is needed when e.g. the receiver has several mixers
or the the receiver measures several polarisation and the channels
differ in position or response function. 

The array has one element for each \"receiver chain\". The array
length must match *backend_channel_response_multi*, and possibly
also *lo_multi*.

Usage: Set by the user. 

Unit:  Hz
)--",
      .type = "ArrayOfVector"};

  wsv_data["f_grid"] = {
      .desc =
          R"--(The frequency grid for monochromatic pencil beam calculations.

Usage: Set by the user. 

Unit:  Hz
)--",
      .type = "Vector"};

  wsv_data["f_index"] = {.desc = R"--(Frequency index.

Not all methods handle all monochromatic frequencies (of *f_grid*) in
parellel and this variable is used for communication between methods,
holding the index of the frequency treated presently.

In some contexts, a negative f_index means all frequencies.

Usage: Method output.
)--",
                         .type = "Index"};

  wsv_data["gas_scattering_do"] = {.desc = R"--(Flag to activate gas scattering.
)--",
                                   .type = "Index",
                                   .default_value = Index{0}};

  wsv_data["gas_scattering_output_type"] = {
      .desc = R"--(Flag to select the output type of gas scattering.

Internal communications variable, not intended to be used by user.
If equals 0 *gas_scattering_mat* is output and *gas_scattering_fct_legendre* is empty.
If equals 1 *gas_scattering_fct_legendre* is output and *gas_scattering_mat* is empty.

)--",
      .type = "Index"};

  wsv_data["gas_scattering_coef"] = {
      .desc = R"--(Spectrum of scattering coefficient matrices.

This variable contains the elements of the extinction matrix solely
due to scattering.

Units: [ m^-1. ]

Size:  [fgrid, stokes_dim, stokes_dim]
)--",
      .type = "PropmatVector"};

  wsv_data["gas_scattering_mat"] = {
      .desc = R"--(Spectrum of normalized phase matrices.

This variable contains the elements of the normalized phase matrix
for a specific incoming and outgoing direction.

Units: [ 1 ]

Size:  [fgrid, stokes_dim, stokes_dim]
)--",
      .type = "MuelmatVector"};

  wsv_data["gas_scattering_fct_legendre"] = {
      .desc = R"--(Normalized phase function as Legendre series.

This variable contains the normalized phase function
as Legendre series.

Units: [ 1 ]

Size:  [Number of Legendre polynomials]
)--",
      .type = "Vector"};

  wsv_data["g0"] = {.desc = R"--(Gravity at zero altitude.

This variable is \"little g\" at the reference ellipsiod. That is,
for Earth this is a value around 9.81 m/s2
)--",
                    .type = "Numeric"};

  wsv_data["heating_rates"] = {
      .desc = R"--(The heating rates of atmospheric layers.

The heating rate is defined as the rate of temperature change of an 
atmospheric layer due to heating by absorption of radiation or if it
is negative due to loss of energy by emission of radiation.

Units: K s^-1

Size: [ p_grid, lat_grid, lon_grid ]
)--",
      .type = "Tensor3"};

  wsv_data["xsec_fit_data"] = {
      .desc = R"--(Fitting model coefficients for cross section species.

Dimensions: [ n_species ]

XsecRecord:

- species: Name of species
- version: Fit model version
- fitcoeffs:

  -  Fit model coefficients as an *ArrayOfGriddedField2*
  -  Dimensions: [ n_bands ]

    -   GriddedField2: [ n_band_frequencies, n_coeffs ]

      -    The fit model:

        -     z = p00 + p10*x + p01*y + p20*x^2
        -     z = Xsec [m^2]
        -     x = T / T0
        -     y = P / P0
        -     T0 = 1 [K]
        -     P0 = 1 [Pa]
        -     fitcoeffs(:, 0)           p00  [m^2]
        -     fitcoeffs(:, 1)           p10  [m^2]
        -     fitcoeffs(:, 2)           p01  [m^2]
        -     fitcoeffs(:, 3)           p20  [m^2]

- fitminpressures:

  -  Minimum pressure available in source xsec data to generate the fit coefficients.
  -  Dimensions: [ n_bands ]

- fitmaxpressures:

  -  Maximum pressure available in source xsec data to generate the fit coefficients.
  -  Dimensions: [ n_bands ]

- fitmintemperatures:

  -  Minimum temperature available in source xsec data to generate the fit coefficients.
  -  Dimensions: [ n_bands ]

- fitmintemperatures:

  -  Maximum temperature available in source xsec data to generate the fit coefficients.
  -  Dimensions: [ n_bands ]

fitminpressures, fitmaxpressures, fitmintemperatures and fitmaxtemperatures
are not used to apply the model and solely serve for informational purposes.
)--",
      .type = "ArrayOfXsecRecord"};

  wsv_data["instrument_pol"] = {
      .desc = R"--(Definition of the polarisation of an instrument.

The default for output is to give data for the selected Stokes
elements (1:stokes_dim). This variable defines the polarisations
that are actually measured, or are transmitted.

The polarisation states/components are coded as
  (0) Undefined.
  (1) I, total intensity.
  (2) Q, second Stokes component, Iv - Ih.
  (3) U, third Stokes component, I+45 - I-45.
  (4) V, forth Stokes component, Irc - Ilc
  (5) Iv, intensity of vertically polarised component.
  (6) Ih, intensity of horizontally polarised component.
  (7) I+45, intensity of +45 deg linearly polarised component.
  (8) I-45, intensity of -45 deg linearly polarised component.
  (9) Ilhc, intensity of left-hand circularly polarised component.
  (10) Irhc, intensity of right-hand circularly polarised component.

See the documentation for definition of the Stokes vector and the
different components.

If the instrument measures, or transmits, vertical and horizontal
components, this variable shall accordingly be set to [5,6].

Conversion to Planck-BT of components 2-4 requires that component
1 is kept, and must be included as first element.

The shift from the Stokes vector can be made at any stage when of the
sensor response set-up. The responses used must of course be adopted
correspondingly. Or reversed, if the antenna response is defined for
Iv or Ih it could be useful to shift polarisation as first sensor
operation.

Usage: Set by the user.
)--",
      .type = "ArrayOfIndex"};

  wsv_data["instrument_pol_array"] = {
      .desc = R"--(Multiple definition of instrument polarisation.

Defined as *instrument_pol* but used when multiple polarisations
are possible/required.

Usage: Set by the user.
)--",
      .type = "ArrayOfArrayOfIndex"};

  wsv_data["irradiance_field"] = {
      .desc = R"--(Irradiance field also known as flux density.

Radiant flux received by a surface per unit area for each hemisphere.
The last dimension denotes the hemispheres. The first component is
the downward irradiance and the second component is the upward irradiance

Units: W m^-2

Size: [ p_grid,  lat_grid,  lon_grid,  2 ]
)--",
      .type = "Tensor4"};

  wsv_data["jacobian"] = {.desc = R"--(The Jacobian matrix.

The matrix holding the Jacobians of the retrieval quantities. The
matrix has to be initialised before the retrieval quantities can be
defined. Initialisation is done by ``jacobianInit``. Retrieval quantities
are then added with ``jacobianAdd...`` or ``retrievalAdd...`` methods.

The order between rows and columns follows how data are stored in ``y``
and *x*, respectively.

Units:   See the different retrieval quantities.

Dimension: [ y, number of retrieval quantities and grids ]
)--",
                          .type = "Matrix"};

  wsv_data["jacobian_do"] = {
      .desc = R"--(Flag to activate (clear-sky) Jacobian calculations.

If this variable is set to 0, no Jacobian calculations will be done,
even if such calculations have been set-up (through the
jacobianAddXxx methods).

Needs to be 0 if cloudy-sky (Doit) Jacobians shall be calculated.
)--",
      .type = "Index"};

  wsv_data["lat"] = {.desc = R"--(A latitude.

Unit:  degrees
)--",
                     .type = "Numeric"};

  wsv_data["lbl_checked"] = {
      .desc = R"--(Flag to check if the line-by-line calculations will work

Usage: Set manually on own risk, or use *lbl_checkedCalc*.

Unit:  Boolean
)--",
      .type = "Index",
      .default_value = Index{0}};

  wsv_data["line_irradiance"] = {
      .desc = R"--(Irradiance as seen by a single absorption line.

Used internally for, e.g., NLTE effects
)--",
      .type = "Matrix"};

  wsv_data["line_transmission"] = {
      .desc = R"--(Transmission as seen by a single absorption line.

Used internally for, e.g., NLTE effects
)--",
      .type = "Tensor3"};

  wsv_data["lo"] = {.desc = R"--(The local oscillator frequency.

A local oscillator frequency is used in a heterodyne system when
the mixer folds the spectra from from radio frequencies (RF) to
intermediate frequencies (IF).

Unit:  Hz

Usage: Set by the user.
)--",
                    .type = "Numeric"};

  wsv_data["lo_multi"] = {.desc = R"--(Local oscillator frequencies.

As *lo* but describes an instrument with multiple mixers. A vector
element for each LO. The size of this variable and
*sideband_response_multi* shall match, and probably also
*sideband_mode_multi*.

Unit:  Hz

Usage: Set by the user.
)--",
                          .type = "Vector"};

  wsv_data["lon"] = {.desc = R"--(A longitude.

Unit:  degrees
)--",
                     .type = "Numeric"};

  wsv_data["mblock_index"] = {.desc = R"--(Measurement block index. 

Used to tell agendas the index of present measurement block.

Usage: Used internally.
)--",
                              .type = "Index"};

  wsv_data["mc_antenna"] = {
      .desc = R"--(Antenna pattern description for dedicated MC calculaions.

Usage: Input to MCGeneral. Set by *mc_antennaSetGaussian* and similar methods.
)--",
      .type = "MCAntenna"};

  wsv_data["mc_error"] = {
      .desc = R"--(Error in simulated ``y`` when using a Monte Carlo approach.

Usage: Output from Monte Carlo functions. 

Size:  [ stokes_dim ]
)--",
      .type = "Vector"};

  wsv_data["mc_iteration_count"] = {
      .desc =
          R"--(Counts the number of iterations (or photons) used in the MC
scattering algorithm.

Usage: Set by MCGeneral and other MC methods.
)--",
      .type = "Index"};

  wsv_data["mc_max_iter"] = {
      .desc = R"--(The maximum number of iterations allowed for Monte Carlo
calculations.

Usage: Set by the user.
)--",
      .type = "Index"};

  wsv_data["mc_max_scatorder"] = {
      .desc = R"--(The maximum scattering order allowed for Monte Carlo
radar calculations.

Usage: Set by the user.
)--",
      .type = "Index"};

  wsv_data["mc_max_time"] = {
      .desc = R"--(The maximum time allowed for Monte Carlo calculations.

Usage: Set by the user.

Unit: s
)--",
      .type = "Index"};

  wsv_data["mc_min_iter"] = {
      .desc = R"--(The minimum number of iterations allowed for Monte Carlo
calculations.

Usage: Set by the user.
)--",
      .type = "Index",
      .default_value = Index{100}};

  wsv_data["mc_points"] = {.desc = R"--(FIXMEDOC
Source to emission, position.

Counts the number of MC endpoints in each grid cell.

Usage: Set by MCGeneral and other MC methods.
)--",
                           .type = "Tensor3"};

  wsv_data["mc_scat_order"] = {
      .desc =
          R"--(Number of atmospheric scattering events between emission point and sensor.

The first element gives the number of cases with zero scattering events,
the second the number of single scattering cases etc.

Scattering orders above what the variable can hold are not stored at all.
The number of such cases can be determined by comparing
*mc_iteration_count* with the sum of the elements in this array.

Usage: Set by MCGeneral and other MC methods.
)--",
      .type = "ArrayOfIndex"};

  wsv_data["mc_source_domain"] = {
      .desc = R"--(Rough classification of source to emission.

This is an array of length 4, where the elements in order represent
space, the surface, atmospheric gas and atmospheric particle.
The distinction between the two last elements is if the emission
is associated with ``vmr_field`` or *pnd_field*.

The values of the array give the number of cases where the emission
source was found to be inside each \"domain\".

Usage: Set by MCGeneral and other MC methods.
)--",
      .type = "ArrayOfIndex"};

  wsv_data["mc_seed"] = {
      .desc = R"--(The integer seed for the random number generator used by
Monte Carlo methods.

Usage: Set by MCSetSeed.
)--",
      .type = "Index"};

  wsv_data["mc_std_err"] = {
      .desc = R"--(Target precision (1 std. dev.) for Monte Carlo calculations.

Usage: Set by the user.
)--",
      .type = "Numeric"};

  wsv_data["mc_y_tx"] = {
      .desc = R"--(Normalized Stokes vector for transmittance (e.g., radar).

The first element (intensity) should have a value of 1.

Usage: Set by user. 

Units: Unitless.

Size:  [ stokes_dim ]
)--",
      .type = "Vector"};

  wsv_data["mc_taustep_limit"] = {
      .desc =
          R"--(Defines an upper step length in terms of optical thickness for Monte 
Carlo calculations.

Usage: Set by the user.
)--",
      .type = "Numeric",
      .default_value = Numeric{0.1}};

  wsv_data["met_amsu_data"] = {.desc = R"--(The AMSU data set.

This is intended as input for the method ybatchMetProfiles. It holds the
latitude, longitude, satellite zenith angle and amsu-b corrected and 
uncorrected brightness temperatures.  It also has information about 
the particular pixel corresponds to a land or sea point.  This will be 
read in the method ybatchMetProfiles and the profiles corresponding to 
each latitude and longitude will be read in.

See documentation of WSM ``ybatchMetProfiles`` for more information.
)--",
                               .type = "Matrix"};

  wsv_data["met_mm_antenna"] = {
      .desc =
          R"--(The antenna beam width for meteorological millimeter instruments.

This Vector must match the number and order of channels in
*met_mm_backend*.

Usage: Set by the user.

Unit:  [ Hz ]

Size:  [ number of channels ]
)--",
      .type = "Vector"};

  wsv_data["met_mm_backend"] = {
      .desc =
          R"--(Backend description for meteorological millimeter sensors with passbands.

This is a compact description of a passband-type sensor, e.g. AMSU-A. The matrix
contains one row for each instrument channel. Each row contains four elements:

 - LO position [Hz]
 - first offset from the LO [Hz]
 - second offset from the LO+offset1 [Hz]
 - channel width [Hz]

Overview::

                            LO
                             |
                 offset1     |    offset1
             ----------------+----------------
             |                               |
             |                               |
    offset2  |  offset2             offset2  |  offset2
    ---------+---------             ---------+---------
    |                 |             |                 |
    |                 |             |                 |
  #####             #####         #####             #####
  width             width         width             width

For a sensor with 1 passband, offset1 and offset2 are zero.
For a sensor with 2 passbands, only offset2 is zero.

Usage: Set by the user.

Unit: All entries in Hz.

Size: [number of channels, 4]
)--",
      .type = "Matrix"};

  wsv_data["met_mm_polarisation"] = {
      .desc =
          R"--(The polarisation for meteorological millimeter instruments.

This array must match the number and order of channels in
*met_mm_backend*.

Possible values:

- ``\"V\"``: Vertical polarisation
- ``\"H\"``: Horizontal polarisation
- ``\"LHC\"``: Left-hand circular polarisation
- ``\"RHC\"``: Right-hand circular polarisation
- ``\"AMSU-V\"``: Vertical polarisation dependening on AMSU zenith angle
- ``\"AMSU-H\"``: Horizontal polarisation dependening on AMSU zenith angle
- ``\"ISMAR-V\"``: Vertical polarisation dependening on ISMAR zenith angle
- ``\"ISMAR-H\"``: Horizontal polarisation dependening on AMSU zenith angle

Usage: Set by the user.

Unit:  [ String ]

Size:  [ number of channels ]
)--",
      .type = "ArrayOfString"};

  wsv_data["level0_data"] = {.desc = R"--(List of L0 data.  Can be of any type.
It is method-dependent how this is used to calibrated to L1
)--",
                             .type = "ArrayOfVector"};

  wsv_data["level0_time"] = {.desc = R"--(List of L0 times.  Should be in UTC.
It is method-dependent how this is used to calibrated to L1
)--",
                             .type = "ArrayOfTime"};

  wsv_data["lm_ga_history"] = {
      .desc =
          R"--(The series of gamma values for a Marquardt-levenberg inversion.

The values are stored following iteration order, i.e. the first
is the gamma factor for the first iteration etc.
)--",
      .type = "Vector"};

  wsv_data["molarmass_dry_air"] = {
      .desc = R"--(The average molar mass of dry air.

This could also be referred to as the average molecular weight for
dry air. The definition of \"dry air\" can differ between planets and
methods using the WSV. For Earth, this should be a value around
28.97.
)--",
      .type = "Numeric"};

  wsv_data["nlte_vib_energies"] = {.desc = R"--(A map of energy levels

This map is used when required by an absorption band having a
population distribution that depends on vibrational temperatures
)--",
                                   .type = "VibrationalEnergyLevels",
                                   .default_value = VibrationalEnergyLevels{}};

  wsv_data["collision_line_identifiers"] = {.desc = R"--(FIXMEDOC
An array of quantum identifiers for finding collisional rates
in *collision_coefficients*
)--",
                                            .type = "ArrayOfQuantumIdentifier"};

  wsv_data["collision_coefficients"] = {.desc = R"--(FIXMEDOC
An array of coefficients for effective collisions
)--",
                                        .type = "ArrayOfArrayOfGriddedField1"};

  wsv_data["nelem"] = {.desc = R"--(Number of elements of a Vector or Array.
)--",
                       .type = "Index"};

  wsv_data["ncols"] = {
      .desc =
          R"--(Number of columns (elements in lowest dimension) of a Matrix or Tensor.
)--",
      .type = "Index"};

  wsv_data["nrows"] = {
      .desc =
          R"--(Number of rows (elements in 2nd lowest dimension) of a Matrix or Tensor.
)--",
      .type = "Index"};

  wsv_data["npages"] = {
      .desc = R"--(Number of elements in 3rd lowest dimension of a Tensor.
)--",
      .type = "Index"};

  wsv_data["nbooks"] = {
      .desc = R"--(Number of elements in 4th lowest dimension of a Tensor.
)--",
      .type = "Index"};

  wsv_data["nshelves"] = {
      .desc = R"--(Number of elements in 5th lowest dimension of a Tensor.
)--",
      .type = "Index"};

  wsv_data["nvitrines"] = {
      .desc = R"--(Number of elements in 6th lowest dimension of a Tensor.
)--",
      .type = "Index"};

  wsv_data["nlibraries"] = {
      .desc = R"--(Number of elements in 7th lowest dimension of a Tensor.
)--",
      .type = "Index"};

  wsv_data["nlte_do"] = {.desc = R"--(Flag to perform Non-LTE calculations.
)--",
                         .type = "Index",
                         .default_value = Index{0}};

  wsv_data["nlte_source"] = {
      .desc =
          R"--(Variable to contain the additional source function due to NLTE effects.

Dimensions: [nza, naa, nf, stokes_dim]
)--",
      .type = "StokvecVector"};

  wsv_data["oem_diagnostics"] = {
      .desc = R"--(Basic diagnostics of an OEM type inversion.

This is a vector of length 5, having the elements (0-based index):
  0. Convergence status, with coding
       - 0 = converged
       - 1 = max iterations reached
       - 2 = max gamma of LM reached
       - 9 = some error when calling ``inversion_iterate_agenda``
       - 99 = too high start cost.
  1. Start value of cost function.
  2. End value of cost function.
  3. End value of y-part of cost function.
  4. Number of iterations used.

See WSM ``OEM`` for a definition of \"cost\". Values not calculated
are set to NaN.
)--",
      .type = "Vector"};
  wsv_data["oem_errors"] = {
      .desc = R"--(Errors encountered during OEM execution.
)--",
      .type = "ArrayOfString"};

  wsv_data["output_file_format"] = {.desc = R"--(Output file format.

This variable sets the format for output files. It could be set to:

- \"ascii\": for plain xml files
- \"zascii\": for zipped xml files
- \"binary\": for binary.

To change the value of this variable use the workspace methods
*output_file_formatSetAscii*, *output_file_formatSetZippedAscii*, and
*output_file_formatSetBinary*
)--",
                                    .type = "String",
                                    .default_value = String{"ascii"}};

  wsv_data["particle_bulkprop_names"] = {
      .desc = R"--(Identification of the data in ``particle_bulkprop_field``.

This variable assigns a name to each field in ``particle_bulkprop_field``.
The naming is totally free. If two fields are given the same name, the
first one will be selected.

Dimensions: length should match book-dimension of ``particle_bulkprop_field``
)--",
      .type = "ArrayOfString",
      .default_value = ArrayOfString{}};

  wsv_data["particle_masses"] = {
      .desc = R"--(The mass of individual particles (or bulks).

Each row corresponds to a scattering element (i.e. an element in
*scat_data*). The user is free to define different mass
categories and assign a mass for each category. Each column
of *particle_masses* corresponds to such a mass category. A scattering
element can have a non-zero mass for more than one category.

For example, if you work with clouds, your mass categories could
be ice and liquid, corresponding to IWC and LWC, respectively.
The mass of particles inside the melting layer, having a mixed
phase, could be divided between the two columns of the matrix.

Shall either be empty, or have a row size consistent with the
scattering variables (*scat_data*, *pnd_field*).

Usage:      Set by the user.

Unit:       kg

Dimensions: [number of scattering elements, number of mass categories]
)--",
      .type = "Matrix",
      .default_value = Matrix{}};

  wsv_data["pha_mat"] = {.desc = R"--(Ensemble averaged phase matrix.

This workspace variable represents the actual physical phase
matrix (averaged over all scattering elements) for given propagation
directions. It is calculated in the method *pha_matCalc*.

See ARTS user guide (AUG) for further information. Use the index to find
where this variable is discussed. The variable is listed as a subentry
to \"workspace variables\".

Usage:      Output of the method *pha_matCalc*

Unit:        m^2 FIXME: really m2? not 1/m?

Dimensions: [za_grid, aa_grid, stokes_dim, stokes_dim]
)--",
                         .type = "Tensor4"};

  wsv_data["pha_mat_doit"] = {
      .desc = R"--(Ensemble averaged phase matrix for DOIT calculation.

This workspace variable represents the actual physical phase
matrix (averaged over all scattering elements) for given incident and 
propagation directions. It is calculated in the method ``DoitScatteringDataPrepare``.

See ARTS user guide (AUG) for further information.

Usage:      Output of the method *pha_matCalc*

Unit:        m^2 FIXME: really m2? not 1/m?

Dimensions: [T,za_grid, aa_grid, za_grid, aa_grid, stokes_dim, stokes_dim]
)--",
      .type = "Tensor7"};

  wsv_data["pha_mat_spt"] = {
      .desc = R"--(Phase matrix for all individual scattering elements.

This variable contains the elements of phase matrix for all individual
scattering elements for given propagation directions. It is the
calculated in the agenda *pha_mat_spt_agenda*. The elements of the phase
matrix are calculated from the single scattering data.

See ARTS user guide (AUG) for further information.

Usage:      Input and Output of the pha_mat_sptFrom* methods

Unit:       m^2 FIXME: really m2? not 1/m?

Dimensions: [number of scattering elements, za_grid, aa_grid, stokes_dim, stokes_dim]
)--",
      .type = "Tensor5"};

  wsv_data["pha_mat_sptDOITOpt"] = {.desc = R"--(Interpolated phase matrix.

This variable contains the data of the phase matrix in the 
scattering frame interpolated on the actual frequency (the variable
is used inside *doit_mono_agenda*) and also interpolated on all 
possible scattering angles following from all combinations of 
*za_grid* and *aa_grid*. 

Usage:      Input of the method *pha_mat_sptFromDataDOITOpt*

Unit:        m^2 FIXME: really m2? not 1/m?

Dimensions: 

- [number of scattering elements]

  - [T, za_grid, aa_grid, za_grid, aa_grid, stokes_dim, stokes_dim]
)--",
                                    .type = "ArrayOfTensor7"};

  wsv_data["planet_rotation_period"] = {
      .desc = R"--(The sidereal rotation period of the planet.

This is time that it takes for the planet to complete one revolution
around its axis of rotation relative to the suns. For Earth, this
is a value roughly 4 min less than 24 h.

A negative value signifies a retrograde rotation, i.e. opposite to
the rotation of Earth.

Unit:   s
)--",
      .type = "Numeric"};

  wsv_data["pnd_agenda_input"] = {
      .desc = R"--(The variable input to one element of *pnd_agenda_array*.

The column dimension corresponds to the input to the underlying
particle size distribution method. For example, the first column
can hold ice water content values, and the second one temperature
data.

Temperatures are handled by *pnd_agenda_input_t* and shall not be
included in this variable.

Each row corresponds to a position. That is, the methods in the
pnd-agendas are expected to process multiple points in one call.

Dimensions: [ n_points, n_input_variables ]
)--",
      .type = "Matrix"};

  wsv_data["pnd_agenda_input_t"] = {
      .desc = R"--(Temperature input to one element of *pnd_agenda_array*.

This WSV works as *pnd_agenda_input* but holds a specific quantity,
temperature.

Each element corresponds to a position. That is, the methods in the
pnd-agendas are expected to process multiple points in one call.

Dimensions: [ n_points ]
)--",
      .type = "Vector"};

  wsv_data["pnd_agenda_array_input_names"] = {
      .desc = R"--(Naming of all input expected by *pnd_agenda_array*.

This variable contains *pnd_agenda_input_names* for each agenda
element in *pnd_agenda_array*.

Dimension: [ n_scattering_species ][ n_input_variables ]
)--",
      .type = "ArrayOfArrayOfString"};

  wsv_data["pnd_agenda_input_names"] = {
      .desc = R"--(Naming of (existing or expected) data in *pnd_agenda_input*.

The strings of this variable refer to the corresponding column in
*pnd_agenda_input*.

Dimension: [ n_input_variables ]
)--",
      .type = "ArrayOfString"};

  wsv_data["pnd_data"] = {
      .desc = R"--(Particle number density values for a set of points.

The variable contains particle number density data for one scattering
species. The row dimension corresponds to different positions, in the
same way as *pnd_agenda_input* is defined.

Dimensions: [ n_points, n_scattering_elements ]
)--",
      .type = "Matrix"};

  wsv_data["pnd_field"] = {.desc = R"--(Particle number density field.

This variable holds the particle number density fields for all
scattering elements being read in the WSMs
*ScatElementsPndAndScatAdd* or *ScatSpeciesPndAndScatAdd* and
interpolated to the calculation grids ``p_grid``, ``lat_grid``, and
``lon_grid`` inside the cloudbox. An alternative method to create
*pnd_field* is ``pnd_fieldCalcFromParticleBulkProps``.

Total number and order of scattering elements in *pnd_field* and (the
flattened) *scat_data* has to be identical.

Note: To ensure that no particles exist outside the cloudbox,
*pnd_field* is required to be 0 at its outer limits (corresponding
to the *cloudbox_limits*).

Usage:      Set by user or output of ``pnd_fieldCalcFromParticleBulkProps``

Unit:        m^-3

Size:
 [number of scattering elements, 
 ( *cloudbox_limits* [1] - *cloudbox_limits* [0]) +1, 
 ( *cloudbox_limits* [3] - *cloudbox_limits* [2]) +1, 
 ( *cloudbox_limits* [5] - *cloudbox_limits* [4]) +1 ]
)--",
                           .type = "Tensor4"};

  wsv_data["pnd_size_grid"] = {
      .desc = R"--(The particle sizes associated with *pnd_data*.

This variable holds the size of each scattering element considered.
Size can be defined differently, depending on particle size distribution
used. Most common choices should by equivalent diameter, maximum diameter
and mass.

Dimension: [ n_sizes ]
)--",
      .type = "Vector"};

  wsv_data["pnd_field_raw"] = {
      .desc = R"--(The particle number density field raw data.

This variable contains the particle number density data for all
considered scattering elements. *pnd_field_raw* is an Array of
GriddedField3. It includes one GriddedField3 for each scattering
element, which contains both the data and the corresponding grids.

Usage:
  Set by the user. Input to methods *ScatElementsPndAndScatAdd* and 
  *ScatSpeciesPndAndScatAdd*

Unit:  m^-3

Size:

- Array[number of scattering elementst]

  - GriddedField3
  - [number of pressure levels]
  - [number of latitudes]
  - [number of longitudes]
  - [number of pressure levels, number of latitudes, number of longitudes]
)--",
      .type = "ArrayOfGriddedField3"};

  wsv_data["ppath_field"] = {
      .desc =
          R"--(An array meant to build up the necessary geometries for radiative
field calculations.

Can be ordered or not

Size: user-defined
)--",
      .type = "ArrayOfPpath"};

  wsv_data["ppath_inside_cloudbox_do"] = {
      .desc = R"--(Flag to perform ray tracing inside the cloudbox.

Standard propagation path calculations stop at the boundary of the
cloudbox, or stop directly if started inside the cloudbox. This WSV
allows scattering methods to obtain propagation paths inside the
cloudbox. Hence, this variable is for internal usage primarily.

Usage: For communication between modules of arts.
)--",
      .type = "Index",
      .default_value = Index{0}};

  wsv_data["ppath_lmax"] = {
      .desc = R"--(Maximum length between points describing propagation paths.
The exact spacing of point in propagation paths can vary between
calculation options, but it is guaranteed to not exceed the value
of *ppath_lmax*.

Usage: Ppath methods such as ``ppath_stepGeometric``.
)--",
      .type = "Numeric",
      .default_value = Numeric{10e3}};

  wsv_data["ppath_lraytrace"] = {
      .desc =
          R"--(Ray tracing step length when determining refracted propagation paths.

The variable sets an upper limit for the length of each ray tracing
step. Workspace methods can apply a somewhat smaller value to split
*ppath_lstep* in sections of equal length.

Unit:  m
)--",
      .type = "Numeric",
      .default_value = Numeric{1e2}};

  wsv_data["ppath_lstep"] = {
      .desc = R"--(Length between points describing propagation paths.

The variable sets an upper limit for the length between the points.
step. Workspace methods can apply a somewhat smaller value to split
the total propagation path in sections of equal length.

Unit:  m
)--",
      .type = "Numeric"};

  wsv_data["ppath_ltotal"] = {
      .desc = R"--(Sets a maximum length of propagation paths.

Default is to calculate propagation paths until the surface or the
top of the atmosphere is reached. If *ppath_ltotal* is set to be >0,
then the selected value gives a third constrain for ending paths.
If reached, the propagation path ends exactly at the maximum length.

This criterion refers to the length of the propagation path inside
the atmosphere.

Unit:  m
)--",
      .type = "Numeric",
      .default_value = Numeric{-1}};

  wsv_data["ppvar_atm"] = {
      .desc = R"--(Atmospheric points along the propagation path.

ppvar stands for propagation path variable. The variables named in is
way describe the atmosphere and its properties at each point of the
propagation path

See *atm_point* for information about atmospheric points

Dimension: [ ppath.np ]

Usage: Output of radiative transfer methods.
)--",
      .type = "ArrayOfAtmPoint"};

  wsv_data["ppvar_f"] = {
      .desc = R"--(Atmospheric frequency grids along the propagation path.

ppvar stands for propagation path variable. The variables named in is
way describe the atmosphere and its properties at each point of the
propagation path

See *f_grid* for information about the frequency grid

Dimension: [ ppath.np ]

Usage: Output of radiative transfer methods.
)--",
      .type = "ArrayOfVector"};

  wsv_data["ppvar_pnd"] = {.desc = R"--(PND values along the propagation path.

See ``ppvar_p`` for a general description of WSVs of ppvar-type.

Dimension: [ number of scattering elements, ppath.np ]

Usage: Output of radiative transfer methods.
)--",
                           .type = "Matrix"};

  wsv_data["ppvar_optical_depth"] = {
      .desc =
          R"--(The optical depth between the sensor and each point of the propagation path.

Returned as the one-way optical depth even in the case of radar
simulations. Just a scalar value, i.e. no polarisation information is
provided.

See ``ppvar_p`` for a general description of WSVs of ppvar-type.

Dimension: [ ppath.np, f_grid]

Usage: Output of radiative transfer methods.
)--",
      .type = "Matrix"};

  wsv_data["ppvar_trans_cumulat"] = {
      .desc =
          R"--(The transmittance between the sensor and each point of the propagation path.

See ``ppvar_p`` for a general description of WSVs of ppvar-type.

Dimension: [ ppath.np, f_grid, stokes_dim, stokes_dim ]

Usage: Output of radiative transfer methods.
)--",
      .type = "Tensor4"};

  wsv_data["ppvar_trans_partial"] = {
      .desc =
          R"--(The transmittance between the points along the propagation path.

To maintain consistency in size also this variable stores np transmissivities,
while there are only np-1 distances between the points of the ppath. The
extra values placed at index 0 and can be seen as the transmissivities
between the sensor and the start of the ppath. These transmissivities
are always unity. That is, the transmissivities between ppath point i and i+1
are found at index i+1 in *ppvar_trans_partial*.

See ``ppvar_p`` for a general description of WSVs of ppvar-type.

Dimension: [ ppath.np, f_grid, stokes_dim, stokes_dim ]

Usage: Output of radiative transfer methods.
)--",
      .type = "Tensor4"};

  wsv_data["predefined_model_data"] = {
      .desc =
          R"--(This contains predefined model data.

Can currently only contain data for new MT CKD models of water.
)--",
      .type = "PredefinedModelData",
      .default_value = PredefinedModelData{}};

  wsv_data["spectral_radiance_profile_operator"] = {
      .desc =
          R"--(An operator to create a spectral radiance profile.

This is an experimental solution.
)--",
      .type = "SpectralRadianceProfileOperator"};

  wsv_data["propmat_clearsky"] = {
      .desc =
          R"--(This contains the absorption coefficients for one point in the atmosphere.

Dimensions: [naa, nza, nf, f(stokes_dim)]

Unit: 1/m
)--",
      .type = "PropmatVector"};

  wsv_data["propmat_clearsky_agenda_checked"] = {
      .desc = R"--(OK-flag for *propmat_clearsky_agenda*.

Set by *propmat_clearsky_agenda_checkedCalc*.
)--",
      .type = "Index",
      .default_value = Index{0}};

  wsv_data["propmat_clearsky_field"] = {.desc = R"--(Gas absorption field.

Contains the (polarized) gas absorption coefficients for all species
as a function of *f_grid*, ``p_grid``, ``lat_grid``, and ``lon_grid``. 

This is mainly for testing and plotting gas absorption. For RT
calculations, gas absorption is calculated or extracted locally,
therefore there is no need to store a global field. But this variable
is handy for easy plotting of absorption vs. pressure, for example.

Unit:       1/m

Dimensions: [species, f_grid, ``stokes_dim``, stokes_dim, p_grid, lat_grid, lon_grid]
)--",
                                        .type = "Tensor7"};

  wsv_data["psd_data"] = {
      .desc =
          R"--(Particle size distribution values for a set of points.

The variable contains particle size distribution data for one scattering
species. The row dimension corresponds to different positions, in the
same way as *pnd_agenda_input* is defined.

Dimensions: [ n_points, n_scattering_elements ]
)--",
      .type = "Matrix"};

  wsv_data["psd_size_grid"] = {
      .desc = R"--(The particle sizes associated with *psd_data*.

This variable holds the size of each scattering element considered.
Size can be defined differently, depending on particle size distribution
used. Most common choices should by equivalent diameter, maximum diameter
and mass.

Dimension: [ n_sizes ]
)--",
      .type = "Vector"};

  wsv_data["p_grid_orig"] = {
      .desc = R"--(The original pressure grid before optimization.

This variable is used to interpolate *cloudbox_field* back to its original
size after the calculation with ``OptimizeDoitPressureGrid``.
The variable is listed as a subentry to
\"workspace variables\".

Usage: Set by the user.

Unit:  Pa
)--",
      .type = "Vector"};

  wsv_data["p_hse"] = {
      .desc =
          R"--(Reference pressure calculation of hydrostatic equilibrium.

The altitude specified by this pressure is used as the reference
when calculating hydrostatic equilibrium. That is, the geometrical
altitude at this pressure is not changed.

Usage: Set by the user.

Unit:  Pa
)--",
      .type = "Numeric"};
  wsv_data["radiance_field"] = {.desc = R"--(Radiance field.

Radiant flux received by a surface per unit solid angle and per unit
area for each hemisphere. The last dimension denotes the hemispheres.
The first component is the downward radiance and the second component
is the upward radiance.

Units: W / (m^2 sr)

Size: [p_grid, 
       lat_grid, 
       lon_grid, 
       N_za, N_aa
)--",
                                .type = "Tensor5"};

  wsv_data["range_bins"] = {.desc = R"--(The range bins of an active instrument.

The bins are assumed to cover a range without gaps, and the bins are
defined by their edges. That is, the length of this vector is the
number of bins + 1.

The bins can potentially be defined in two ways, by altitude or time.
See the method you are using, if this variable shall hold time or
altitude (or maybe both options are treated).

Unit: m or s
)--",
                            .type = "Vector"};

  wsv_data["refr_index_air"] = {
      .desc = R"--(Real part of the refractive index of air.

The variable contains the refractive index summed over all relevant
constituents, at one position in the atmosphere. This refractive
is related to the phase velocity. See also *refr_index_air_group*.

Unit: 1
)--",
      .type = "Numeric"};

  wsv_data["refr_index_air_group"] = {.desc = R"--(Group index of refractivity.

This variable is defined as the ratio between group velocity and the
speed of ligh in vacuum. That is, it is defined as the \"standard\"
refractive index, but refers to the group velocity instead of the
phase velocity. See also *refr_index_air*.

Unit: 1
)--",
                                      .type = "Numeric"};

  wsv_data["retrieval_checked"] = {
      .desc =
          R"--(Flag indicating completeness and consistency of retrieval setup.

Unit: Boolean
)--",
      .type = "Index",
      .default_value = Index{0}};

  wsv_data["retrieval_eo"] = {
      .desc =
          R"--(The estimated error in the retrieval due to uncertainty in the observations.

The vector contains the square roots  of the diagonal elements of  the
covariance matrix of the error due to measurement noise, S_m in Rodgers'
book.
)--",
      .type = "Vector"};

  wsv_data["retrieval_ss"] = {
      .desc =
          R"--(The estimated error in the retrieval due to limited resolution of the
observation system.

The vector contains the square roots of the diagonal
elements of the covariance matrix of the smoothing error, S_s in Rodgers'
book.
)--",
      .type = "Vector"};

  wsv_data["rte_alonglos_v"] = {
      .desc =
          R"--(Velocity along the line-of-sight to consider for a RT calculation.

This variable gives the velocity of the imaginary detector in
monochromatic pencil beam calculations. The relevant velocity is
the projection along the line-of-sight (ie. total velocity shall not
be given). A positive value means a movement of the detector in the
same direction as the line-of-sight.

This variable is required to include Doppler effects due to
velocities of the observer, relative the centre of the coordinate
system used that is fixed to the planets centre point.

Unit: [ m/s ]
)--",
      .type = "Numeric",
      .default_value = Numeric{0.0}};

  wsv_data["rtp_pressure"] = {
      .desc = R"--(Pressure at a radiative transfer point.

This scalar variable holds the local pressure.

The WSV is used as input to methods and agendas calculating radiative
properties for a given conditions.

Usage: Communication variable.

Units: [ Pa ]
)--",
      .type = "Numeric"};

  wsv_data["rtp_temperature"] = {
      .desc = R"--(Temperature at a radiative transfer point.

This scalar variable can hold the local temperature. It is intended
mainly for communication with various methods and agendas, such as
methods and agendas calculating absorption coefficients.
The WSV is used as input to methods and agendas calculating radiative
properties for a given conditions.

Usage: Communication variable.

Units: [ K ]
)--",
      .type = "Numeric"};

  wsv_data["rt_integration_option"] = {
      .desc =
          R"--(Switch between integration approaches for radiative transfer steps.

See each WSM using this varaible as input for available options.
)--",
      .type = "String",
      .default_value = String{"default"}};

  wsv_data["rtp_vmr"] = {
      .desc =
          R"--(Absorption species abundances for radiative transfer calculations.

This vector variable holds the local abundance of the constituents
included in *abs_species*.

The WSV is used as input to methods and agendas calculating radiative
properties for a given conditions.

Usage: Communication variable.

Units: [ Differ between the elements, can be VMR, kg/m3 or #/m3. ]

Size:  Should match abs_species.nelem()
)--",
      .type = "Vector"};

  wsv_data["scat_data"] = {.desc = R"--(Array of single scattering data.

As *scat_data_raw*, but with frequency grids and dimensions reduced
to the RT's *f_grid* or a single frequency entry. Also, temperature
grid or dimensions can be reduced to a single entry, meaning no
temperature interpolation is done for the respective data.

Standard approach to derive scat_data is to use *scat_dataCalc* to
derive it from *scat_data_raw*.
)--",
                           .type = "ArrayOfArrayOfSingleScatteringData"};

  wsv_data["scat_data_checked"] = {.desc = R"--(OK-flag for *scat_data*.

Relevant checks are performed by *scat_data_checkedCalc*. Only the
value 1 is taken as OK.
)--",
                                   .type = "Index",
                                   .default_value = Index{0}};

  wsv_data["scat_data_raw"] = {.desc = R"--(Array of raw single scattering data.

This variable holds the single scattering properties for all 
scattering elements, organized according to their assignment to a
scattering species. *scat_data_raw* entries can be derived from
precalculated data files using the methods *ScatElementsPndAndScatAdd*,
*ScatSpeciesPndAndScatAdd*, or *ScatSpeciesScatAndMetaRead* or
can be calculated using *scat_data_singleTmatrix*.

This may be used in combination with *scat_meta*

Usage: Method ouput.

Members:

- SingleScatteringData:

  - Enum[ptype attribute]
  - String[description] 
  - Vector[f_grid]
  - Vector[T_grid]
  - Vector[za_grid]
  - Vector[aa_grid]
  - Tensor7[pha_mat_data]

    - [f_grid, T_grid, za_grid(scattered), aa_grid(scattered), za_grid(incoming), aa_grid(incoming), matrix_element]

  - Tensor5[ext_mat_data]

    - [f_grid, T_grid, za_grid, aa_grid, matrix_element]

  - Tensor5[abs_vec_data]

    - [f_grid, T_grid, za_grid, aa_grid, matrix_element]

Dimensions: [number of scattering species][number of scattering elements]
)--",
                               .type = "ArrayOfArrayOfSingleScatteringData"};

  wsv_data["scat_data_mono"] = {
      .desc = R"--(Monochromatic single scattering data.

This variable holds the single scattering properties for all
scattering species and scattering elements for a specified frequency.
It can be calculated from *scat_data* using *scat_data_monoCalc*,
which interpolates *scat_data* to the required frequency.
)--",
      .type = "ArrayOfArrayOfSingleScatteringData"};

  wsv_data["scat_data_single"] = {
      .desc = R"--(Structure for the single scattering data.

Comprises the single scattering data of a single scattering element.
See ARTS user guide for further information.

Usage: Set by the user.

Dimensions:

- SingleScatteringData 

  - Enum[ptype attribute]
  - String[description] 
  - Vector[f_grid]
  - Vector[T_grid]
  - Vector[za_grid]
  - Vector[aa_grid]
  - Tensor7[pha_mat_data]

    - [f_grid, T_grid, za_grid(scattered), aa_grid(scattered), za_grid(incoming), aa_grid(incoming), matrix_element]

  - Tensor5[ext_mat_data]

    - [f_grid, T_grid, za_grid, aa_grid, matrix_element]

  - Tensor5[abs_vec_data]

    - [f_grid, T_grid, za_grid, aa_grid, matrix_element]
)--",
      .type = "SingleScatteringData"};

  wsv_data["scat_lat_index"] = {
      .desc = R"--(Latitude index for scattering calculations.

This variable is used in methods used for computing scattering
properties of scattering elements like *opt_prop_sptFromData* and
*pha_matCalc*. It holds the information about the position for which the
scattering calculations are done.

Usage:    Input to the methods *spt_calc_agenda*, *pha_mat_spt_agenda*
)--",
      .type = "Index"};

  wsv_data["scat_lon_index"] = {
      .desc = R"--(Longitude index for scattering calculations.

This variable is used in methods used for computing scattering
properties of scattering elements like *opt_prop_sptFromData* and
*pha_matCalc*. It holds the information about the position for which the
scattering calculations are done.

Usage:    Input to the methods *spt_calc_agenda*, *pha_mat_spt_agenda*
)--",
      .type = "Index"};

  wsv_data["scat_meta_single"] = {
      .desc = R"--(Structure for the scattering meta data.

This variable holds the scattering meta data for a single scattering
element (see AUG for definition). Scattering meta data comprises
the microphysical description of the scattering element as necessary
to relate single scattering properties with mass density or flux
fields. That is, e.g., in order to handle the scattering element in
particle size (and shape) distribution calculations.

For a definition of the structure members see below.

Members of Numeric type can be flagged as unknown by setting them to
NAN. This will cause a runtime error in case the parameter is needed in
the calculation, but will be ignored otherwise.

Usage: Set by the user.

Members:
  description [*String*]
    Description: Free-form description of the scattering element,
    holding information deemed of interest by the user but not covered
    by other structure members (and not used within ARTS).
  source [*String*]
    Description: Free-form description of the source of the data,
    e.g., Mie, T-Matrix, or DDA calculation or a database or a
    literature source.
  refr_index [*String*]
    Description: Free-form description of the underlying complex
    refractive index data, e.g., a literature source.
  mass [*Numeric*]
    Unit: [kg]
    Description: The mass of the scattering element.
  diameter_max [*Numeric*]
    Unit: [m]
    Description: The maximum diameter (or dimension) of the scattering
    element, defined by the circumferential sphere diameter of the
    element. Note that this parameter is only used by some size
    distributions; it does not have a proper meaning if the scattering
    element represents an ensemble of differently sized particles.
  diameter_volume_equ [*Numeric*]
    Unit: [m]
    Description: The volume equivalent sphere diameter of the
    scattering element, i.e., the diameter of a sphere with the same
    volume. For nonspherical particles, volume refers to the volume
    of the particle-forming substance, not that of the circumferential
    sphere (which can be derived from diameter_max). If the particle
    consists of a mixture of materials, the substance
    encompasses the complete mixture. E.g., the substance of 'soft'
    ice particles includes both the ice and the air.
  diameter_area_equ_aerodynamical [*Numeric*]
    Unit: [m]
    Description: The area equivalent sphere diameter of the
    scattering element, i.e., the diameter of a sphere with the same
    cross-sectional area. Here, area refers to the aerodynamically
    relevant area, i.e., the cross-sectional area perpendicular to the
    direction of fall. Similarly to volume in the definition of
    diameter_volume_equ, for non-spherical and mixed-material
    particles, area refers to the area covered by the substance
    mixture of the particle.
)--",
      .type = "ScatteringMetaData"};

  wsv_data["scat_meta"] = {
      .desc = R"--(An Array of scattering meta data (*scat_meta_single*).

The array holds the meta data for all scattering elements. For a
description of the meta data contents refer to the documentation
of *scat_data_single*.

Corresponding to *scat_data*, it is organized in terms of scattering
species (i.e., one sub-array per scattering species holding one
*scat_meta_single* instance per scattering element assigned to this
scattering species). It is primarily used for particle size and shape
distribution calculations using ``pnd_fieldCalcFromParticleBulkProps``.
It is also applied for deducing microphysical characterizations of
scattering species, e.g., by *particle_massesFromMetaData*.

Note: This array must contain as many elements as *scat_data* (on
both array levels).

Usage: Set by the user.

Dimensions: [scattering species][scattering elements]

See also: *scat_meta_single*.
)--",
      .type = "ArrayOfArrayOfScatteringMetaData"};

  wsv_data["scat_p_index"] = {
      .desc = R"--(Pressure index for scattering calculations.

This variable is used in methods used for computing scattering
properties of scattering elements like *opt_prop_sptFromData* and
*pha_matCalc*. It holds the information about the location for which the
scattering calculations are done.

Usage:    Input to the methods *spt_calc_agenda*, *pha_mat_spt_agenda*
)--",
      .type = "Index"};

  wsv_data["scat_species"] = {
      .desc = R"--(Array of Strings defining the scattering species to consider.

Each String contains the information to connect scattering species
(e.g., hydrometeor) atmospheric fields with the microphysical
information like size and shape distributions. The strings follow
the following structure with individual elements separated by dashes:

- scattering species name [*String*]
  the name of the scattering species' atmospheric field. Free form,
  but is matched to *atm_fields_compact* fields by their names.
  Common are, e.g., IWC (ice water content), LWC (liquid water
  content), RR (rain rate), and SR (snow rate).
- particle size distribution [*String*]:
  the size distribution function/parametrization to apply. For
  currently possible PSDs see ``pnd_fieldCalcFromParticleBulkProps``.

Example: [''IWC-MH97'', ''LWC-H98_STCO'', ...]
)--",
      .type = "ArrayOfString",
      .default_value = ArrayOfString{}};

  wsv_data["scat_species_a"] = {
      .desc = R"--(Mass-size relationship parameter, for one scattering species.

Some methods require a relationship between mass and particle size,
valid for the complete scattering species. A common model for this
relationship is::

  mass(x) = a * x^b,

where x is size (that could be Dveq, Dmax or mass) and a/b are parameters.

This WSV is a in the expression above.
The WSV matching b is *scat_species_b*.
The WSV matching x is *scat_species_x*.
)--",
      .type = "Numeric"};

  wsv_data["scat_species_b"] = {
      .desc = R"--(Mass-size relationship parameter, for one scattering species.

See *scat_species_a* for details.
)--",
      .type = "Numeric"};

  wsv_data["scat_species_x"] = {
      .desc = R"--(The size grid of one scattering species.

The variable holds the sizes associated with one scattering species.
The typical application of these data are as the size grid when
calculating particle size distributions.

The user must set this WSV as several quantities can be used as size,
such as mass and maximum diamater.

See also *scat_species_a*, for example usage of this WSV.

Dimension:  [number of scattering elements]
)--",
      .type = "Vector"};

  wsv_data["select_abs_species"] = {
      .desc = R"--(A select species tag group from *abs_species*

If set to empty, this selection is void.  It must otherwise match perfectly a tag inside
*abs_species* for that to be the selection.
)--",
      .type = "ArrayOfSpeciesTag",
      .default_value = ArrayOfSpeciesTag{}};

  wsv_data["sensor_description_amsu"] = {
      .desc = R"--(Sensor description for simple AMSU setup.

This is a compact description of an AMSU-type sensor. The matrix
contains one row for each instrument channel. Each row contains three
elements: LO position [Hz], offset of the channel center from the LO
[Hz], and channel width [Hz].

Usage: Set by the user.

Unit: All entries in Hz.

Size: [number of channels, 3]
)--",
      .type = "Matrix"};

  wsv_data["sensor_norm"] = {
      .desc = R"--(Flag if sensor response should be normalised or not (0 or 1).

If the flag is set to 1 each sensor response is normalised (where
applicable). If set to 0 the sensor responses are left as provided.

See further the ARTS user guide (AUG). Use the index to find where
this variable is discussed. The variable is listed as a sub-entry to
\"workspace variables\".

Usage: Set by the user.
)--",
      .type = "Index"};

  wsv_data["sensor_pol"] = {.desc = R"--(A set of polarisation response angles.

The standard choice to consider the polarisation response of the
reciever is by *instrument_pol*, and this response becomes then part
of *sensor_response*. However, that choice is not possible when the
polartisation response changes between measurement blocks, and this
variable combined with the ``yApplySensorPol`` offers an alternative for
such situations. This WSV also allows defintion of an arbitrary
polarisation angle.

The columns of *sensor_pol* corresponds to the channels/frequencies
of the receiver. Each element gives the polarisation angle. A pure
vertical response has the angle 0 deg, and pure horisontal 90 deg.
If all U values (Stokes element 3) are zero, the sign of the angle does,
not matter, and 0 and 180 degrees give the same result. With non-zero
U, the result of e.g. -45 and +45 degrees differ.

Note that a receiver with a linear response is assumed. Circular
polarisation is not affected by any rotation.

Usage: Set by the user.

Unit:  [ degrees ]

Size:  [ number of measurement blocks, number of channels/frequencies ]
)--",
                            .type = "Matrix"};

  wsv_data["sensor_response"] = {
      .desc = R"--(The matrix modelling the total sensor response.

This matrix describes the sensor respons for one measurement block
The response is assumed to be identical for each such block.

The matrix is the product of all the individual sensor response
matrices. Therefore its dimensions are depending on the total sensor
configuration.

Usage: Output/input to the ``sensor_response...`` methods.

Units: -

Dimension: See the individual ``sensor_response...`` method.
)--",
      .type = "Sparse"};

  wsv_data["sensor_response_aa"] = {
      .desc = R"--(The relative azimuth angles associated with the output of
*sensor_response*.

The variable shall not be set manually, it will be set together with
*sensor_response* by sensor response WSMs.

Usage: Set by sensor response methods.

Unit:  [ degrees ]
)--",
      .type = "Vector"};

  wsv_data["spectral_radiance_background_jacobian"] = {
      .desc = R"--(Spectral radiance derivative from the background
)--",
      .type = "StokvecMatrix"};

  wsv_data["spectral_radiance_background"] = {
      .desc = R"--(Spectral radiance from the background
)--",
      .type = "StokvecVector"};

  wsv_data["background_transmittance"] = {
      .desc = R"--(Transmittance from the background
)--",
      .type = "MuelmatVector"};

  wsv_data["spectral_radiance_path"] = {.desc = R"--(Radiation along the propagation path
)--",
                           .type = "ArrayOfStokvecVector"};

  wsv_data["spectral_radiance_path_jacobian"] = {
      .desc = R"--(Radiation derivative along the propagation path
)--",
      .type = "ArrayOfStokvecMatrix"};

  wsv_data["ppvar_propmat"] = {
      .desc = R"--(Propagation matrices along the propagation path
)--",
      .type = "ArrayOfPropmatVector"};

  wsv_data["ppvar_dpropmat"] = {
      .desc = R"--(Propagation derivative matrices along the propagation path
)--",
      .type = "ArrayOfPropmatMatrix"};

  wsv_data["ppvar_nlte"] = {
      .desc = R"--(Additional NLTE along the propagation path
)--",
      .type = "ArrayOfStokvecVector"};

  wsv_data["ppvar_dnlte"] = {
      .desc = R"--(Additional NLTE derivative along the propagation path
)--",
      .type = "ArrayOfStokvecMatrix"};

  wsv_data["spectral_radiance_path_source"] = {
      .desc = R"--(Source vectors along the propagation path
)--",
      .type = "ArrayOfStokvecVector"};

  wsv_data["spectral_radiance_path_source_jacobian"] = {
      .desc = R"--(Source derivative vectors along the propagation path
)--",
      .type = "ArrayOfStokvecMatrix"};

  wsv_data["ppvar_tramat"] = {
      .desc = R"--(Transmission matrices along the propagation path
)--",
      .type = "ArrayOfMuelmatVector"};

  wsv_data["ppvar_cumtramat"] = {
      .desc = R"--(Cumulative transmission matrices along the propagation path
)--",
      .type = "ArrayOfMuelmatVector"};

  wsv_data["ppvar_dtramat"] = {
      .desc = R"--(Transmission derivative matrices along the propagation path
)--",
      .type = "ArrayOfArrayOfMuelmatMatrix"};

  wsv_data["ppvar_distance"] = {
      .desc = R"--(Layer distances along the propagation path
)--",
      .type = "Vector"};

  wsv_data["ppvar_ddistance"] = {
      .desc = R"--(Derivative of layer distances along the propagation path
)--",
      .type = "ArrayOfArrayOfVector"};

  wsv_data["sensor_response_f"] = {
      .desc =
          R"--(The frequencies associated with the output of *sensor_response*.

This vector gives the frequency for each element of the measurement
vector produced inside one measurement block. The frequencies of
the total measurement vector, ``y``, are obtained by repeating these
frequencies n times, where n is the number of measurement blocks.

The variable shall not be set manually, it will be set together with
*sensor_response* by sensor response WSMs.

Usage: Set by sensor response methods.

Unit:  [ Hz ]
)--",
      .type = "Vector"};

  wsv_data["sensor_response_f_grid"] = {
      .desc = R"--(The frequency grid associated with *sensor_response*.

A variable for communication between sensor response WSMs. Matches
initially *f_grid*, but is later adjusted according to the sensor
specifications. Only defined when a common grid exists. Values are
here not repeated as in *sensor_response_f*

Usage: Set by sensor response methods.

Unit:  [ Hz ]
)--",
      .type = "Vector"};

  wsv_data["sensor_response_pol"] = {
      .desc = R"--(The polarisation states associated with the output of
*sensor_response*.

Works basically as *sensor_response_f*.

See *instrument_pol* for coding of polarisation states.

The variable shall not be set manually, it will be set together with
*sensor_response* by sensor response WSMs.

Usage: Set by sensor response methods.

Unit:  [ - ]
)--",
      .type = "ArrayOfIndex"};

  wsv_data["sensor_response_pol_grid"] = {
      .desc = R"--(The \"polarisation grid\" associated with *sensor_response*.

A variable for communication between sensor response WSMs. It is
initially 1:stokes_dim, but can later adjusted according to the 
sensor specifications. Only defined when a common grid exists. 

See *instrument_pol* for coding of polarisation states.

Usage: Set by sensor response methods.

Unit:  [ - ]
)--",
      .type = "ArrayOfIndex"};

  wsv_data["sensor_time"] = {.desc = R"--(The time for each measurement block.

This WSV is used when a time must be assigned to the measurements.

Usage: Set by the user.

Unit:  [ UTC date and time ]

Size:  [ number of measurement blocks ]
)--",
                             .type = "ArrayOfTime"};

  wsv_data["sideband_mode"] = {.desc = R"--(Description of target sideband.

A text string describing which of the two sidebands (of a heterodyne
instrument) that can be seen as \"main\" band. Possible choices are:

- \"lower\" : Low frequency sideband shall be considered as target.
- \"upper\" : High frequency sideband shall be considered as target.

Usage: Set by the user.
)--",
                               .type = "String"};

  wsv_data["sideband_mode_multi"] = {
      .desc = R"--(Description of target sideband for a multiple LO receiver.

As *sideband_mode* but handles an instrument with several LO chains.
See further *lo_multi* and *sideband_response_multi*. This length of
this array must match the size of those WSVs.

Usage: Set by the user.
)--",
      .type = "ArrayOfString"};

  wsv_data["sideband_response"] = {
      .desc = R"--(Description of (mixer) sideband response.

This variable describes the response of each sideband of a heterodyne
receiver. The response is given as a GriddedField1, with frequency as the
grid. The actual data describe the sideband filter function at each
frequency grid point. An interpolation is applied to obtain the
response for other frequencies.

The frequency grid should be given in terms of IF, with end points
symmetrically placed around zero. That is, the grid must contain
both negative and positive values. The sideband response (after 
summation with *lo*) is not allowed to extend outside the range
for which spectral data exist (normally determined by *f_grid*).

Usage: Set by the user.

Dimensions: 

-  GriddedField1:

  -    Vector f_grid[N_f]
  -    Vector data[N_f]
)--",
      .type = "GriddedField1"};

  wsv_data["sideband_response_multi"] = {
      .desc = R"--(Description of multiple (mixer) sideband responses.

As *sideband_response* but describes an instrument with multiple
mixers. An array element for each LO. The size of this variable and
*lo_multi* shall match.

Unit: Hz

Usage: Set by the user.
)--",
      .type = "ArrayOfGriddedField1"};

  wsv_data["spectral_irradiance_field"] = {
      .desc = R"--(Spectral irradiance field.

Spectral irradiance is the radiative power per unit area
and unit frequency. The last dimension denotes the hemispheres.
The first component denotes the downward direction and the second
component denotes the upward direction.

Units: W m^-2 Hz^-1

 Size: [ Nf, p_grid, lat_grid, lon_grid, 2 ]
)--",
      .type = "Tensor5"};

  wsv_data["spectral_radiance_field"] = {.desc = R"--(Spectral radiance field.

This variable holds a calculation of the radiance field through
the atmosphere, for the directions matching *za_grid* and *aa_grid*.

Don't confuse this variable with *cloudbox_field*. That varinale also
holds a field of spectral radiances, but is restricted to the cloud box.

Units: W / (m^2 Hz sr)

 Size: [f_grid, p_grid,  lat_grid,  lon_grid,  za_grid, aa_grid, stokes_dim ]

Note:
 For 1D, the size of the latitude, longitude and azimuth
 dimension (N_aa) are all 1.
)--",
                                         .type = "Tensor7"};

  wsv_data["specific_heat_capacity"] = {.desc = R"--(Specific heat capacity.

It is the heat capacity per unit 
mass of a material.

Units: K J^-1 kg^-1

Size: [ p_grid, lat_grid,  lon_grid]
)--",
                                        .type = "Tensor3"};

  wsv_data["suns_do"] = {.desc = R"--(Flag to activate the sun(s).
)--",
                         .type = "Index",
                         .default_value = Index{0}};

  wsv_data["suns"] = {.desc = R"--(Array of Sun.

This variable describes a list of suns.
Each sun is described by a struct with its spectrum, radius,
distance from center of planet to center of sun,
temperature (if possible), latitude in the sky of the planet,
longitude in the sky of the planet and the type
)--",
                      .type = "ArrayOfSun",
                      .default_value = ArrayOfSun{}};

  wsv_data["surface_complex_refr_index"] = {
      .desc = R"--(Complex refractive index of the surface, at a single point.

See *complex_refr_index* for the expected format and how the data
are treated.
)--",
      .type = "ComplexGriddedField2"};

  wsv_data["surface_emission"] = {.desc = R"--(The emission from the surface.

See specific methods generating *surface_emission* and the user
guide for more information.

Dimensions: [ f_grid, stokes_dim ]
)--",
                                  .type = "Matrix"};

  wsv_data["surface_field"] = {
      .desc = R"--(The surface field describes the surface properties.

Connected to *surface_point*, this describes the global surface values, such as elevation and 
temperature but also entirerly abstract properties and types.
)--",
      .type = "SurfaceField"};

  wsv_data["surface_normal"] = {
      .desc = R"--(The normal vector for a point at the surface.

The vector is given as a zenith and azimuth angle following the
definition of line-of-sights.

Units: degrees

Size:  [ 2 ]
)--",
      .type = "Vector"};

  wsv_data["surface_point"] = {
      .desc = R"--(The surface point describes the surface properties.

Connected to *surface_field*, this describes the local surface values, such as elevation and 
temperature but also entirerly abstract properties and types.  As the elevation field is not
known for a single point, the normal of the surface at the position is also stored.
)--",
      .type = "SurfacePoint"};

  wsv_data["surface_props_names"] = {
      .desc = R"--(Name on surface properties found in ``surface_props_data``.

Each string names a property in ``surface_props_data``. The user is free
to include data with any name, but the surface methods making use of
``surface_props_data`` expect data to be named in a specific way. See
the documentation of each method for recognised choices.

Size:  [ number of props. ]
)--",
      .type = "ArrayOfString",
      .default_value = ArrayOfString{}};

  wsv_data["surface_rmatrix"] = {
      .desc = R"--(The reflection coefficients for the surface
      
The directions are given by surface line-of-sight to the direction of interest.

The rows and columns of this tensor holds the reflection
coefficient matrix for one frequency and one LOS. The reflection
coefficients shall take into accound the angular weighting of the
downwelling radiation.

See specific methods generating *surface_rmatrix* and the user guide
for more information.

Units:      -

Dimensions: [ surface_los, f_grid, stokes_dim, stokes_dim ]
)--",
      .type = "Tensor4"};

  wsv_data["surface_search_accuracy"] = {
      .desc = R"--(Accuracy of length to surface intersections.

The surface elevation can not be expressed analytically and inter-
section between the propagation path and the surface must be found
by some search algorithm. This variable specifies the accuracy to
meet by these algorithms.

Unit:  m
)--",
      .type = "Numeric",
      .default_value = Numeric{1}};

  wsv_data["surface_skin_t"] = {.desc = R"--(Surface skin temperature.

This temperature shall be selected considering the radiative
properties of the surface, and can differ from the \"bulk\"
temperature.
)--",
                                .type = "Numeric"};

  wsv_data["surface_reflectivity"] = {
      .desc = R"--(Surface reflectivity, for a given position and angle.

This variable describes the surface reflectivity at one position
and one incidence angle. It works as *surface_scalar_reflectivity*
but is also defined for vector radiative transfer.

The first dimension of the variable shall either match *f_grid* or
be 1. The later case is interpreted as the reflectivity is the same
for all frequencies.

Usage:   Input to some surface properties methods.

Dimensions: [ f_grid or 1, stokes_dim, stokes_dim]
)--",
      .type = "Tensor3"};

  wsv_data["surface_rv_rh"] = {
      .desc =
          R"--(Surface reflectivity, described by rv and rh (power) reflectivities.

This variable describes the surface reflectivity at one position
and one incidence angle. For this position and angle, one or multiple
combinations of rv and rh are specified, where rv and rh are the
reflectivity for vertical and horizontal polarisation, respectively.

This matrix shall always have two columns, where the first column
holds rv values, and the second column rh. It is up to the user to
make sure that data are put into the correct column, this can not
be checked bu the methods using this WSV.

The number of rows shall either match *f_grid* or be 1. The later case
is interpreted as the reflectivities are the same for all frequencies.

Usage:   Input to some surface properties methods.

Dimensions: [ f_grid or 1, 2]
)--",
      .type = "Matrix"};

  wsv_data["surface_scalar_reflectivity"] = {
      .desc =
          R"--(Surface reflectivity, assuming it can be described as a scalar value.

This variable describes the surface reflectivity at one position
and one incidence angle. For this position and angle, one or multiple
scalar reflectivities are specified.

The length of the vector shall either match *f_grid* or be 1. The 
later case is interpreted as the reflectivity is the same for all
frequencies (ie. matches a constant vector).

Usage:   Input to some surface properties methods.

Dimensions: [ f_grid or 1]
)--",
      .type = "Vector"};

  wsv_data["surface_type_mask"] = {
      .desc = R"--(Classification of the surface using a type coding.

Dimensions: 

-  GriddedField2:

  -    Vector Latitude [N_lat]
  -    Vector Longitude [N_lon]
  -    Matrix data [N_lat][N_lon]
)--",
      .type = "GriddedField2"};

  wsv_data["surface_type_mix"] = {
      .desc = R"--(Gives the fraction of different surface types.
)--",
      .type = "Vector"};

  wsv_data["telsem_atlases"] = {.desc = R"--(TELSEM 2 emissivity atlases.

Array should be filled with 12
atlases, one for each month. Index 0 is January, index 11 December.

)--",
                                .type = "ArrayOfTelsemAtlas"};

  wsv_data["tessem_neth"] = {.desc = R"--(FIXMEDOC Add more documentation?
TESSEM2 neural network parameters for horizontal polarization.
)--",
                             .type = "TessemNN"};

  wsv_data["tessem_netv"] = {.desc = R"--(FIXMEDOC Add more documentation?
TESSEM2 neural network parameters for vertical polarization.
)--",
                             .type = "TessemNN"};

  wsv_data["time"] = {.desc = R"--(A UTC time point.
)--",
                      .type = "Time"};

  wsv_data["time_grid"] = {.desc = R"--(A grid of times.  Should be increasing
)--",
                           .type = "ArrayOfTime"};

  wsv_data["time_stamps"] = {
      .desc = R"--(A set of times.  Can be in random order
)--",
      .type = "ArrayOfTime"};

  wsv_data["water_p_eq_field"] = {
      .desc = R"--(The field of water saturation pressure.

This variable holds the saturation pressure of water at each crossing of
the pressure, latitude and longitude grids.

Unit:       Pa

Dimensions: [ p_grid, lat_grid, lon_grid ]
)--",
      .type = "Tensor3"};

  wsv_data["wigner_initialized"] = {
      .desc = R"--(Indicates if the wigner tables are initialized.
If they are not, computations will be aborted.

Will hold the value of provided maximum factorial value

The developer should always test this variable in functions
that might require computing wigner symbols because the error
handling is otherwise offloaded to third party software...
)--",
      .type = "Index"};

  wsv_data["wmrf_channels"] = {
      .desc = R"--(Channel selection for WMRF fast calculation.

This variable can be used to select one or several instrument channels
from the list of all possible channels. Zero-based indexing is used, so
Channel 0 is the first instrument channel!
)--",
      .type = "ArrayOfIndex"};

  wsv_data["wmrf_weights"] = {
      .desc = R"--(The weights for a WMRF fast calculation.

Weights are stored in a sparse matrix. This can be used as a
sensor_response matrix.

The dimension of the matrix is (nchan, nfreq), where nchan
is the number of instrument channels and nfreq is the number
of monochromatic frequencies.
)--",
      .type = "Sparse"};

  wsv_data["xml_output_type"] = {
      .desc =
          R"--(Flag to determine whether XML output shall be binary or ascii.

This flag has to be set using the workspace method
*output_file_formatSetAscii* or *output_file_formatSetBinary*.
One of these methods MUST be called before writing the first
output file.

Usage: Set by user.
)--",
      .type = "Index"};

  wsv_data["x"] = {.desc = R"--(The state vector.

This WSV matches directly the x-vector in the formalism by C.D. Rodgers.

Inside *x*, the elements matching one retrieval quantity, such as
atmospheric temperatures, are kept together. That is, each retrieval
quantity covers a continuous range inside *x*. The start and index of
these ranges can be deduced by *jacobian_targets* (see function(s)
inside jacobian.cc for details).

The order of elements inside each retrieval quantity should be clarified
by corresponding \"adding\" method, i.e. ``jacobianAddTemperature`` for
atmospheric temperatures. The general rule is that data are sorted from
left to right with respect to the order in the corresponding WSV. For
example, inside *x* atmospheric data are stored with pressure as inner-
most loop, followed by latitude and longitude as outermost loop.

Usage: Used by inversion methods.

Unit:  Varies, follows unit of selected retrieval quantities.
)--",
                   .type = "Vector"};

  wsv_data["xa"] = {.desc = R"--(The a priori state vector.

This WSV matches directly the x_a-vector in the formalism by C.D. Rodgers.

Usage: Used by inversion methods.

Unit:  Varies, follows unit of selected retrieval quantities.
)--",
                    .type = "Vector"};

  wsv_data["za_grid"] = {.desc = R"--(Zenith angle grid.

The zenith angle grid, on which the *cloudbox_field* is stored. 
This grid is used for RT calculations inside the cloudbox, therefore
the grid has to be defined if the cloudbox is activated by the flag
*cloudbox_on*. Furthermore the zenith angle grid is also used for RT
calculations of clear-sky *spectral_radiance_field*. 
The grid must be sorted in increasing order, with no repetitions.

Usage:      Set by the user.

Unit:       degrees
)--",
                         .type = "Vector"};

  wsv_data["za_grid_weights"] = {.desc = R"--(Zenith angle integration weights.

The integration weight are needed for calculation of radiation fluxes

Unit:  unitless
)--",
                                 .type = "Vector"};

  wsv_data["za_index"] = {
      .desc = R"--(Zenith angle index for scattering calculations.
 
This variable is used internally in WSMs for computing scattering 
properties. 

Usage:    Input to the agendas *spt_calc_agenda*, *pha_mat_spt_agenda*.
)--",
      .type = "Index"};

  wsv_data["z_hse_accuracy"] = {
      .desc =
          R"--(Minimum accuracy for calculation of hydrostatic equilibrium.

Usage: Set by the user.

Unit:  m
)--",
      .type = "Numeric"};

  wsv_data["water_equivalent_pressure_operator"] = {
      .desc = R"--(The water equivalent pressure operator.
)--",
      .type = "NumericUnaryOperator"};

  wsv_data["gravity_operator"] = {.desc = R"--(The gravity operator.

Returns gravity in m/s^2 for a given altitude [m], latitude [deg] and longitude [deg].
)--",
                                  .type = "NumericTernaryOperator"};

  wsv_data["spectral_radiance"] = {.desc = R"--(A spectral radiance vector.
)--",
                     .type = "StokvecVector"};

  wsv_data["spectral_radiance_jacobian"] = {.desc = R"--(A spectral radiance derivative matrix.
)--",
                      .type = "StokvecMatrix"};

  wsv_data["jacobian_targets"] = {
      .desc = R"--(A list of targets for the Jacobian Matrix calculations.
)--",
      .type = "JacobianTargets",
      .default_value = JacobianTargets{}};

  wsv_data["path_point"] = {.desc = R"--(A single path point.
)--",
                            .type = "PropagationPathPoint"};

  wsv_data["propagation_path"] = {
      .desc = R"--(A list path points making up a propagation path.
)--",
      .type = "ArrayOfPropagationPathPoint"};

  return wsv_data;
}
