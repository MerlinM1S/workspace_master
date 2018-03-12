import tensorflow as tf

mantatensor_module = tf.load_op_library('/home/ansorge/tensorflow/bazel-bin/tensorflow/core/mantatensor/mantatensor.so')
manta_k_epsilon_compute_production = mantatensor_module.k_epsilon_compute_production
manta_k_epsilon_sources = mantatensor_module.k_epsilon_sources
manta_k_epsilon_bcs = mantatensor_module.k_epsilon_bcs
manta_k_epsilon_gradient_diffusion = mantatensor_module.k_epsilon_gradient_diffusion
manta_extrapolate_mac_simple = mantatensor_module.extrapolate_mac_simple
manta_extrapolate_mac_from_weight = mantatensor_module.extrapolate_mac_from_weight
manta_extrapolate_ls_simple = mantatensor_module.extrapolate_ls_simple
manta_extrapolate_vec3_simple = mantatensor_module.extrapolate_vec3_simple
manta_apply_mask_to_grid_int = mantatensor_module.apply_mask_to_grid_int
manta_apply_mask_to_grid_float = mantatensor_module.apply_mask_to_grid_float
manta_apply_mask_to_grid_vec3 = mantatensor_module.apply_mask_to_grid_vec3
manta_add_gravity = mantatensor_module.add_gravity
manta_add_gravity_no_scale = mantatensor_module.add_gravity_no_scale
manta_add_buoyancy = mantatensor_module.add_buoyancy
manta_set_open_bound = mantatensor_module.set_open_bound
manta_reset_outflow = mantatensor_module.reset_outflow
manta_kn_set_wall_bcs = mantatensor_module.kn_set_wall_bcs
manta_set_wall_bcs = mantatensor_module.set_wall_bcs
manta_set_obstacle_velocity = mantatensor_module.set_obstacle_velocity
manta_vorticity_confinement = mantatensor_module.vorticity_confinement
manta_add_force_field = mantatensor_module.add_force_field
manta_interpolate_grid = mantatensor_module.interpolate_grid
manta_interpolate_grid_vec3 = mantatensor_module.interpolate_grid_vec3
manta_interpolate_mac_grid = mantatensor_module.interpolate_mac_grid
manta_compute_energy = mantatensor_module.compute_energy
manta_compute_wavelet_coeffs = mantatensor_module.compute_wavelet_coeffs
manta_compute_vorticity = mantatensor_module.compute_vorticity
manta_compute_strain_rate_mag = mantatensor_module.compute_strain_rate_mag
manta_extrapolate_simple_flags_mac_grid = mantatensor_module.extrapolate_simple_flags_mac_grid
manta_extrapolate_simple_flags_grid_vec3 = mantatensor_module.extrapolate_simple_flags_grid_vec3
manta_extrapolate_simple_flags_gridfloat = mantatensor_module.extrapolate_simple_flags_gridfloat
manta_get_curl = mantatensor_module.get_curl
manta_test_init_grid_with_pos = mantatensor_module.test_init_grid_with_pos
manta_debug_int_to_real = mantatensor_module.debug_int_to_real
manta_combine_grid_vel = mantatensor_module.combine_grid_vel
manta_get_laplacian = mantatensor_module.get_laplacian
manta_reset_phi_in_obs = mantatensor_module.reset_phi_in_obs
manta_advect_semi_lagrange_mac_grid = mantatensor_module.advect_semi_lagrange_mac_grid
manta_advect_semi_lagrange_grid_vec3 = mantatensor_module.advect_semi_lagrange_grid_vec3
manta_advect_semi_lagrange_gridfloat = mantatensor_module.advect_semi_lagrange_gridfloat
manta_calc_sec_deriv2d = mantatensor_module.calc_sec_deriv2d
manta_normalize_sum_to = mantatensor_module.normalize_sum_to
manta_cg_solve_we = mantatensor_module.cg_solve_we
manta_copy_mac_to_vec3 = mantatensor_module.copy_mac_to_vec3
manta_convert_mac_to_vec3 = mantatensor_module.convert_mac_to_vec3
manta_resample_vec3_to_mac = mantatensor_module.resample_vec3_to_mac
manta_resample_mac_to_vec3 = mantatensor_module.resample_mac_to_vec3
manta_copy_levelset_to_real = mantatensor_module.copy_levelset_to_real
manta_copy_vec3_to_real = mantatensor_module.copy_vec3_to_real
manta_copy_real_to_vec3 = mantatensor_module.copy_real_to_vec3
manta_convert_levelset_to_real = mantatensor_module.convert_levelset_to_real
manta_swap_components = mantatensor_module.swap_components
manta_reset_uv_grid = mantatensor_module.reset_uv_grid
manta_update_uv_weight = mantatensor_module.update_uv_weight
manta_get_component = mantatensor_module.get_component
manta_set_component = mantatensor_module.set_component
manta_density_from_levelset = mantatensor_module.density_from_levelset
manta_get_spiral_velocity2d = mantatensor_module.get_spiral_velocity2d
manta_set_gradient_y_weight = mantatensor_module.set_gradient_y_weight
manta_pd_fluid_guiding = mantatensor_module.pd_fluid_guiding
manta_cg_solve_diffusion_mac_grid = mantatensor_module.cg_solve_diffusion_mac_grid
manta_cg_solve_diffusion_grid_vec3 = mantatensor_module.cg_solve_diffusion_grid_vec3
manta_cg_solve_diffusion_gridfloat = mantatensor_module.cg_solve_diffusion_gridfloat
manta_apply_emission = mantatensor_module.apply_emission
manta_check_symmetry = mantatensor_module.check_symmetry
manta_check_symmetry_vec3 = mantatensor_module.check_symmetry_vec3
manta_update_fractions = mantatensor_module.update_fractions
manta_set_obstacle_flags = mantatensor_module.set_obstacle_flags
manta_init_vortex_velocity = mantatensor_module.init_vortex_velocity
manta_process_burn = mantatensor_module.process_burn
manta_update_flame = mantatensor_module.update_flame
manta_quantize_grid = mantatensor_module.quantize_grid
manta_quantize_grid_vec3 = mantatensor_module.quantize_grid_vec3
manta_solve_pressure = mantatensor_module.solve_pressure
manta_solve_pressure_part1 = mantatensor_module.solve_pressure_part1
manta_solve_pressure_part2 = mantatensor_module.solve_pressure_part2


