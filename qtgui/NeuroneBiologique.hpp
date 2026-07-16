#ifndef NEURONE_BIOLOGIQUE_HPP
#define NEURONE_BIOLOGIQUE_HPP

#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>
#include <chrono>

// ============================================================
//  TYPES DE SYNAPSE (biologiquement réalistes)
// ============================================================
enum class TypeSynapse {
    AXO_DENDRITIQUE,   // 66.6% — classique, entrée → dendrite
    AXO_AXONIQUE,      // 25.8% — modulation présynaptique
    DENDRO_DENDRITIQUE,// 5.8%  — interaction latérale
    DENDRO_AXONIQUE    // 1.8%  — rétroaction dendrite → axone
};

// ============================================================
//  SYNAPSE
// ============================================================
struct Synapse {
    double poids;               // w
    double poids_initial;       // pour traçabilité
    TypeSynapse type;
    double dernier_pre = 0.0;   // trace du signal présynaptique
    double dernier_post = 0.0;  // trace du signal postsynaptique

    Synapse(double w, TypeSynapse t) : poids(w), poids_initial(w), type(t) {}
};

// ============================================================
//  NEURONE BIOLOGIQUE (modèle à fuite + sigmoïde)
// ============================================================
class NeuroneBiologique {
private:
    // --- Paramètres physiques ---
    double V;              // Potentiel de membrane (V)
    double V_rest;         // Potentiel de repos
    double tau;            // Constante de temps de fuite (ms)
    double biais;          // Biais (seuil d'activation)

    // --- Période réfractaire ---
    double periode_refractaire_ms;  // 2 ms après activation forte
    double timer_refractaire;       // compteur temps restant

    // --- Synapses ---
    std::vector<Synapse> synapses;

    // --- Apprentissage DAN-Modulé ---
    double taux_apprentissage;      // η (eta)
    double oubli_lent;              // DAN = -0.001 quand pas de signal

    // --- Générateur aléatoire pour les types de synapses ---
    static std::mt19937& rng() {
        static thread_local std::mt19937 gen(
            static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count())
        );
        return gen;
    }

    // Distribution pour les 4 types de synapses (proportions biologiques)
    static TypeSynapse tirer_type_synapse() {
        static std::uniform_real_distribution<double> dist(0.0, 1.0);
        double r = dist(rng());
        if (r < 0.666) return TypeSynapse::AXO_DENDRITIQUE;
        if (r < 0.666 + 0.258) return TypeSynapse::AXO_AXONIQUE;
        if (r < 0.666 + 0.258 + 0.058) return TypeSynapse::DENDRO_DENDRITIQUE;
        return TypeSynapse::DENDRO_AXONIQUE;
    }

