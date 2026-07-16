#include "ReseauNeural.hpp"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <map>
#include <cmath>
#include <algorithm>
#include <random>
#include <chrono>

NeuroneInfo* ReseauNeural::trouver_neurone(int id) {
    for (auto& n : neurones)
        if (n.id == id) return &n;
    return nullptr;
}

const NeuroneInfo* ReseauNeural::trouver_neurone(int id) const {
    for (const auto& n : neurones)
        if (n.id == id) return &n;
    return nullptr;
}

int ReseauNeural::ajouter_neurone(const QString& nom, float x, float y, bool entree) {
    neurones.push_back({prochain_id_neurone, nom, 0.0f, 10.0f, 0.0f, 2.0f, 0.01f, 0.001f,
                        entree, x, y});
    return prochain_id_neurone++;
}

bool ReseauNeural::supprimer_neurone(int id) {
    auto it = std::remove_if(neurones.begin(), neurones.end(),
                             [id](const NeuroneInfo& n) { return n.id == id; });
    if (it == neurones.end()) return false;
    neurones.erase(it, neurones.end());
    synapses.erase(std::remove_if(synapses.begin(), synapses.end(),
                   [id](const SynapseInfo& s) {
                       return s.source_id == id || s.target_id == id;
                   }), synapses.end());
    return true;
}

int ReseauNeural::ajouter_synapse(int source_id, int target_id, float poids,
                                   TypeSynapse type) {
    if (!trouver_neurone(source_id) || !trouver_neurone(target_id)) return -1;
    if (source_id == target_id) return -1;
    synapses.push_back({prochain_id_synapse, source_id, target_id, poids, type});
    return prochain_id_synapse++;
}

bool ReseauNeural::supprimer_synapse(int id) {
    auto it = std::remove_if(synapses.begin(), synapses.end(),
                             [id](const SynapseInfo& s) { return s.id == id; });
    if (it == synapses.end()) return false;
    synapses.erase(it, synapses.end());
    return true;
}

std::vector<int> ReseauNeural::ids_entree() const {
    std::vector<int> ids;
    for (const auto& n : neurones)
        if (n.est_entree) ids.push_back(n.id);
    return ids;
}

std::vector<int> ReseauNeural::ids_sortie() const {
    if (neurones.empty()) return {};
    std::vector<int> ids;
    for (const auto& n : neurones) {
        if (n.est_entree) continue;
        bool est_source = false;
        for (const auto& s : synapses)
            if (s.source_id == n.id) { est_source = true; break; }
        if (!est_source) ids.push_back(n.id);
    }
    if (ids.empty() && !neurones.empty())
        ids.push_back(neurones.back().id);
    return ids;
}

void ReseauNeural::reinitialiser_etat() {
    for (auto& n : neurones) {
        n.V = 0.0f;
        n.timer_refractaire = 0.0f;
        n.sortie = 0.0f;
    }
}

float ReseauNeural::sigmoide(float v) {
    const float min_act = 0.005f;
    float s = 1.0f / (1.0f + std::exp(-10.0f * (v - 0.5f)));
    return min_act + (1.0f - 2.0f * min_act) * s;
}