# void KEpsilonComputeProduction(MACGrid vel, Grid<Real> k, Grid<Real> eps, Grid<Real> prod, Grid<Real> nuT, Grid<Real> strain=0, const Real pscale=1.0f);
def k_epsilon_compute_production(vel, k, eps, prod, nuT, strain, pscale):
	output = (vel, k, eps, prod, nuT, strain)
	funcCall = manta_k_epsilon_compute_production(vel, k, eps, prod, nuT, strain, pscale)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void KEpsilonSources(Grid<Real> k, Grid<Real> eps, Grid<Real> prod);
def k_epsilon_sources(k, eps, prod):
	output = (k, eps, prod)
	funcCall = manta_k_epsilon_sources(k, eps, prod)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void KEpsilonBcs(FlagGrid flags, Grid<Real> k, Grid<Real> eps, const Real intensity, const Real nu, const bool fillArea);
def k_epsilon_bcs(flags, k, eps, intensity, nu, fillArea):
	output = (flags, k, eps)
	funcCall = manta_k_epsilon_bcs(flags, k, eps, intensity, nu, fillArea)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void KEpsilonGradientDiffusion(Grid<Real> k, Grid<Real> eps, Grid<Real> nuT, const Real sigmaU=4.0, MACGrid vel=0);
def k_epsilon_gradient_diffusion(k, eps, nuT, sigmaU, vel):
	output = (k, eps, nuT, vel)
	funcCall = manta_k_epsilon_gradient_diffusion(k, eps, nuT, sigmaU, vel)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void extrapolateMACSimple(FlagGrid flags, MACGrid vel, const int distance=4, LevelsetGrid phiObs=NULL, const bool intoObs=false);
def extrapolate_mac_simple(flags, vel, distance, phiObs, intoObs):
	output = (flags, vel, phiObs)
	funcCall = manta_extrapolate_mac_simple(flags, vel, distance, phiObs, intoObs)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void extrapolateMACFromWeight(MACGrid vel, Grid<Vec3> weight, const int distance=2);
