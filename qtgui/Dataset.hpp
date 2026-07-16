#ifndef DATASET_HPP
#define DATASET_HPP

#include <vector>
#include <QString>
#include <QStringList>

struct Exemple {
    std::vector<float> entrees;
    std::vector<float> cibles;

    float cible() const { return cibles.empty() ? 0.0f : cibles[0]; }
};

class Dataset {
public:
    std::vector<Exemple> exemples;
    QStringList noms_colonnes;
    QString chemin_fichier;
    int nb_entrees = 0;

    void vider();
    void sauvegarder_csv(const QString& chemin);
    bool charger_csv(const QString& chemin);
    int nb_entrees_reel() const;
    int nb_sorties() const;
    int nb_exemples() const;
    QString resume() const;
};

#endif
