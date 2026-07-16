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
| Scene | `GraphScene.{hpp,cpp}`, `NeuroneNode.{hpp,cpp}`, `SynapseEdge.{hpp,cpp}`, `ComponentPalette.{hpp,cpp}` | Visual editor on QGraphicsScene |
| Model | `ReseauNeural.{hpp,cpp}` | Network simulation engine |
| Data | `Dataset.{hpp,cpp}` | CSV dataset load/save |
| Neuron | `NeuroneBiologique.hpp` | **Header-only** neuron model (leaky-integrate + sigmoid + DAN learning) |

`NeuroneBiologique.hpp` is a copy of the standalone library at `github.com/Fo170/NeuroneBiologique` — zero dependencies beyond `<cmath>`, usable independently from the GUI. The upstream repo has API docs, math details, and usage examples.

`ReseauNeural.cpp` uses its own **delta rule** implementation (not the DAN rule from `NeuroneBiologique.hpp`). All computation is `float`, with backpropagation through hidden layers.

## Key conventions

- **CSV format for datasets**: first line `# nb_entrees=N` header, then examples (multi-target supported).
- **CSV format for networks**: sections `[NEURONES]` and `[SYNAPSES]`. Each neuron has `col_entree,col_sortie` fields (default -1 = positional).
- **Module format (JSON)**: sections `neurones` and `synapses` with type `"module"`. Ext `.neuron`.
- **Simulation**: F5 or toolbar button starts/stops. `QTimer(20ms)` processes **10 epochs per tick** for visible evolution. Synapses and outputs update live in the scene.
- **Stop button**: flag `stop_demande_` vérifié entre chaque epoch pour réactivité sur les gros modèles.
- **Dataset tab** (1er onglet du PropertyPanel): table éditable (entrées + cibles + sortie + erreur), courbe d'erreur live auto-scalée, seuil d'arrêt, bouton Arrêter rouge.
- **Component palette** (panneau latéral gauche) : cliquer un composant → mode placement → clic sur la scène pour poser neurone ou module. Sélectionner des éléments → clic droit → "Exporter la sélection en module" pour créer un module réutilisable. Fichier → Importer un module pour ajouter des modules externes.
- **RubberBandDrag** (au lieu de ScrollHandDrag) : sélection multiple par rectangle ; panorama par clic molette + glisser.
- **Pins** : clic sur pin sortie → glisser fil → relâcher sur pin entrée pour créer synapse. Surbrillance orange (source) et vert (cible).
- **Mapping colonnes** : clic droit sur neurone → menu pour choisir colonne dataset. Stocké dans `colonne_entree`/`colonne_sortie` des `NeuroneInfo`.
- **Learning rule**: delta rule `Δw = η × (cible - sortie) × pre` for output, linear backprop `δ_hidden = Σ(δ_output × w)` (no σ' derivative) for hidden layers. Bias delta `Δb = η × (cible - sortie)` (output) or `Δb = Σ(δ_output × w)` (caché). **Batch gradient** : les deltas sont accumulés sur tout le dataset, puis la moyenne est appliquée une fois par epoch.
- **Sigmoid**: `σ(v) = 0.005 + 0.99 / (1 + exp(-10(v-0.5)))` — 0.5% spontaneous baseline activity (biologically realistic).
- **Synapse types** (biologically proportioned): axo-dendritic 66.6%, axo-axonic 25.8%, dendro-dendritic 5.8%, dendro-axonic 1.8%.
- **Couleur synapses**: dégradé bleu (poids -5) → gris clair (poids 0) → rouge (poids +5).
- **Couleur neurones**: dégradé bleu (inactif) → blanc (50%) → rouge (actif).
- **Example files**: `qtgui/exemples/datasets/` (8 CSVs) and `qtgui/exemples/reseaux/` (7 CSVs). Dataset `74ls181` has a generator script at `qtgui/exemples/generate_74ls181.py`.
- **Splitter**: palette 180px | view 720px | panel 300px via `setSizes`.
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
| — | **Palette de composants** (ComponentPalette) : panneau gauche avec neurones de base + modules ; clic → placement sur la scène |
| — | **Modules réutilisables** : export de la sélection en `.neuron` (JSON), import depuis Fichier → Importer un module |
| — | **Molette sur synapse** : sélectionner une synapse → molette pour ajuster le poids (pas 0.05) |
| — | **Clic-droit-glissé** : créer une synapse en glissant d'un neurone à un autre |
| — | **Touche Suppr** : supprimer l'élément sélectionné (neurone ou synapse) |
| — | **Pin connection** : clic sur pin sortie → fil élastique → relâcher sur neurone cible. Surbrillance des pins (orange/vert) |
| — | **RubberBandDrag** : sélection multiple par rectangle ; panorama par clic molette |
| — | **Batch gradient fix** : accumulation + moyenne sur dataset (convergence XOR/XNOR) |
| — | **Stop button reactif** : flag `stop_demande_` vérifié entre chaque epoch |
| — | **Mapping colonnes dataset** : clic droit sur neurone → choisir colonne. Stocké dans NeuroneInfo, sauvegardé en CSV |
| — | **Info colonne sous neurone** : bannière avec nom de colonne dataset |
| — | **Filenames in title/status bar** : réseau + dataset dans barre titre et statut |
| — | **Couleur synapses** : bleu→gris→rouge (au lieu de rouge/vert binaire) |
| — | **Synapse sélectionnée en vert** (au lieu de bleu) |
| — | **Bouton Simuler vert** + **Arrêter rouge** (textes stylés) |