def extrapolate_mac_from_weight(vel, weight, distance):
	output = (vel, weight)
	funcCall = manta_extrapolate_mac_from_weight(vel, weight, distance)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void extrapolateLsSimple(Grid<Real> phi, const int distance=4, const bool inside=false);
def extrapolate_ls_simple(phi, distance, inside):
	return tf.assign(phi, manta_extrapolate_ls_simple(phi, distance, inside))

# void extrapolateVec3Simple(Grid<Vec3> vel, Grid<Real> phi, const int distance=4, const bool inside=false);
def extrapolate_vec3_simple(vel, phi, distance, inside):
	output = (vel, phi)
	funcCall = manta_extrapolate_vec3_simple(vel, phi, distance, inside)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void ApplyMaskToGrid<int>(Grid<int> grid, const Grid<int> mask, const int value, const FlagGrid respectFlags);
def apply_mask_to_grid_int(grid, mask, value, respectFlags):
	return tf.assign(grid, manta_apply_mask_to_grid_int(grid, mask, value, respectFlags))

# void ApplyMaskToGrid<float>(Grid<float> grid, const Grid<int> mask, const float value, const FlagGrid respectFlags);
def apply_mask_to_grid_float(grid, mask, value, respectFlags):
	return tf.assign(grid, manta_apply_mask_to_grid_float(grid, mask, value, respectFlags))

# void ApplyMaskToGrid<Vec3>(Grid<Vec3> grid, const Grid<int> mask, const Vec3 value, const FlagGrid respectFlags);
def apply_mask_to_grid_vec3(grid, mask, value, respectFlags):
	return tf.assign(grid, manta_apply_mask_to_grid_vec3(grid, mask, value, respectFlags))

# void addGravity(FlagGrid flags, MACGrid vel, const Vec3 gravity);
def add_gravity(flags, vel, gravity):
	output = (flags, vel)
	funcCall = manta_add_gravity(flags, vel, gravity)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void addGravityNoScale(FlagGrid flags, MACGrid vel, const Vec3 gravity);
def add_gravity_no_scale(flags, vel, gravity):
	output = (flags, vel)
	funcCall = manta_add_gravity_no_scale(flags, vel, gravity)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void addBuoyancy(const FlagGrid flags, const Grid<Real> density, MACGrid vel, const Vec3 gravity, const Real coefficient=1.);
def add_buoyancy(flags, density, vel, gravity, coefficient):
	return tf.assign(vel, manta_add_buoyancy(flags, density, vel, gravity, coefficient))

# void setOpenBound(FlagGrid flags, const int bWidth, const string openBound="", const int type=FlagGrid::TypeOutflow | FlagGrid::TypeEmpty);
def set_open_bound(flags, bWidth, type):
	return tf.assign(flags, manta_set_open_bound(flags, bWidth, type))

# void resetOutflow(FlagGrid flags, Grid<Real> phi=0, BasicParticleSystem parts=0, Grid<Real> real=0, Grid<int> index=0, ParticleIndexSystem indexSys=0);
def reset_outflow(flags, phi, real, index):
	output = (flags, phi, real, index)
	funcCall = manta_reset_outflow(flags, phi, real, index)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void KnSetWallBcs(const FlagGrid flags, MACGrid vel);
def kn_set_wall_bcs(flags, vel):
	return tf.assign(vel, manta_kn_set_wall_bcs(flags, vel))

# void setWallBcs(const FlagGrid flags, MACGrid vel, const MACGrid fractions=0, const Grid<Real> phiObs=0, const int boundaryWidth=0);
def set_wall_bcs(flags, vel, fractions, phiObs, boundaryWidth):
	return tf.assign(vel, manta_set_wall_bcs(flags, vel, fractions, phiObs, boundaryWidth))

# void setObstacleVelocity(FlagGrid flags, MACGrid vel, Grid<Vec3> obsvel, const int borderWidth=0);
def set_obstacle_velocity(flags, vel, obsvel, borderWidth):
	output = (flags, vel, obsvel)
	funcCall = manta_set_obstacle_velocity(flags, vel, obsvel, borderWidth)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void vorticityConfinement(MACGrid vel, FlagGrid flags, const Real strength);
