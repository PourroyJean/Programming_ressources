//
// Created by Jean Pourroy on 02/10/2018.
//

#include <iostream>
#include "DesignPatternComposite.h"


using namespace std;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const int Colis::POIDS_MIN = 1;
const int Colis::POIDS_MAX = 10000;

Colis::Colis(const string &libelle, const int poids)
        : m_libelle(libelle), m_poids(poids, POIDS_MIN, POIDS_MAX) {}

// Comme toutes les opérations des classes filles sont dans la classe mère
// l'affichage peut être géré ici, getPoids () utilisera le polymorphisme
void Colis::affiche() const {
    cout << "Libelle : " << m_libelle << " - Poids : " << getPoids() << "g - Prix : " << getPrix() << "€" << endl;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const float ColisSimple::PRIX_MIN = 0.01;
const float ColisSimple::PRIX_MAX = 1000;

//Constructeur: on construit toujours la classe mère en première
ColisSimple::ColisSimple(const string &libelle, const int poids, const float prix) : Colis(libelle, poids),
                                                                                     m_prix(prix, PRIX_MIN, PRIX_MAX) {
}

float ColisSimple::getPrix() const {
    return float(m_prix);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const float ColisCompose::REDUC_MIN = 0.00;
const float ColisCompose::REDUC_MAX = 0.99;

ColisCompose::ColisCompose(const string &libelle, const int poids, const float reduction) : Colis(libelle, poids),
                                                                                            m_reduction(reduction,
                                                                                                        REDUC_MIN,
                                                                                                        REDUC_MAX),
                                                                                            m_colis() {
}

void ColisCompose::ajoute(Colis *c) {
    m_colis.push_back(c);
}

int ColisCompose::getPoids() const {
    int poids = m_poids;
    for (Colis *colis : m_colis) {
        poids += colis->getPoids();
    }
    return poids;
}

float ColisCompose::getPrix() const {
    int prix = 0;
    for (Colis *colis : m_colis) {
        prix += colis->getPrix();
    }
    return prix * (1.0 - float(m_reduction));
}