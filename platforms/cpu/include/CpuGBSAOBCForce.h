
/* Portions copyright (c) 2006-2013 Stanford University and Simbios.
 * Contributors: Pande Group
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS, CONTRIBUTORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef OPENMM_CPU_GBSAOBC_FORCE_H__
#define OPENMM_CPU_GBSAOBC_FORCE_H__

#include "openmm/internal/ThreadPool.h"
#include "openmm/internal/vectorize.h"
#include <set>
#include <utility>
#include <vector>

namespace OpenMM {

class CpuGBSAOBCForce {
public:
    class ComputeTask;
    CpuGBSAOBCForce();

    /**
     * Set the force to use a cutoff.
     * 
     * @param distance    the cutoff distance
     */
    void setUseCutoff(float distance);

    /**
     * 
     * Set the force to use periodic boundary conditions.  This requires that a cutoff has
     * already been set, and the smallest side of the periodic box is at least twice the cutoff
     * distance.
     *
     * @param boxSize             the X, Y, and Z widths of the periodic box
     */
    void setPeriodic(float* periodicBoxSize);

    /**
     * Set the solute dielectric constant.
     */
    void setSoluteDielectric(float dielectric);

    /**
     * Set the solvent dielectric constant.
     */
    void setSolventDielectric(float dielectric);
    
    /**
     * Get the per-particle parameters (offset radius, scaled radius).
     */
    const std::vector<std::pair<float, float> >& getParticleParameters() const;
    
    /**
     * Set the per-particle parameters (offset radius, scaled radius).
     */
    void setParticleParameters(const std::vector<std::pair<float, float> >& params);

    /**
     * 
     * Calculate LJ Coulomb pair ixn
     *
     * @param posq             atom coordinates and charges
     * @param forces           force array (forces added)
     * @param totalEnergy      total energy
     * @param threads          the thread pool to use
     */
    void computeForce(const std::vector<float>& posq, std::vector<std::vector<float> >& threadForce, double* totalEnergy, ThreadPool& threads);

    /**
     * This routine contains the code executed by each thread.
     */
    void threadComputeForce(ThreadPool& threads, int threadIndex);

private:
    bool cutoff;
    bool periodic;
    float periodicBoxSize[3];
    float cutoffDistance, soluteDielectric, solventDielectric;
    std::vector<std::pair<float, float> > particleParams;        
    std::vector<float> bornRadii;
    std::vector<std::vector<float> > threadBornForces;
    std::vector<float> obcChain;
    std::vector<double> threadEnergy;
    // The following variables are used to make information accessible to the individual threads.
    float const* posq;
    std::vector<std::vector<float> >* threadForce;
    bool includeEnergy;
  
  /**
   * Compute the displacement and squared distance between two points, optionally using
   * periodic boundary conditions.
   */
  void getDeltaR(const fvec4& posI, const fvec4& posJ, fvec4& deltaR, float& r2, bool periodic, const fvec4& boxSize, const fvec4& invBoxSize) const;
};

} // namespace OpenMM

// ---------------------------------------------------------------------------------------

#endif // OPENMM_CPU_GBSAOBC_FORCE_H__