def vorticity_confinement(vel, flags, strength):
	output = (vel, flags)
	funcCall = manta_vorticity_confinement(vel, flags, strength)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void addForceField(FlagGrid flags, MACGrid vel, Grid<Vec3> force);
def add_force_field(flags, vel, force):
	output = (flags, vel, force)
	funcCall = manta_add_force_field(flags, vel, force)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void interpolateGrid(Grid<Real> target, Grid<Real> source, const Vec3 scale=Vec3(1.), const Vec3 offset=Vec3(0.), const Vec3i size=Vec3i(-1,-1,-1), const int orderSpace=1);
def interpolate_grid(target, source, scale, offset, size, orderSpace):
	output = (target, source)
	funcCall = manta_interpolate_grid(target, source, scale, offset, size, orderSpace)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void interpolateGridVec3(Grid<Vec3> target, Grid<Vec3> source, const Vec3 scale=Vec3(1.), const Vec3 offset=Vec3(0.), const Vec3i size=Vec3i(-1,-1,-1), const int orderSpace=1);
def interpolate_grid_vec3(target, source, scale, offset, size, orderSpace):
	output = (target, source)
	funcCall = manta_interpolate_grid_vec3(target, source, scale, offset, size, orderSpace)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void interpolateMACGrid(MACGrid target, MACGrid source, const Vec3 scale=Vec3(1.), const Vec3 offset=Vec3(0.), const Vec3i size=Vec3i(-1,-1,-1), const int orderSpace=1);
def interpolate_mac_grid(target, source, scale, offset, size, orderSpace):
	output = (target, source)
	funcCall = manta_interpolate_mac_grid(target, source, scale, offset, size, orderSpace)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void computeEnergy(FlagGrid flags, MACGrid vel, Grid<Real> energy);
def compute_energy(flags, vel, energy):
	output = (flags, vel, energy)
	funcCall = manta_compute_energy(flags, vel, energy)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void computeWaveletCoeffs(Grid<Real> input);
def compute_wavelet_coeffs(input):
	return tf.assign(input, manta_compute_wavelet_coeffs(input))

# void computeVorticity(MACGrid vel, Grid<Vec3> vorticity, Grid<Real> norm);
def compute_vorticity(vel, vorticity, norm):
	output = (vel, vorticity, norm)
	funcCall = manta_compute_vorticity(vel, vorticity, norm)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void computeStrainRateMag(MACGrid vel, Grid<Real> mag);
def compute_strain_rate_mag(vel, mag):
	output = (vel, mag)
	funcCall = manta_compute_strain_rate_mag(vel, mag)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void extrapolateSimpleFlags(FlagGrid flags, MACGrid val, const int distance=4, const int flagFrom=FlagGrid::TypeFluid, const int flagTo=FlagGrid::TypeObstacle);
def extrapolate_simple_flags_mac_grid(flags, val, distance, flagFrom, flagTo):
	output = (flags, val)
	funcCall = manta_extrapolate_simple_flags_mac_grid(flags, val, distance, flagFrom, flagTo)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void extrapolateSimpleFlags(FlagGrid flags, Grid<Vec3> val, const int distance=4, const int flagFrom=FlagGrid::TypeFluid, const int flagTo=FlagGrid::TypeObstacle);
def extrapolate_simple_flags_grid_vec3(flags, val, distance, flagFrom, flagTo):
	output = (flags, val)
	funcCall = manta_extrapolate_simple_flags_grid_vec3(flags, val, distance, flagFrom, flagTo)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void extrapolateSimpleFlags(FlagGrid flags, Grid<float> val, const int distance=4, const int flagFrom=FlagGrid::TypeFluid, const int flagTo=FlagGrid::TypeObstacle);
def extrapolate_simple_flags_gridfloat(flags, val, distance, flagFrom, flagTo):
	output = (flags, val)
	funcCall = manta_extrapolate_simple_flags_gridfloat(flags, val, distance, flagFrom, flagTo)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void getCurl(MACGrid vel, Grid<Real> vort, const int comp);
def get_curl(vel, vort, comp):
	output = (vel, vort)
	funcCall = manta_get_curl(vel, vort, comp)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void testInitGridWithPos(Grid<Real> grid);
