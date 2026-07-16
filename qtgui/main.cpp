#include <QApplication>
#include <QFont>
#include "MainWindow.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Éditeur de Réseau de Neurones Biologiques");

    QFont f = app.font();
    QStringList fam = f.families();
    fam << "Noto Color Emoji" << "Symbola" << "Segoe UI Emoji";
    f.setFamilies(fam);
    app.setFont(f);

    MainWindow fenetre;
    fenetre.show();
    return app.exec();
}
