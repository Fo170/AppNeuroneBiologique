# AGENTS.md — AppNeuroneBiologique

## Build & run

```bash
cd qtgui/build
cmake .. && make -j$(nproc)
./NeuroneBiologiqueGUI
```

Pre-built binary at `qtgui/build/NeuroneBiologiqueGUI`.

Dependencies: Qt6 Widgets, CMake ≥ 3.16, C++17 compiler. `CMAKE_AUTOMOC` is ON. No Qt6 Core/Qt6 Gui needed separately — Widgets pulls them.

## Architecture

Single executable, no monorepo. All source is in `qtgui/`.

| Layer | Files | Role |
|-------|-------|------|
| Entry | `main.cpp` → `MainWindow.{hpp,cpp}` | QApplication + main window |
| Scene | `GraphScene.{hpp,cpp}`, `NeuroneNode.{hpp,cpp}`, `SynapseEdge.{hpp,cpp}` | Visual editor on QGraphicsScene |
| Model | `ReseauNeural.{hpp,cpp}` | Network simulation engine |
| Data | `Dataset.{hpp,cpp}` | CSV dataset load/save |
| Neuron | `NeuroneBiologique.hpp` | **Header-only** neuron model (leaky-integrate + sigmoid + DAN learning) |

`NeuroneBiologique.hpp` is a copy of the standalone library at `github.com/Fo170/NeuroneBiologique` — zero dependencies beyond `<cmath>`, usable independently from the GUI. The upstream repo has API docs, math details, and usage examples.

`ReseauNeural.cpp` uses its own **delta rule** implementation (not the DAN rule from `NeuroneBiologique.hpp`). All computation is `float`, with backpropagation through hidden layers.

## Key conventions

- **CSV format for datasets**: first line `# nb_entrees=N` header, then examples (multi-target supported).
- **CSV format for networks**: sections `[NEURONES]` and `[SYNAPSES]`.
- **Simulation**: F5 or toolbar button starts/stops. `QTimer(20ms)` processes **10 epochs per tick** for visible evolution. Synapses and outputs update live in the scene.
- **Dataset tab** (1er onglet du PropertyPanel): table éditable (entrées + cibles + sortie + erreur), courbe d'erreur live auto-scalée, seuil d'arrêt, bouton Arrêter.
- **Learning rule**: delta rule `Δw = η × (cible - sortie) × pre` for output, linear backprop `δ_hidden = Σ(δ_output × w)` (no σ' derivative) for hidden layers. Bias delta `Δb = η × (cible - sortie)` (output) or `Δb = Σ(δ_output × w)` (caché). **Batch gradient** : les deltas sont accumulés sur tout le dataset, puis la moyenne est appliquée une fois par epoch.
- **Sigmoid**: `σ(v) = 0.005 + 0.99 / (1 + exp(-10(v-0.5)))` — 0.5% spontaneous baseline activity (biologically realistic).
- **Synapse types** (biologically proportioned): axo-dendritic 66.6%, axo-axonic 25.8%, dendro-dendritic 5.8%, dendro-axonic 1.8%.
- **Example files**: `qtgui/exemples/datasets/` (8 CSVs) and `qtgui/exemples/reseaux/` (7 CSVs). Dataset `74ls181` has a generator script at `qtgui/exemples/generate_74ls181.py`.
- **Splitter**: 75% view / 25% panel via `setSizes({900, 300})`.
- **File dialogs** default to `exemples/reseaux/` for networks, `exemples/datasets/` for datasets.
- **Controls**: "Max itérations" (toolbar, defaut 100, max 1 000 000), "Batch" (panel, defaut 10 iterations/refresh), "Seuil erreur" (panel, défaut 0.001).

## No tests / lint / CI

None found in repo. No test framework, no CI config, no formatter config.

## Changelog (recent)

| Date | Change |
|------|--------|
| — | `FullViewportUpdate` → `MinimalViewportUpdate` (fix crash on Linux) |
| — | All computation converted to `float` |
| — | Delta rule (DAN/Hebb removed); linear backprop without σ' derivative |
| — | Sigmoid baseline `σ_min = 0.005` |
| — | Bias clamped to `[-2, +2]`, weights clamped to `[-5, +5]` |
| — | Weight decay removed (clamps suffice) |
| — | Batch gradient : accumulation + moyenne sur le dataset |
| — | Fix `η²` bug dans le gradient caché (enlevé le η en trop) |
| — | `randomiser_poids()` randomise aussi les biais |
| — | Couleur neurones : bleu→blanc→rouge |
| — | Polices emoji en fallback (`Noto Color Emoji`, `Symbola`, `Segoe UI Emoji`) |
| — | Batch 10 epochs/tick (was 1) |
| — | Splitter 75/25 with `setSizes` |
| — | Dataset + Apprentissage tabs merged into single Dataset tab |
| — | Timer interval 20ms (was 0) |
| — | File dialogs default to `exemples/` subdirs |