def test_init_grid_with_pos(grid):
	return tf.assign(grid, manta_test_init_grid_with_pos(grid))

# void debugIntToReal(Grid<int> source, Grid<Real> dest, const Real factor=1.);
def debug_int_to_real(source, dest, factor):
	output = (source, dest)
	funcCall = manta_debug_int_to_real(source, dest, factor)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void combineGridVel(MACGrid vel, Grid<Vec3> weight, MACGrid combineVel, LevelsetGrid phi=NULL, const Real narrowBand=0.0, const Real thresh=0.0);
def combine_grid_vel(vel, weight, combineVel, phi, narrowBand, thresh):
	output = (vel, weight, combineVel, phi)
	funcCall = manta_combine_grid_vel(vel, weight, combineVel, phi, narrowBand, thresh)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void getLaplacian(Grid<Real> laplacian, const Grid<Real> grid);
def get_laplacian(laplacian, grid):
	return tf.assign(laplacian, manta_get_laplacian(laplacian, grid))

# void resetPhiInObs(const FlagGrid flags, Grid<Real> sdf);
def reset_phi_in_obs(flags, sdf):
	return tf.assign(sdf, manta_reset_phi_in_obs(flags, sdf))

# void advectSemiLagrange(const FlagGrid flags, const MACGrid vel, MACGrid grid, const int order=1, const Real strength=1.0, const int orderSpace=1, const bool openBounds=false, const int boundaryWidth=1, const int clampMode=1);
def advect_semi_lagrange_mac_grid(flags, vel, grid, order, strength, orderSpace, openBounds, boundaryWidth, clampMode):
	return tf.assign(grid, manta_advect_semi_lagrange_mac_grid(flags, vel, grid, order, strength, orderSpace, openBounds, boundaryWidth, clampMode))

# void advectSemiLagrange(const FlagGrid flags, const MACGrid vel, Grid<Vec3> grid, const int order=1, const Real strength=1.0, const int orderSpace=1, const bool openBounds=false, const int boundaryWidth=1, const int clampMode=1);
def advect_semi_lagrange_grid_vec3(flags, vel, grid, order, strength, orderSpace, openBounds, boundaryWidth, clampMode):
	return tf.assign(grid, manta_advect_semi_lagrange_grid_vec3(flags, vel, grid, order, strength, orderSpace, openBounds, boundaryWidth, clampMode))

# void advectSemiLagrange(const FlagGrid flags, const MACGrid vel, Grid<float> grid, const int order=1, const Real strength=1.0, const int orderSpace=1, const bool openBounds=false, const int boundaryWidth=1, const int clampMode=1);
def advect_semi_lagrange_gridfloat(flags, vel, grid, order, strength, orderSpace, openBounds, boundaryWidth, clampMode):
	return tf.assign(grid, manta_advect_semi_lagrange_gridfloat(flags, vel, grid, order, strength, orderSpace, openBounds, boundaryWidth, clampMode))

# void calcSecDeriv2d(const Grid<Real> v, Grid<Real> curv);
def calc_sec_deriv2d(v, curv):
	return tf.assign(curv, manta_calc_sec_deriv2d(v, curv))

# void normalizeSumTo(Grid<Real> height, const Real target);
def normalize_sum_to(height, target):
	return tf.assign(height, manta_normalize_sum_to(height, target))

# void cgSolveWE(FlagGrid flags, Grid<Real> ut, Grid<Real> utm1, Grid<Real> out, const bool crankNic=false, const Real cSqr=0.25, const Real cgMaxIterFac=1.5, const Real cgAccuracy=1e-5);
def cg_solve_we(flags, ut, utm1, out, crankNic, cSqr, cgMaxIterFac, cgAccuracy):
	output = (flags, ut, utm1, out)
	funcCall = manta_cg_solve_we(flags, ut, utm1, out, crankNic, cSqr, cgMaxIterFac, cgAccuracy)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void copyMacToVec3(MACGrid source, Grid<Vec3> target);
def copy_mac_to_vec3(source, target):
	output = (source, target)
	funcCall = manta_copy_mac_to_vec3(source, target)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void convertMacToVec3(MACGrid source, Grid<Vec3> target);
