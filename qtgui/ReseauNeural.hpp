#ifndef RESEAU_NEURAL_HPP
#define RESEAU_NEURAL_HPP

#include <vector>
#include <QString>
#include <QPointF>
#include "NeuroneBiologique.hpp"
#include "Dataset.hpp"

struct NeuroneInfo {
    int id;
    QString nom;
    float V_rest = 0.0f;
    float tau = 10.0f;
    float biais = 0.0f;
    float refractaire_ms = 2.0f;
    float eta = 0.01f;
    float oubli_lent = 0.0001f;
    bool est_entree = false;
    float pos_x = 0.0f;
    float pos_y = 0.0f;
    float V = 0.0f;
    float timer_refractaire = 0.0f;
    float sortie = 0.0f;
    int colonne_entree = -1; // -1 = position par défaut
    int colonne_sortie = -1; // -1 = position par défaut
};

struct SynapseInfo {
    int id;
    int source_id;
    int target_id;
    float poids = 0.5f;
    TypeSynapse type = TypeSynapse::AXO_DENDRITIQUE;
};

class ReseauNeural {
public:
    std::vector<NeuroneInfo> neurones;
    std::vector<SynapseInfo> synapses;
    int prochain_id_neurone = 0;
    int prochain_id_synapse = 0;

    NeuroneInfo* trouver_neurone(int id);
    const NeuroneInfo* trouver_neurone(int id) const;
    SynapseInfo* trouver_synapse(int id);
    const SynapseInfo* trouver_synapse(int id) const;

    int ajouter_neurone(const QString& nom, float x, float y, bool entree = false);
    bool supprimer_neurone(int id);

    int ajouter_synapse(int source_id, int target_id, float poids,
                        TypeSynapse type = TypeSynapse::AXO_DENDRITIQUE);
    bool supprimer_synapse(int id);

    std::vector<int> ids_entree() const;
    std::vector<int> ids_sortie() const;

    void reinitialiser_etat();
    void simuler_pas(const std::vector<float>& entrees,
                     const std::vector<float>& cibles,
                     float dt_ms, bool mode_apprentissage);
    float simuler_un_epoch(const Dataset& dataset, float dt_ms, bool apprentissage);
    std::vector<std::vector<float>> calculer_sorties(const Dataset& dataset, float dt_ms);
    void simuler_dataset(const Dataset& dataset, float dt_ms,
                         bool apprentissage, int epochs = 1);

    static float sigmoide(float v);

    void randomiser_poids(float min = -1.0f, float max = 1.0f);
    void sauvegarder_csv(const QString& chemin);
    void charger_csv(const QString& chemin);
    QString resume() const;

    bool exporter_module(const QString& chemin,
                         const std::vector<int>& ids_neurones,
                         const std::vector<int>& ids_synapses) const;
    bool importer_module(const QString& chemin, QPointF decalage);

    QString chemin_fichier;

    std::vector<float> accum_poids_;
    std::vector<float> accum_biais_;
};

#endif