void ReseauNeural::simuler_pas(const std::vector<float>& entrees,
                                const std::vector<float>& cibles,
                                float dt_ms, bool mode_apprentissage) {
    auto ids_ent = ids_entree();
    for (size_t i = 0; i < ids_ent.size() && i < entrees.size(); ++i) {
        auto* n = trouver_neurone(ids_ent[i]);
        if (n) n->sortie = entrees[i];
    }

    for (auto& n : neurones) {
        if (n.est_entree) continue;
        if (n.timer_refractaire > 0.0f) {
            n.timer_refractaire -= dt_ms;
            if (n.timer_refractaire < 0.0f) n.timer_refractaire = 0.0f;
            continue;
        }
        float I_syn = 0.0f;
        for (const auto& s : synapses) {
            if (s.target_id == n.id) {
                auto* src = trouver_neurone(s.source_id);
                if (src) I_syn += s.poids * src->sortie;
            }
        }
        I_syn += n.biais;
        n.V += ((-(n.V - n.V_rest) / n.tau + I_syn) * dt_ms);
        n.sortie = sigmoide(n.V);
        if (n.sortie > 0.9f)
            n.timer_refractaire = n.refractaire_ms;
    }

    if (mode_apprentissage) {
        auto ids_s = ids_sortie();
        std::map<int, float> deltas;
        for (size_t k = 0; k < ids_s.size(); ++k) {
            auto* n = trouver_neurone(ids_s[k]);
            if (!n) continue;
            float cible = (k < cibles.size()) ? cibles[k] : 0.0f;
            deltas[n->id] = n->eta * (cible - n->sortie);
        }
        for (auto& n : neurones) {
            if (n.est_entree) continue;
            if (deltas.count(n.id)) continue;
            float downstream = 0.0f;
            for (const auto& s : synapses)
                if (s.source_id == n.id) {
                    auto it = deltas.find(s.target_id);
                    if (it != deltas.end())
                        downstream += it->second * s.poids;
                }
            deltas[n.id] = downstream;
        }
        for (auto& n : neurones) {
            if (n.est_entree) continue;
            auto it = deltas.find(n.id);
            if (it == deltas.end()) continue;
            float delta = it->second;
            if (accum_poids_.size() == synapses.size()) {
                for (size_t si = 0; si < synapses.size(); ++si) {
                    if (synapses[si].target_id == n.id) {
                        auto* src = trouver_neurone(synapses[si].source_id);
                        if (src)
                            accum_poids_[si] += delta * src->sortie;
                    }
                }
                accum_biais_[&n - &neurones.front()] += delta;
            } else {
                for (auto& s : synapses) {
                    if (s.target_id == n.id) {
                        auto* src = trouver_neurone(s.source_id);
                        if (src) {
                            s.poids += delta * src->sortie;
                            s.poids = std::clamp(s.poids, -5.0f, 5.0f);
                        }
                    }
                }
                n.biais += delta;
                n.biais = std::clamp(n.biais, -2.0f, 2.0f);
            }
        }
    }
}

void ReseauNeural::randomiser_poids(float min, float max) {
    static std::mt19937 gen(
        static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count())
    );
    std::uniform_real_distribution<float> dist(min, max);
    std::uniform_real_distribution<float> bias_dist(-0.5f, 0.5f);
    for (auto& s : synapses)
        s.poids = dist(gen);
    for (auto& n : neurones)
        if (!n.est_entree)
            n.biais = bias_dist(gen);
}

float ReseauNeural::simuler_un_epoch(const Dataset& dataset, float dt_ms, bool apprentissage) {
    if (apprentissage) {
        accum_poids_.assign(synapses.size(), 0.0f);
        accum_biais_.assign(neurones.size(), 0.0f);
    }

    float erreur_total = 0.0f;
    int denom = 0;
    for (const auto& ex : dataset.exemples) {
        reinitialiser_etat();
        simuler_pas(ex.entrees, ex.cibles, dt_ms, apprentissage);
        auto ids_s = ids_sortie();
        for (size_t k = 0; k < ids_s.size(); ++k) {
            auto* n = trouver_neurone(ids_s[k]);
            if (!n) continue;
            float cible = (k < ex.cibles.size()) ? ex.cibles[k] : 0.0f;
            erreur_total += std::abs(n->sortie - cible);
            ++denom;
        }
    }

    if (apprentissage && !dataset.exemples.empty()) {
        int N = dataset.exemples.size();
        for (size_t i = 0; i < synapses.size(); ++i) {
            synapses[i].poids += accum_poids_[i] / N;
            synapses[i].poids = std::clamp(synapses[i].poids, -5.0f, 5.0f);
        }
        for (size_t j = 0; j < neurones.size(); ++j) {
            if (neurones[j].est_entree) continue;
            neurones[j].biais += accum_biais_[j] / N;
            neurones[j].biais = std::clamp(neurones[j].biais, -2.0f, 2.0f);
        }
    }

    return (denom > 0) ? erreur_total / denom : 0.0f;
}

void ReseauNeural::simuler_dataset(const Dataset& dataset, float dt_ms,
                                    bool apprentissage, int epochs) {
    for (int e = 0; e < epochs; ++e) {
        for (const auto& ex : dataset.exemples) {
            reinitialiser_etat();
            simuler_pas(ex.entrees, ex.cibles, dt_ms, apprentissage);
        }
    }
}

