# Gamma Labs Simulation Software
Simulation software and set up for attentuation and absolute determination experiments for the PHYS5036 labs.

### GammaLabMC generated-event cut

The GammaLabMC runner generates the requested event list first, then retains
only gamma rays whose unscattered path comes within a crystal-specific margin
of the detector housing before Geant4 transport. The hard-coded housing radii
are 14 mm for GAGG and 28.596 mm for NaI, with additional margins of 5 mm and
10 mm respectively. These values are deliberately explicit in
`GenerateSpectrum.C` so they can be tuned and validated without adding user
configuration.
The source-holder and detector distances used by the cut are taken from the
same run settings as the Geant4 geometry.

The generated ROOT input records `GeneratedEventCount` (before the cut) and
`TransportedEventCount` (after the cut). The simulation output preserves both
counts as ROOT parameters with the same names. Since the runner removes its
intermediate ROOT files after smearing, it also saves both values in
`<simulation-basename>_event_counts.txt` in `OUTPUT_DIR`. Use
`GeneratedEventCount` as `N_generated` when calculating absolute efficiency;
the number of transported events is not the efficiency denominator.

The enlarged acceptance includes direct rays toward the casing and some
nearby scatter-in events, but does not guarantee retention of every event that
scatters into the crystal from upstream materials. The margins are starting
values, not a validated uncertainty bound. Compare cut and uncut reference runs
for each experimental configuration and increase the margins until the absolute
efficiency is stable before using this optimization for absolute activity
results.

Each source spectrum file lists every simulated gamma line and its photon yield
per decay. Yields below one include decays with no photon in the listed lines;
yields above one are sampled as an average number of photons per decay. Keep
all line yields in the same units when editing a source spectrum.
