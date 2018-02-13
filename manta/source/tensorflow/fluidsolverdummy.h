/******************************************************************************
 *
 * MantaFlow fluid solver framework
 * Copyright 2011 Tobias Pfaff, Nils Thuerey 
 *
 * This program is free software, distributed under the terms of the
 * GNU General Public License (GPL) 
 * http://www.gnu.org/licenses
 *
 * Main class for the fluid solver
 *
 ******************************************************************************/

#ifndef _FLUIDSOLVERDUMMY_H
#define _FLUIDSOLVERDUMMY_H

#include "manta.h"
#include "vectorbase.h"
#include "vector4d.h"
#include <vector>
#include <map>

namespace Manta { 
	
class FluidSolverDummy : public FluidSolver {
public:
FluidSolver::FluidSolver(Vec3i gridsize, int dim, int fourthDim)
	: PbClass(this), mDt(1.0), mTimeTotal(0.), mFrame(0), 
	  mCflCond(1000), mDtMin(1.), mDtMax(1.), mFrameLength(1.),
	  mGridSize(gridsize), mDim(dim) , mTimePerFrame(0.), mLockDt(false), mFourthDim(fourthDim)
{
	if(dim==4 && mFourthDim>0) errMsg("Don't create 4D solvers, use 3D with fourth-dim parameter >0 instead.");
	assertMsg(dim==2 || dim==3, "Only 2D and 3D solvers allowed.");
	assertMsg(dim!=2 || gridsize.z == 1, "Trying to create 2D solver with size.z != 1");
}

FluidSolver::~FluidSolver() {
	mGridsInt.free();
	mGridsReal.free();
	mGridsVec.free();
	mGridsVec4.free();

	mGrids4dInt.free();
	mGrids4dReal.free();
	mGrids4dVec.free();
	mGrids4dVec4.free();
}


	FluidSolver(Vec3i gridSize, int dim=3, int fourthDim=-1) { 
	}
	virtual ~FluidSolver();

};

}

#endif
