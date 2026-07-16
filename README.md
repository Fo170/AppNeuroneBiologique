# NeuroneBiologiqueGUI — Éditeur CAO de réseaux de neurones biologiques

Interface graphique Qt6 pour concevoir, éditer et simuler des réseaux de neurones
biologiques, sur le principe d'un éditeur CAO.

## Dépendances

- Qt6 Widgets
- CMake ≥ 3.16
- Compilateur C++17

## Compilation

```bash
cd qtgui/build
cmake .. && make -j$(nproc)
./NeuroneBiologiqueGUI
```

## Fonctionnalités

### Datasets — création et édition libre
- **Tableau éditable** : ajouter, supprimer, modifier les valeurs directement dans l'interface.
  Colonnes entrées + cibles + sortie + erreur (lecture seule) visibles pendant l'apprentissage.
- **Multi-cibles** : plusieurs colonnes de sorties par exemple.
- **En-tête `# nb_entrees=N`** dans les fichiers CSV.
- **Courbe d'erreur live** : auto-scalée, seuil d'arrêt.
- Exemples inclus : and, or, nand, nor, xor, xnor, 74ls181 (dans `exemples/datasets/`).

### Réseaux — édition visuelle à la CAO
- **Ajout de neurones** : clic droit sur la scène → Neurone d'entrée / Neurone caché.
- **Liaison** : clic droit sur un neurone → Relier depuis ce neurone → clic droit sur le neurone cible → Lier.
- **Déplacement** : glisser-déposer des neurones sur la scène.
- **Suppression** : clic droit sur un neurone ou une synapse → Supprimer.
- **Panneau de propriétés** (3 onglets) :
  - *Dataset* : tableau éditable + courbe d'erreur + contrôles d'arrêt
  - *Neurone* : nom, V_rest, τ, biais, réfractaire, η, état runtime (V, sortie)
  - *Synapse* : poids, type (axo-dendritique, axo-axonique, dendro-dendritique, dendro-axonique)
- **Randomisation des poids** (barre d'outils ou menu Édition).
- Chargement et sauvegarde au format CSV (sections `[NEURONES]` et `[SYNAPSES]`).
- Exemples inclus dans `exemples/reseaux/`.

### Simulation
- **Bouton Simuler** (F5) : exécute l'apprentissage par lots configurables (Batch, défaut 10 itérations par rafraîchissement, timer 20 ms).
- **Sélecteur Max itérations** : de 1 à 1 000 000 dans la barre d'outils (défaut 100).
- **Algorithme** :
  - Propagation avant avec sigmoïde (activité spontanée `σ_min = 0.005`)
  - Delta rule `Δw = η × (cible - sortie) × pre` pour la sortie
  - Rétropropagation linéaire `δ_hidden = Σ(δ_output × w)` sans dérivée de sigmoïde
  - Biais : `Δb = η × (cible - sortie)` (sortie), `Δb = Σ(δ_output × w)` (caché)
  - **Batch gradient** : les deltas sont accumulés sur tout le dataset, moyennés, puis appliqués une fois par epoch
  - Clamps : poids `[-5, +5]`, biais `[-2, +2]`
- **Courbe d'erreur** : affichée en direct avec auto-échelle.
- **Seuil erreur** : si l'erreur moyenne sur le dataset descend sous ce seuil, la simulation s'arrête automatiquement (défaut 0.001).
- **Batch** : nombre d'itérations entre deux rafraîchissements de l'interface (défaut 10).
- **Reset état** : remet à zéro potentiels et sorties.

## Architecture

| Couche | Fichiers | Rôle |
|--------|----------|------|
| Entrée | `main.cpp` → `MainWindow.{hpp,cpp}` | QApplication + fenêtre principale |
| Scène | `GraphScene.{hpp,cpp}`, `NeuroneNode.{hpp,cpp}`, `SynapseEdge.{hpp,cpp}` | Éditeur visuel sur QGraphicsScene |
| Modèle | `ReseauNeural.{hpp,cpp}` | Simulation en `float` — delta rule + rétropropagation linéaire |
| Données | `Dataset.{hpp,cpp}` | Chargement/sauvegarde CSV |
| Neurone | `NeuroneBiologique.hpp` | Header-only, copie de `github.com/Fo170/NeuroneBiologique` |

## Structure du projet

```
AppNeuroneBiologique/
├── Readme.md
├── AGENTS.md
├── qtgui/
│   ├── CMakeLists.txt
│   ├── main.cpp
│   ├── MainWindow.{hpp,cpp}
│   ├── GraphScene.{hpp,cpp}
│   ├── NeuroneNode.{hpp,cpp}
│   ├── SynapseEdge.{hpp,cpp}
│   ├── PropertyPanel.{hpp,cpp}
│   ├── ReseauNeural.{hpp,cpp}
│   ├── Dataset.{hpp,cpp}
│   ├── NeuroneBiologique.hpp
│   ├── build/
│   └── exemples/
│       ├── datasets/
│       └── reseaux/
```

## Utilisation rapide

1. Lancer l'application : `./qtgui/build/NeuroneBiologiqueGUI`
2. Charger un réseau : menu Fichier → Charger un réseau (ex: `exemples/reseaux/xor.csv`)
3. Charger un dataset : menu Fichier → Charger un dataset (ex: `exemples/datasets/xor.csv`)
4. Définir le nombre d'époques dans la barre d'outils
5. Cliquer **Simuler** (ou F5) pour lancer l'apprentissage
6. Observer la courbe d'erreur et les sorties dans le tableau dataset

## Licence

**GPL-3.0** — FOURNET Olivier — olivier.fournet@free.fr