def convert_mac_to_vec3(source, target):
	output = (source, target)
	funcCall = manta_convert_mac_to_vec3(source, target)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void resampleVec3ToMac(Grid<Vec3> source, MACGrid target);
def resample_vec3_to_mac(source, target):
	output = (source, target)
	funcCall = manta_resample_vec3_to_mac(source, target)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void resampleMacToVec3(MACGrid source, Grid<Vec3> target);
def resample_mac_to_vec3(source, target):
	output = (source, target)
	funcCall = manta_resample_mac_to_vec3(source, target)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void copyLevelsetToReal(LevelsetGrid source, Grid<Real> target);
def copy_levelset_to_real(source, target):
	output = (source, target)
	funcCall = manta_copy_levelset_to_real(source, target)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void copyVec3ToReal(Grid<Vec3> source, Grid<Real> targetX, Grid<Real> targetY, Grid<Real> targetZ);
def copy_vec3_to_real(source, targetX, targetY, targetZ):
	output = (source, targetX, targetY, targetZ)
	funcCall = manta_copy_vec3_to_real(source, targetX, targetY, targetZ)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void copyRealToVec3(Grid<Real> sourceX, Grid<Real> sourceY, Grid<Real> sourceZ, Grid<Vec3> target);
def copy_real_to_vec3(sourceX, sourceY, sourceZ, target):
	output = (sourceX, sourceY, sourceZ, target)
	funcCall = manta_copy_real_to_vec3(sourceX, sourceY, sourceZ, target)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void convertLevelsetToReal(LevelsetGrid source, Grid<Real> target);
def convert_levelset_to_real(source, target):
	output = (source, target)
	funcCall = manta_convert_levelset_to_real(source, target)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void swapComponents(Grid<Vec3> vel, const int c1=0, const int c2=1, const int c3=2);
def swap_components(vel, c1, c2, c3):
	return tf.assign(vel, manta_swap_components(vel, c1, c2, c3))

# void resetUvGrid(Grid<Vec3> target);
def reset_uv_grid(target):
	return tf.assign(target, manta_reset_uv_grid(target))

# void updateUvWeight(const Real resetTime, const int index, const int numUvs, Grid<Vec3> uv);
def update_uv_weight(resetTime, index, numUvs, uv):
	return tf.assign(uv, manta_update_uv_weight(resetTime, index, numUvs, uv))

# void getComponent(const Grid<Vec3> source, Grid<Real> target, const int component);
def get_component(source, target, component):
	return tf.assign(target, manta_get_component(source, target, component))

# void setComponent(const Grid<Real> source, Grid<Vec3> target, const int component);
def set_component(source, target, component):
	return tf.assign(target, manta_set_component(source, target, component))

# void densityFromLevelset(LevelsetGrid phi, Grid<Real> density, const Real value=1.0, const Real sigma=1.0);
def density_from_levelset(phi, density, value, sigma):
	output = (phi, density)
	funcCall = manta_density_from_levelset(phi, density, value, sigma)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void getSpiralVelocity2D(const FlagGrid flags, MACGrid vel, const Real strength=1.0);
def get_spiral_velocity2d(flags, vel, strength):
	return tf.assign(vel, manta_get_spiral_velocity2d(flags, vel, strength))

# void setGradientYWeight(Grid<Real> W, const int minY, const int maxY, const Real valAtMin, const Real valAtMax);
def set_gradient_y_weight(W, minY, maxY, valAtMin, valAtMax):
	return tf.assign(W, manta_set_gradient_y_weight(W, minY, maxY, valAtMin, valAtMax))

