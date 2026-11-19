# Need for Closure — Model and Program Description

## Program Purpose

`need4closure` is an agent-based simulation example written in C++ using the SymShell library. The program implements a simplified, Ari Kruglanski theory-inspired social model describing opinion formation, change, and the role of the need for closure.

The entry point is `need4clos.cpp`. This file reads the parameters, launches the graphical window, and creates the `kWorld` world object. The agent definition is in `kAgent.h`, while the world logic and the simulation step are implemented in `kworld.cpp` and `kvisual.cpp`.

## World Model

The world is a square grid with dimensions `WIDTH × WIDTH` (default `100 × 100`) and toroidal geometry: opposite edges of the grid are adjacent to each other. A cell may contain an agent or be empty. The initial occupancy of the grid is controlled by `FILL`.

The initial distribution of attitudes is sampled from probabilities specified by `ILEF` and `IRIG`; the remaining agents begin undecided. The default values are 5% and 2.5%, respectively.

## Agent and Its State

`kAgent` inherits from `agent_base`. Each agent stores:

- `First` — the current attitude: `-1` (left), `0` (neutral), `1` (right);
- `Second` — the previous or prepared attitude, used during updates and visualization;
- `Power` — individual strength, randomized when created within the range from 1 to the maximum strength;
- `ForLeft` and `ForRight` — counters/strengths of arguments for the left and right attitudes;
- `DurCh` — a marker that an attitude change has been calculated and is awaiting application.

At the beginning, attitude is randomized according to the `ILEF` and `IRIG` parameters, and both argument counters receive random values. During a change, `new_attitude()` stores the proposed attitude and, when updating synchronously, marks it for application in the next pass.

## Interaction Rule and Attitude Change

At every step, the world visits the agents and, for every active agent, randomly draws partners from the neighborhood. The number of samples is controlled by `PRTR`, and the `NFOC` parameter is passed to the neighborhood generator.

The partner's influence depends on their strength, the need-for-closure parameter, and distance. In `CheckChange()`, the code calculates it as `Power^NFOC / √distance`; the agent's own contribution is weighted by `SELF`.

If the absolute difference between the counters exceeds `TRSP` (threshold, default `100`), the agent adopts the attitude of the side with the larger counter. Otherwise, it retains its current attitude.

Two update orders are available:

- **Synchronous** (`SYNC=Y`) — first, proposed changes for all agents are calculated, and only in the second pass are those changes applied;
- **Monte Carlo** (`SYNC=N`) — agents are selected randomly, and the calculated change is applied immediately; in this mode, an agent may be selected again.

## Program Execution and Results

`main()` in `need4clos.cpp` performs the following actions:

1. Parses the startup parameters and initializes the SymShell window manager.
2. Creates `kWorld`, initializes the random generator, and sets iteration limits and visualization and logging frequencies.
3. Initializes the agent layers and visualization, then starts the simulation loop. In `REPL=...` mode, it replays a saved history instead of starting a new simulation.
4. At the end, releases the world object.

By default, the simulation has a very large step limit (`MAX`), so it may run until manually interrupted. `STOP=Y` ends it after reaching the limit. `AUTO=N` starts a series of automatic runs.

The window presents a map of current and previous attitudes, maps of argument counters and strength, an attitude histogram, and history charts for, among others, the number of classes, entropy, changes, and approximate cluster size.

## Most Important Startup Parameters

Options are supplied as `NAME=VALUE` arguments; letter case does not matter. The most important ones are:

| Parameter | Meaning |
| --- | --- |
| `WIDTH=N` | Side length of the world grid. Default 100. |
| `FILL=x` | Initial proportion of occupied cells; values greater than 1 are interpreted as percentages. |
| `ILEF=x`, `IRIG=x` | Initial proportions of left-wing and right-wing attitudes; values greater than 1 are interpreted as percentages. |
| `NFOC=x` | Need-for-closure parameter, affecting the range/weighting of interactions. |
| `SELF=x` | Weight of the agent's own contribution. Default 9. |
| `PRTR=N` | Number of randomly selected partners; the code also accepts `PRTR=-1` as a "all neighbors" variant. |
| `TRSP=N` | Counter-difference threshold required to change attitude. |
| `MPOW=N` | Maximum strength during agent initialization. |
| `SYNC=Y/N` | Synchronous or Monte Carlo update order. |
| `MAX=N`, `STOP=Y/N` | Step limit and whether to terminate after reaching it. |
| `VIEW=N`, `LOGC=N` | Visualization refresh frequency and log-write frequency. |
| `LOGF=file`, `HIST=file`, `REPL=file` | Log file, history file, or history replay file. |
| `MAPL=file`, `MAPP=file`, `MASK=file` | Initial bitmap files and occupancy mask. |
| `NOIP=N`, `SPCH=N`, `PMIG=N` | Noise, spontaneous-change, and migration parameters available in the program interface. |

### Implementation Notes

Not all parameters described by the parser are visible in the attitude-change rule. In particular, `Noise`, `MutationProb`, and `MigrationP` are stored/passed through, but in the current code...

## Main Files in the `src/` Directory

- `need4clos.cpp` — entry point, application parameters, and execution control.
- `kAgent.h` — agent state, initialization, and operations.
- `kWorld.h` — world and parameter declarations and agent layers.
- `kworld.cpp` — world initialization, one-step logic, and interaction rule.
- `kvisual.cpp` — data series, statistics, charts, and maps.

## Interpretation Scope

The description of the rules above reflects the source code of this example. It is a simplified model and the parameters or update mechanism should not be equated with the full, empirically verified model of need for closure.