std::vector<std::vector<float>> ReseauNeural::calculer_sorties(const Dataset& dataset, float dt_ms) {
    std::vector<std::vector<float>> resultats;
    for (const auto& ex : dataset.exemples) {
        reinitialiser_etat();
        simuler_pas(ex.entrees, ex.cibles, dt_ms, false);
        auto ids_s = ids_sortie();
        std::vector<float> sorties_ex;
        for (int id : ids_s) {
            auto* n = trouver_neurone(id);
            sorties_ex.push_back(n ? n->sortie : 0.0f);
        }
        resultats.push_back(sorties_ex);
    }
    return resultats;
}

void ReseauNeural::sauvegarder_csv(const QString& chemin) const {
    QFile f(chemin);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&f);
    out << "[NEURONES]\n";
    out << "id,nom,V_rest,tau,biais,refractaire_ms,eta,oubli,est_entree,x,y\n";
    for (const auto& n : neurones) {
        out << n.id << "," << n.nom << "," << n.V_rest << "," << n.tau << ","
            << n.biais << "," << n.refractaire_ms << "," << n.eta << ","
            << n.oubli_lent << "," << (n.est_entree ? 1 : 0) << ","
            << n.pos_x << "," << n.pos_y << "\n";
    }
    out << "[SYNAPSES]\n";
    out << "id,source,target,poids,type\n";
    for (const auto& s : synapses) {
        QString tnom;
        switch (s.type) {
            case TypeSynapse::AXO_DENDRITIQUE: tnom = "AXO_DENDRITIQUE"; break;
            case TypeSynapse::AXO_AXONIQUE: tnom = "AXO_AXONIQUE"; break;
            case TypeSynapse::DENDRO_DENDRITIQUE: tnom = "DENDRO_DENDRITIQUE"; break;
            case TypeSynapse::DENDRO_AXONIQUE: tnom = "DENDRO_AXONIQUE"; break;
        }
        out << s.id << "," << s.source_id << "," << s.target_id << ","
            << s.poids << "," << tnom << "\n";
    }
}

static TypeSynapse type_from_string(const QString& s) {
    if (s == "AXO_DENDRITIQUE") return TypeSynapse::AXO_DENDRITIQUE;
    if (s == "AXO_AXONIQUE") return TypeSynapse::AXO_AXONIQUE;
    if (s == "DENDRO_DENDRITIQUE") return TypeSynapse::DENDRO_DENDRITIQUE;
    return TypeSynapse::DENDRO_AXONIQUE;
}

void ReseauNeural::charger_csv(const QString& chemin) {
    QFile f(chemin);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream in(&f);
    neurones.clear();
    synapses.clear();
    prochain_id_neurone = 0;
    prochain_id_synapse = 0;
    enum Section { AUCUNE, NEURONES, SYNAPSES };
    Section section = AUCUNE;
    bool first = true;
    while (!in.atEnd()) {
        QString ligne = in.readLine().trimmed();
        if (ligne.isEmpty()) continue;
        if (ligne == "[NEURONES]") { section = NEURONES; first = true; continue; }
        if (ligne == "[SYNAPSES]") { section = SYNAPSES; first = true; continue; }
        if (ligne.startsWith('[')) continue;
        if (first) { first = false; continue; }
        QStringList champs = ligne.split(',');
        if (section == NEURONES && champs.size() >= 11) {
            NeuroneInfo n;
            n.id = champs[0].toInt();
            n.nom = champs[1];
            n.V_rest = champs[2].toFloat();
            n.tau = champs[3].toFloat();
            n.biais = champs[4].toFloat();
            n.refractaire_ms = champs[5].toFloat();
            n.eta = champs[6].toFloat();
            n.oubli_lent = champs[7].toFloat();
            n.est_entree = champs[8].toInt() != 0;
            n.pos_x = champs[9].toFloat();
            n.pos_y = champs[10].toFloat();
            neurones.push_back(n);
            if (n.id >= prochain_id_neurone) prochain_id_neurone = n.id + 1;
        } else if (section == SYNAPSES && champs.size() >= 5) {
            SynapseInfo s;
            s.id = champs[0].toInt();
            s.source_id = champs[1].toInt();
            s.target_id = champs[2].toInt();
            s.poids = champs[3].toFloat();
            s.type = type_from_string(champs[4]);
            synapses.push_back(s);
            if (s.id >= prochain_id_synapse) prochain_id_synapse = s.id + 1;
        }
    }
}

QString ReseauNeural::resume() const {
    return QString("%1 neurones, %2 synapses")
            .arg(neurones.size()).arg(synapses.size());
}