# void PD_fluid_guiding(MACGrid vel, MACGrid velT, Grid<Real> pressure, FlagGrid flags, Grid<Real> weight, const int blurRadius=5, const Real theta=1.0, const Real tau=1.0, const Real sigma=1.0, const Real epsRel=1e-3, const Real epsAbs=1e-3, const int maxIters=200, Grid<Real> phi=0, Grid<Real> perCellCorr=0, MACGrid fractions=0, const Real gfClamp=1e-04, const Real cgMaxIterFac=1.5, const Real cgAccuracy=1e-3, const int preconditioner=1, const bool zeroPressureFixing=false);
def pd_fluid_guiding(vel, velT, pressure, flags, weight, blurRadius, theta, tau, sigma, epsRel, epsAbs, maxIters, phi, perCellCorr, fractions, gfClamp, cgMaxIterFac, cgAccuracy, preconditioner, zeroPressureFixing):
	output = (vel, velT, pressure, flags, weight, phi, perCellCorr, fractions)
	funcCall = manta_pd_fluid_guiding(vel, velT, pressure, flags, weight, blurRadius, theta, tau, sigma, epsRel, epsAbs, maxIters, phi, perCellCorr, fractions, gfClamp, cgMaxIterFac, cgAccuracy, preconditioner, zeroPressureFixing)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void cgSolveDiffusion(const FlagGrid flags, MACGrid grid, const Real alpha=0.25, const Real cgMaxIterFac=1.0, const Real cgAccuracy=1e-4);
def cg_solve_diffusion_mac_grid(flags, grid, alpha, cgMaxIterFac, cgAccuracy):
	return tf.assign(grid, manta_cg_solve_diffusion_mac_grid(flags, grid, alpha, cgMaxIterFac, cgAccuracy))

# void cgSolveDiffusion(const FlagGrid flags, Grid<Vec3> grid, const Real alpha=0.25, const Real cgMaxIterFac=1.0, const Real cgAccuracy=1e-4);
def cg_solve_diffusion_grid_vec3(flags, grid, alpha, cgMaxIterFac, cgAccuracy):
	return tf.assign(grid, manta_cg_solve_diffusion_grid_vec3(flags, grid, alpha, cgMaxIterFac, cgAccuracy))

# void cgSolveDiffusion(const FlagGrid flags, Grid<float> grid, const Real alpha=0.25, const Real cgMaxIterFac=1.0, const Real cgAccuracy=1e-4);
def cg_solve_diffusion_gridfloat(flags, grid, alpha, cgMaxIterFac, cgAccuracy):
	return tf.assign(grid, manta_cg_solve_diffusion_gridfloat(flags, grid, alpha, cgMaxIterFac, cgAccuracy))

# void applyEmission(FlagGrid flags, Grid<Real> density, Grid<Real> emission, const bool isAbsolute);
def apply_emission(flags, density, emission, isAbsolute):
	output = (flags, density, emission)
	funcCall = manta_apply_emission(flags, density, emission, isAbsolute)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void checkSymmetry(Grid<Real> a, Grid<Real> err=NULL, const bool symmetrize=false, const int axis=0, const int bound=0);
def check_symmetry(a, err, symmetrize, axis, bound):
	output = (a, err)
	funcCall = manta_check_symmetry(a, err, symmetrize, axis, bound)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void checkSymmetryVec3(Grid<Vec3> a, Grid<Real> err=NULL, const bool symmetrize=false, const int axis=0, const int bound=0, const int disable=0);
def check_symmetry_vec3(a, err, symmetrize, axis, bound, disable):
	output = (a, err)
	funcCall = manta_check_symmetry_vec3(a, err, symmetrize, axis, bound, disable)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void updateFractions(const FlagGrid flags, const Grid<Real> phiObs, MACGrid fractions, const int boundaryWidth=0);
def update_fractions(flags, phiObs, fractions, boundaryWidth):
	return tf.assign(fractions, manta_update_fractions(flags, phiObs, fractions, boundaryWidth))

# void setObstacleFlags(FlagGrid flags, const Grid<Real> phiObs, const MACGrid fractions=NULL, const Grid<Real> phiOut=NULL);
def set_obstacle_flags(flags, phiObs, fractions, phiOut):
	return tf.assign(flags, manta_set_obstacle_flags(flags, phiObs, fractions, phiOut))

# void initVortexVelocity(Grid<Real> phiObs, MACGrid vel, const Vec3 center, const Real radius);
def init_vortex_velocity(phiObs, vel, center, radius):
	output = (phiObs, vel)
	funcCall = manta_init_vortex_velocity(phiObs, vel, center, radius)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void processBurn(Grid<Real> fuel, Grid<Real> density, Grid<Real> react, Grid<Real> red=NULL, Grid<Real> green=NULL, Grid<Real> blue=NULL, Grid<Real> heat=NULL, const Real burningRate=0.75f, const Real flameSmoke=1.0f, const Real ignitionTemp=1.25f, const Real maxTemp=1.75f, const Real dt=0.1f, const Vec3 flameSmokeColor=Vec3(0.7f, 0.7f, 0.7f));