public:
    // ============================================================
    //  CONSTRUCTEUR
    // ============================================================
    NeuroneBiologique(
        int nombre_entrees,
        double v_rest = 0.0,
        double tau_ms = 10.0,
        double biais_val = 0.0,
        double refractaire_ms = 2.0,
        double eta = 0.01,
        double oubli = 0.001
    ) : V(v_rest), V_rest(v_rest), tau(tau_ms), biais(biais_val),
        periode_refractaire_ms(refractaire_ms), timer_refractaire(0.0),
        taux_apprentissage(eta), oubli_lent(oubli)
    {
        // Initialisation aléatoire des poids (loi normale centrée)
        std::normal_distribution<double> dist(0.0, 0.5);
        for (int i = 0; i < nombre_entrees; ++i) {
            synapses.emplace_back(dist(rng()), tirer_type_synapse());
        }
    }

    // ============================================================
    //  ÉQUATION DE FUITE TEMPORNELLE (dV/dt)
    // ============================================================
    // dV/dt = -(V - V_rest)/τ + I_syn
    // Intégration par Euler explicite avec pas de temps dt_ms
    // ============================================================
    void integrer_fuite(double dt_ms) {
        if (timer_refractaire > 0.0) {
            timer_refractaire -= dt_ms;
            if (timer_refractaire < 0.0) timer_refractaire = 0.0;
            return; // pendant la réfractaire, pas de changement de V
        }

        double I_syn = courant_synapse();
        double dV = (-(V - V_rest) / tau + I_syn) * dt_ms;
        V += dV;
    }

    // ============================================================
    //  COURANT SYNAPTIQUE I_syn = Σ(wᵢ × eᵢ)
    // ============================================================
    double courant_synapse() const {
        double I = 0.0;
        for (const auto& syn : synapses) {
            I += syn.poids * syn.dernier_pre;
        }
        return I + biais;
    }

    // ============================================================
    //  FONCTION D'ACTIVATION SIGMOÏDE
    // ============================================================
    // σ(V) = 1 / (1 + e^(-10(V - 0.5)))
    // sortie ∈ [0,1] → taux de décharge
    // ============================================================
    double sigmoide(double v) const {
        return 1.0 / (1.0 + std::exp(-10.0 * (v - 0.5)));
    }

    // ============================================================
    //  ACTIVATION COMPLÈTE (une étape de simulation)
    // ============================================================
    double activer(const std::vector<double>& entrees, double dt_ms = 1.0) {
        // 1. Mettre à jour les traces présynaptiques
        for (size_t i = 0; i < synapses.size() && i < entrees.size(); ++i) {
            synapses[i].dernier_pre = entrees[i];
        }

        // 2. Intégrer l'équation de fuite
        integrer_fuite(dt_ms);

        // 3. Calculer la sortie sigmoïde
        double sortie = sigmoide(V);

        // 4. Mettre à jour la trace postsynaptique
        for (auto& syn : synapses) {
            syn.dernier_post = sortie;
        }

        // 5. Déclencher période réfractaire si activation forte
        if (sortie > 0.9) {
            timer_refractaire = periode_refractaire_ms;
        }

        return sortie;
    }

    // ============================================================
    //  APPRENTISSAGE DAN-MODULÉ (Règle de trois facteurs)
    // ============================================================
    // Δw = η × DAN(t) × (pré × post)
    //
    // DAN = +1.0  → Potentiation (récompense)
    // DAN = -0.5  → Dépression (punition)
    // DAN = -0.001→ Oubli lent (pas de signal)
    // ============================================================
    void apprendre(double signal_DAN) {
        for (auto& syn : synapses) {
            double produit_hebbien = syn.dernier_pre * syn.dernier_post;
            double delta_w = taux_apprentissage * signal_DAN * produit_hebbien;
            syn.poids += delta_w;
        }
    }

    // --- Helpers d'apprentissage ---
    void recompense()  { apprendre(+1.0); }   // Sucre
    void punition()    { apprendre(-0.5); }   // Choc
    void oubli()       { apprendre(-0.001); } // Pas de signal

    // ============================================================
    //  ACCESSEURS
    // ============================================================
    double get_potentiel() const { return V; }
    double get_sortie() const { return sigmoide(V); }
    double get_tau() const { return tau; }
    double get_biais() const { return biais; }
    bool est_refractaire() const { return timer_refractaire > 0.0; }

    void set_potentiel(double v) { V = v; }
    void set_biais(double b) { biais = b; }

    const std::vector<Synapse>& get_synapses() const { return synapses; }
    std::vector<Synapse>& get_synapses() { return synapses; }

    // ============================================================
    //  STATISTIQUES
    // ============================================================
    void afficher_stats() const {
        int n = synapses.size();
        int axo_dend = 0, axo_axo = 0, dend_dend = 0, dend_axo = 0;
        double poids_moy = 0.0;

        for (const auto& syn : synapses) {
            poids_moy += syn.poids;
            switch (syn.type) {
                case TypeSynapse::AXO_DENDRITIQUE:    ++axo_dend; break;
                case TypeSynapse::AXO_AXONIQUE:       ++axo_axo;  break;
                case TypeSynapse::DENDRO_DENDRITIQUE: ++dend_dend;break;
                case TypeSynapse::DENDRO_AXONIQUE:    ++dend_axo; break;
            }
        }
        poids_moy /= n;

        std::printf("\n=== Statistiques du neurone ===\n");
        std::printf("Potentiel V = %.4f | Sortie σ(V) = %.4f\n", V, get_sortie());
        std::printf("Réfractaire : %s (%.2f ms restant)\n", 
                    est_refractaire() ? "OUI" : "NON", timer_refractaire);
        std::printf("Synapses totales : %d | Poids moyen : %.4f\n", n, poids_moy);
        std::printf("  Axo-dendritique  : %d (%.1f%%)\n", axo_dend, 100.0*axo_dend/n);
        std::printf("  Axo-axonique     : %d (%.1f%%)\n", axo_axo, 100.0*axo_axo/n);
        std::printf("  Dendro-dendritiq : %d (%.1f%%)\n", dend_dend, 100.0*dend_dend/n);
        std::printf("  Dendro-axonique  : %d (%.1f%%)\n", dend_axo, 100.0*dend_axo/n);
        std::printf("===============================\n\n");
    }
};

// ============================================================
//  EXEMPLE D'UTILISATION (main)
// ============================================================
/*
#include <iostream>

int main() {
    // Créer un neurone avec 5 entrées
    NeuroneBiologique neurone(5);

    // Afficher les stats initiales
    neurone.afficher_stats();

    // Simulation sur 20 pas de temps (dt = 1 ms)
    for (int t = 0; t < 20; ++t) {
        std::vector<double> entrees = {0.2, 0.5, 0.1, 0.8, 0.3};
        double sortie = neurone.activer(entrees, 1.0);

        std::printf("t=%3d ms | V=%.4f | sortie=%.4f | refract=%s\n",
                    t, neurone.get_potentiel(), sortie,
                    neurone.est_refractaire() ? "OUI" : "non");

        // Apprentissage DAN-modulé
        if (t == 5)  neurone.recompense();   // DAN = +1.0
        if (t == 10) neurone.punition();     // DAN = -0.5
        if (t == 15) neurone.oubli();        // DAN = -0.001
    }

    neurone.afficher_stats();
    return 0;
}
*/

#endif // NEURONE_BIOLOGIQUE_HPP
