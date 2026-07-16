#include "Dataset.hpp"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>

void Dataset::vider() {
    exemples.clear();
    noms_colonnes.clear();
    chemin_fichier.clear();
    nb_entrees = 0;
}

void Dataset::sauvegarder_csv(const QString& chemin) {
    QFile f(chemin);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&f);

    // Save nb_entrees if explicitly set
    if (nb_entrees > 0)
        out << "# nb_entrees=" << nb_entrees << "\n";

    if (noms_colonnes.isEmpty()) {
        noms_colonnes.clear();
        int ni = nb_entrees > 0 ? nb_entrees : (exemples.empty() ? 1 : (int)exemples[0].entrees.size());
        int ns = exemples.empty() ? 1 : (int)exemples[0].cibles.size();
        for (int i = 0; i < ni; ++i)
            noms_colonnes << QString("e%1").arg(i + 1);
        for (int i = 0; i < ns; ++i)
            noms_colonnes << (ns == 1 ? QString("cible") : QString("cible%1").arg(i + 1));
    }
    out << noms_colonnes.join(',') << "\n";

    for (const auto& ex : exemples) {
        bool first = true;
        auto write_val = [&](float v) {
            if (!first) out << ",";
            out << v;
            first = false;
        };
        for (float v : ex.entrees) write_val(v);
        for (float v : ex.cibles) write_val(v);
        out << "\n";
    }
    chemin_fichier = chemin;
}

bool Dataset::charger_csv(const QString& chemin) {
    QFile f(chemin);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "charger_csv: impossible d'ouvrir" << chemin;
        return false;
    }
    QTextStream in(&f);
    exemples.clear();
    noms_colonnes.clear();

    // Check for nb_entrees header
    nb_entrees = 0;
    QString first_line = in.readLine().trimmed();
    if (first_line.startsWith("# nb_entrees=")) {
        nb_entrees = first_line.mid(QString("# nb_entrees=").length()).trimmed().toInt();
        first_line = in.readLine().trimmed();
    }

    noms_colonnes = first_line.split(',');
    int total_cols = noms_colonnes.size();
    if (total_cols < 2) {
        qWarning() << "charger_csv: fichier invalide (moins de 2 colonnes):" << chemin;
        return false;
    }

    int ni = (nb_entrees > 0) ? nb_entrees : (total_cols - 1);

    while (!in.atEnd()) {
        QString ligne = in.readLine().trimmed();
        if (ligne.isEmpty()) continue;
        QStringList champs = ligne.split(',');
        if (champs.size() < ni + 1) continue;

        Exemple ex;
        for (int i = 0; i < ni; ++i)
            ex.entrees.push_back(champs[i].toFloat());
        for (int i = ni; i < champs.size(); ++i)
            ex.cibles.push_back(champs[i].toFloat());
        exemples.push_back(ex);
    }

    if (exemples.empty()) {
        qWarning() << "charger_csv: aucun exemple trouvé dans" << chemin;
        return false;
    }

    if (nb_entrees == 0)
        nb_entrees = ni;

    chemin_fichier = chemin;
    qDebug() << "charger_csv: OK" << exemples.size() << "exemples,"
             << nb_entrees << "entrees," << (int)exemples[0].cibles.size() << "sorties -" << chemin;
    return true;
}

int Dataset::nb_entrees_reel() const {
    if (nb_entrees > 0) return nb_entrees;
    if (exemples.empty()) return 1;
    return (int)exemples[0].entrees.size();
}

int Dataset::nb_sorties() const {
    if (exemples.empty()) return 1;
    return (int)exemples[0].cibles.size();
}

int Dataset::nb_exemples() const {
    return exemples.size();
}

QString Dataset::resume() const {
    QString s;
    if (!chemin_fichier.isEmpty())
        s += QString("Fichier: %1\n").arg(QFileInfo(chemin_fichier).fileName());
    else
        s += "Fichier: (non sauvegardé)\n";
    s += QString("Exemples: %1  |  Entrées: %2  |  Sorties: %3")
            .arg(nb_exemples()).arg(nb_entrees_reel()).arg(nb_sorties());
    return s;
}
