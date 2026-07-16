#ifndef PROPERTY_PANEL_HPP
#define PROPERTY_PANEL_HPP

#include <QWidget>
#include <QTabWidget>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QGroupBox>
#include <QVector>
#include <QPainter>
#include <QSplitter>
#include "ReseauNeural.hpp"
#include "Dataset.hpp"

class GraphiqueErreur : public QWidget {
    Q_OBJECT
public:
    explicit GraphiqueErreur(QWidget* parent = nullptr);
    void ajouter_point(double erreur);
    void reinitialiser();
    QSize minimumSizeHint() const override { return QSize(200, 80); }
    QSize sizeHint() const override { return QSize(300, 120); }
protected:
    void paintEvent(QPaintEvent*) override;
private:
    QVector<double> points_;
    double max_erreur_ = 1.0;
};

class PropertyPanel : public QWidget {
    Q_OBJECT
public:
    explicit PropertyPanel(QWidget* parent = nullptr);

    void afficher_neurone(NeuroneInfo* info);
    void afficher_synapse(SynapseInfo* info);
    void afficher_dataset(Dataset* dataset);
    void activer_onglet_apprentissage();
    void mettre_a_jour_sorties_dataset(const std::vector<std::vector<float>>& sorties);
    void nettoyer_sorties_dataset();

    double seuil_arret() const;
    int batch() const;

public slots:
    void ajouter_point_erreur(int epoch, double erreur);
    void set_meilleure_erreur(double v);
    void reinitialiser_apprentissage();

signals:
    void neurone_modifie();
    void synapse_modifie();
    void dataset_modifie();
    void stop_simulation();

private:
    QWidget* onglet_dataset();
    QWidget* onglet_neurone();
    QWidget* onglet_synapse();
    QWidget* onglet_apprentissage();

    QTabWidget* tabs_;
    NeuroneInfo* neurone_courant_ = nullptr;
    SynapseInfo* synapse_courant_ = nullptr;
    Dataset* dataset_courant_ = nullptr;

    // Dataset tab (merged with apprentissage)
    QLabel* label_info_dataset_;
    QTableWidget* table_dataset_;
    QPushButton* btn_add_row_;
    QPushButton* btn_del_row_;
    QPushButton* btn_save_csv_;
    QPushButton* btn_load_csv_;
    QSplitter* split_dataset_;

    // Apprentissage widgets (inside dataset tab)
    GraphiqueErreur* graph_erreur_;
    QLabel* label_epoch_;
    QLabel* label_erreur_;
    QLabel* label_meilleure_;
    QDoubleSpinBox* spin_seuil_;
    QSpinBox* spin_batch_;
    QPushButton* btn_stop_;

    // Neuron tab
    QLabel* label_id_;
    QLineEdit* edit_nom_;
    QDoubleSpinBox* spin_v_rest_;
    QDoubleSpinBox* spin_tau_;
    QDoubleSpinBox* spin_biais_;
    QDoubleSpinBox* spin_refr_;
    QDoubleSpinBox* spin_eta_;
    QDoubleSpinBox* spin_oubli_;
    QCheckBox* check_entree_;
    QLabel* label_sortie_;
    QLabel* label_v_;

    // Synapse tab
    QLabel* s_label_id_;
    QLabel* s_label_source_;
    QLabel* s_label_cible_;
    QDoubleSpinBox* s_spin_poids_;
    QComboBox* s_combo_type_;
};

#endif
