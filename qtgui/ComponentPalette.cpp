#include "ComponentPalette.hpp"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QLabel>

ComponentPalette::ComponentPalette(QWidget* parent) : QWidget(parent) {
    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(2, 2, 2, 2);

    auto* titre = new QLabel("<b>Composants</b>");
    titre->setAlignment(Qt::AlignCenter);
    lay->addWidget(titre);

    tree_ = new QTreeWidget;
    tree_->setHeaderHidden(true);
    tree_->setDragEnabled(false);
    tree_->setSelectionMode(QAbstractItemView::SingleSelection);
    tree_->setAnimated(true);
    tree_->setIndentation(12);
    lay->addWidget(tree_);

    groupe_bases_ = new QTreeWidgetItem(tree_, {"Neurones de base"});
    groupe_bases_->setExpanded(true);
    groupe_modules_ = new QTreeWidgetItem(tree_, {"Modules"});
    groupe_modules_->setExpanded(true);

    ajouter_bases();

    connect(tree_, &QTreeWidget::itemClicked, this, [this](QTreeWidgetItem* item, int) {
        if (!item->parent()) return;
        QVariantMap data = item->data(0, Qt::UserRole).toMap();
        if (!data.isEmpty())
            emit component_selected(data);
    });
}

void ComponentPalette::ajouter_bases() {
    QVariantMap entree;
    entree["type"] = "neurone_entree";
    entree["nom"] = "Neurone d'entrée";
    entree["eta"] = 0.01;
    ajouter_item(groupe_bases_, "Neurone d'entrée", entree);

    QVariantMap cache;
    cache["type"] = "neurone_cache";
    cache["nom"] = "Neurone caché";
    cache["eta"] = 0.1;
    ajouter_item(groupe_bases_, "Neurone caché", cache);

    QVariantMap sortie;
    sortie["type"] = "neurone_sortie";
    sortie["nom"] = "Neurone de sortie";
    sortie["eta"] = 0.1;
    ajouter_item(groupe_bases_, "Neurone de sortie", sortie);
}

void ComponentPalette::ajouter_item(QTreeWidgetItem* parent,
                                     const QString& nom,
                                     const QVariantMap& data) {
    auto* item = new QTreeWidgetItem(parent, {nom});
    item->setData(0, Qt::UserRole, data);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

void ComponentPalette::scanner_modules(const QString& repertoire) {
    while (groupe_modules_->childCount() > 0)
        delete groupe_modules_->takeChild(0);

    QDir d(repertoire);
    if (!d.exists()) return;

    QStringList filtres = {"*.neuron", "*.json"};
    for (const auto& fi : d.entryInfoList(filtres, QDir::Files)) {
        QFile f(fi.absoluteFilePath());
        if (!f.open(QIODevice::ReadOnly)) continue;
        QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
        f.close();
        if (!doc.isObject()) continue;
        QJsonObject obj = doc.object();
        QString nom = obj.value("nom").toString(fi.baseName());

        QVariantMap data;
        data["type"] = "module";
        data["nom"] = nom;
        data["fichier"] = fi.absoluteFilePath();
        ajouter_item(groupe_modules_, nom, data);
    }
}