def process_burn(fuel, density, react, red, green, blue, heat, burningRate, flameSmoke, ignitionTemp, maxTemp, dt, flameSmokeColor):
	output = (fuel, density, react, red, green, blue, heat)
	funcCall = manta_process_burn(fuel, density, react, red, green, blue, heat, burningRate, flameSmoke, ignitionTemp, maxTemp, dt, flameSmokeColor)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void updateFlame(Grid<Real> react, Grid<Real> flame);
def update_flame(react, flame):
	output = (react, flame)
	funcCall = manta_update_flame(react, flame)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void quantizeGrid(Grid<Real> grid, const Real step);
def quantize_grid(grid, step):
	return tf.assign(grid, manta_quantize_grid(grid, step))

# void quantizeGridVec3(Grid<Vec3> grid, const Real step);
def quantize_grid_vec3(grid, step):
	return tf.assign(grid, manta_quantize_grid_vec3(grid, step))

# void solvePressure(MACGrid vel, Grid<Real> pressure, const FlagGrid flags, const Real cgAccuracy=1e-3, const Grid<Real> phi=0, const Grid<Real> perCellCorr=0, const MACGrid fractions=0, const Real gfClamp=1e-04, const Real cgMaxIterFac=1.5, const bool precondition=true, const int preconditioner=PcMIC, const bool enforceCompatibility=false, const bool useL2Norm=false, const bool zeroPressureFixing=false, const Grid<Real> curv=NULL, const Real surfTens=0., Grid<Real> retRhs=NULL);
def solve_pressure(vel, pressure, flags, cgAccuracy, phi, perCellCorr, fractions, gfClamp, cgMaxIterFac, precondition, preconditioner, enforceCompatibility, useL2Norm, zeroPressureFixing, curv, surfTens, retRhs):
	output = (vel, pressure, retRhs)
	funcCall = manta_solve_pressure(vel, pressure, flags, cgAccuracy, phi, perCellCorr, fractions, gfClamp, cgMaxIterFac, precondition, preconditioner, enforceCompatibility, useL2Norm, zeroPressureFixing, curv, surfTens, retRhs)
	return tf.contrib.framework.nest.map_structure(lambda state, var: tf.assign(var, state), funcCall, output, check_types=False)

# void solvePressurePart1(const MACGrid vel, Grid<Real> pressure, const FlagGrid flags, const Real cgAccuracy=1e-3, const Grid<Real> phi=0, const Grid<Real> perCellCorr=0, const MACGrid fractions=0, const Real gfClamp=1e-04, const Real cgMaxIterFac=1.5, const bool precondition=false, const int preconditioner=PcMIC, const bool enforceCompatibility=false, const bool useL2Norm=false, const bool zeroPressureFixing=false, const Grid<Real> curv=NULL, const Real surfTens=0.);
def solve_pressure_part1(vel, pressure, flags, cgAccuracy, phi, perCellCorr, fractions, gfClamp, cgMaxIterFac, precondition, preconditioner, enforceCompatibility, useL2Norm, zeroPressureFixing, curv, surfTens):
	return tf.assign(pressure, manta_solve_pressure_part1(vel, pressure, flags, cgAccuracy, phi, perCellCorr, fractions, gfClamp, cgMaxIterFac, precondition, preconditioner, enforceCompatibility, useL2Norm, zeroPressureFixing, curv, surfTens))

# void solvePressurePart2(MACGrid vel, const Grid<Real> pressure, const FlagGrid flags, const Real cgAccuracy=1e-3, const Grid<Real> phi=0, const Real gfClamp=1e-04, const Grid<Real> curv=NULL, const Real surfTens=0.);
def solve_pressure_part2(vel, pressure, flags, cgAccuracy, phi, gfClamp, curv, surfTens):
	return tf.assign(vel, manta_solve_pressure_part2(vel, pressure, flags, cgAccuracy, phi, gfClamp, curv, surfTens))

