# Convince — influence-change simulation

`convince` is an interactive agent-based example implementing a model associated with D. Stauffer. The simulation runs on a square toroidal lattice, where agents have no opinion (`0`) or one of several opinion categories. It uses SymShell/RTM for visualization, history, and statistics.

## Model

- The default lattice is `50 × 50` (`WIDTH × WIDTH`). Opposite edges are neighbours.
- Each agent stores its current belief `First`, previous belief `Second`, and persuasion power `Power`.
- State `0` means no opinion.
- At initialization, an agent remains with state `0` with probability `1 - NewProbability`; otherwise it receives a random category from `1` to `NumOfCate - 1`.
- A Monte Carlo step performs `WIDTH × WIDTH` random agent trials.
- For an opinionated agent, the program checks the Moore neighbourhood (up to eight neighbours).
- With neighbours holding the same opinion, there is probability `ReverseProb - SupportLevel` of losing the opinion.
- With no same-opinion neighbour, the probability of losing it is `ReverseProb`.
- A neutral agent spontaneously adopts a random opinion with probability `NewProbability`.
- If two same-opinion agents are chosen as a support pair, neutral agents in a local rectangle between them can adopt that opinion with probability `NewInfectProb`.
- Agent power is sampled uniformly from `MinStrength ... MaxStrength` when the limits differ. It is visualized but is not directly used by the current update rule.

The constructor links CLI-independent values as follows:

| Static field | Value in `convince.cpp` | Default |
| --- | --- | ---: |
| `ToBeNewProb` | `NewProbability` | `0.01` |
| `NewInfectProb` | `InfectionProb` | `0.5` |
| `SupportLevel` | `SupportLev` | `0.9` |
| `ReverseProb` | `1 - NewProbability` | `0.99` |

## Build

Requirements: C++11, CMake, GCC or Clang, X11/Xpm on Linux, or native Windows graphics.

```bash
git clone --recursive https://github.com/borkowsk/symShell2andRTM.git
cd symShell2andRTM
cmake -S EXAMPLES/convince -B build/convince
cmake --build build/convince --target convince
```

## Run

```bash
./build/convince/convince
```

Options use `NAME=VALUE`, for example:

```bash
./build/convince/convince WIDTH=80 WIDTHWIN=900 HEIGHTWIN=650 MPOW=10 MAX=5000 VIEW=2 LOGC=5 STOP=Y LOGF=convince.log
```

Important options:

| Option | Meaning | Default |
| --- | --- | ---: |
| `WIDTH` | lattice width and height (`>= 3`, below window width) | `50` |
| `MPOW` | maximum and initialization strength | `1` |
| `MAX` | maximum number of Monte Carlo steps; must be positive | `0xffffffff` |
| `VIEW` | visualization refresh interval; must be positive | `1` |
| `LOGC` | log-write interval; must be positive | `1` |
| `ILOG` | internal history length; minimum 50 | `7000` |
| `WIDTHWIN`, `HEIGHTWIN` | initial window size; each at least 50 | `750 × 550` |
| `AUTO=N` | run N automatic restarts | `0` |
| `STOP=Y` | stop after `MAX` steps | `N` |
| `MAPL=file` | opinion initialization bitmap (GIF/BMP) | `convinces.gif` |
| `MAPP=file` | power initialization bitmap | `powers.gif` |
| `MASK=file` | black pixels remove agents | `mask.gif` |
| `LOGF=file` | text log | `attitude.log` |
| `HIST=file` | complete history | `convinces.otx` |
| `REPL=file` | replay a complete-history file instead of simulating | disabled |

`NewProbability`, `InfectionProb`, and `SupportLev` are currently constants in `src/convince.cpp`, not command-line options. The implementation has two categories fixed in `convWorld`; `CLSS` is commented out.

Without `AUTO`, the program waits for the interface before starting. `AUTO=N` runs N simulation passes, restarting the world between passes. `REPL=file` prepares a read-only replay instead of a new simulation.

## Source layout

- `src/convince.cpp`: entry point, parameters, CLI parsing, and execution loops.
- `src/cagent.h`: agent state and bitmap initialization.
- `src/cworld.h`, `src/cworld.cpp`: world, initialization, visualization, and update rules.
- `src/cworldimpi.cpp`, `src/cworldimpo.cpp`: state serialization.
- `src/crand.h`: random-generator selection.
- `src/resamngr.h`: interface menu/resource constants.

## Limitations

The results depend on random initialization and Monte Carlo order. The current model defaults to two opinion categories, and power is visualized and serialized but is not directly used in the current update formula. The default text log name is inherited from the older example, so `LOGF=convince.log` is recommended.

### @date 2026-11-19 (last modification)


