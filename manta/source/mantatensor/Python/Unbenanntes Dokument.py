
//! Perform semi-lagrangian advection of target Real- or Vec3 grid
//! Open boundary handling needs information about width of border
//! Clamping modes: 1 regular clamp leading to more overshoot and sharper results, 2 revert to 1st order slightly smoother less overshoot (enable when 1 gives artifacts)
PYTHON() void advectSemiLagrange (FlagGrid* flags, MACGrid* vel, GridBase* grid,
						   int order = 1, Real strength = 1.0, int orderSpace = 1, bool openBounds = false, int boundaryWidth = 1, int clampMode = 1)
{    
	assertMsg(order==1 || order==2, "AdvectSemiLagrange: Only order 1 (regular SL) and 2 (MacCormack) supported");
	
	// determine type of grid    
	if (grid->getType() & GridBase::TypeReal) {
		fnAdvectSemiLagrange< Grid<Real> >(flags->getParent(), *flags, *vel, *((Grid<Real>*) grid), order, strength, orderSpace, openBounds, boundaryWidth, clampMode);
	}
	else if (grid->getType() & GridBase::TypeMAC) {    
		fnAdvectSemiLagrange< MACGrid >(flags->getParent(), *flags, *vel, *((MACGrid*) grid), order, strength, orderSpace, openBounds, boundaryWidth, clampMode);
	}
	else if (grid->getType() & GridBase::TypeVec3) {    
		fnAdvectSemiLagrange< Grid<Vec3> >(flags->getParent(), *flags, *vel, *((Grid<Vec3>*) grid), order, strength, orderSpace, openBounds, boundaryWidth, clampMode);
	}
	else
		errMsg("AdvectSemiLagrange: Grid Type is not supported (only Real, Vec3, MAC, Levelset)");    
}
