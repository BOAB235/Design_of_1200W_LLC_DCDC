import os

# -----------------------------------------------------------------------------
# Folder structure definition for a DC-DC converter hardware project
# -----------------------------------------------------------------------------
structure = [
    "01_specifications",                         # functional, electrical, thermal specs
    "02_architecture",                           # topology diagrams, block-level description

    "03_hw_design",                              # hardware design main folder

    # design justification: calculations, component selection, theory
    "03_hw_design/01_design_justification",
    "03_hw_design/01_design_justification/01_llc_tank",           # LLC tank resonance and sizing
    "03_hw_design/01_design_justification/02_pcb_calculations",   # trace width, thermal via sizing
    "03_hw_design/01_design_justification/03_output_capacitor",   # output ripple and ESR selection
    "03_hw_design/01_design_justification/04_measurements",       # lab measurements for validation
    "03_hw_design/01_design_justification/05_gate_driver",        # driver sizing, timing
    "03_hw_design/01_design_justification/06_emi_filters",        # filter design, common/diff mode

    # schematic and BOM
    "03_hw_design/02_schematic_and_bom",        # schematics, netlist, bill of materials

    "04_sw_design",                              # firmware, digital control (if applicable)

    "05_validations",                            # all lab test results and setups
    "05_validations/01_functional",              # load tests, waveforms, startup behavior
    "05_validations/02_emc",                     # emi test results and limits
    "05_validations/03_safety",                  # safety clearances, protection validation

    # optional reports folder (disabled here)
    # "06_reports",                              # final pdf reports, presentation material

    "99_archive"                                 # deprecated drafts, backup data
]

# -----------------------------------------------------------------------------
# Folder creation logic (relative to current working directory)
# -----------------------------------------------------------------------------
base_path = os.getcwd()  # or set manually, e.g., r"C:\projects\dcdc_converter"

for folder in structure:
    path = os.path.join(base_path, folder)
    os.makedirs(path, exist_ok=True)
    print(f"Created: {path}")
