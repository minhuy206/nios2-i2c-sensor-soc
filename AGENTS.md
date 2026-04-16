# Repository Guidelines

## Project Structure & Module Organization
This repository combines Intel Quartus hardware design with a Nios II software application.

- Root HDL entry points: `final_project.v`, `sensor_status_regs.v`, `final_project.qpf`, `final_project.qsf`
- Generated Platform Designer system: `system/` and `system/synthesis/`
- Nios II application sources: `software/final_project/` (`main.c`, `i2c_hal.c`, `i2c_hal.h`)
- Generated BSP and HAL files: `software/final_project_bsp/`
- Quartus build outputs: `db/`, `incremental_db/`, `output_files/`

Treat `software/final_project_bsp/`, `db/`, `incremental_db/`, and most of `output_files/` as generated unless you are intentionally regenerating tool output.

## Build, Test, and Development Commands
- `make -C software/final_project_bsp` builds the Nios II BSP static libraries and generated support objects.
- `make -C software/final_project` builds the application ELF (`final_project.elf`).
- `make -C software/final_project_bsp clean` removes BSP object files.
- `make -C software/final_project clean` removes application build artifacts if the generated app makefile includes the standard clean target.

Quartus compilation is typically run from the GUI for `final_project.qpf`; generated reports appear in `output_files/`.

## Coding Style & Naming Conventions
Use 4-space indentation in both C and Verilog. Follow the existing C style: braces on the next line for functions, `snake_case` for functions and locals, `UPPER_CASE` for macros, and short defensive checks early in each function. Keep hardware module names descriptive and aligned with file names, for example `final_project` in `final_project.v`.

Do not mix hand-written logic into generated BSP or synthesis output files. Prefer small, focused changes in source files that can be regenerated cleanly.

## Testing Guidelines
There is no automated unit test suite in this checkout. Validate changes by rebuilding the BSP and app, then running the firmware on target hardware and checking JTAG UART output from `main.c`'s I2C smoke test. For HDL changes, perform a Quartus compile and review timing and fit reports in `output_files/`.

## Commit & Pull Request Guidelines
No `.git` history is present in this workspace, so no repository-specific commit convention could be inferred. Use short imperative subjects such as `Fix I2C probe error handling` or `Wire open-drain SDA/SCL correctly`.

Pull requests should state:
- what changed and why
- whether hardware, firmware, or both were affected
- how the change was validated (`make`, Quartus compile, board test)
- screenshots or report excerpts when timing, pinout, or console output changed

## Configuration Tips
This project expects Intel Quartus Prime 18.1 and the Nios II EDS toolchain (`nios2-elf-gcc`, BSP utilities) to be installed and on `PATH`.
