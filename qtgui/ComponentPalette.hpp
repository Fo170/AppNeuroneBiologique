#ifndef COMPONENT_PALETTE_HPP
#define COMPONENT_PALETTE_HPP

#include <QWidget>
#include <QTreeWidget>
#include <QVariantMap>

class ComponentPalette : public QWidget {
    Q_OBJECT
public:
    explicit ComponentPalette(QWidget* parent = nullptr);

    void scanner_modules(const QString& repertoire);

signals:
    void component_selected(const QVariantMap& data);

private:
    QTreeWidget* tree_;
    QTreeWidgetItem* groupe_bases_;
    QTreeWidgetItem* groupe_modules_;

    void ajouter_bases();
    void ajouter_item(QTreeWidgetItem* parent, const QString& nom,
                      const QVariantMap& data);
};

#endif
