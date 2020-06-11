//
// Created by Jean Pourroy on 25/09/2018.
//

#include "Corps.h"



Visage::Visage(int longueur, const Ethnie &uneEthnie)
        : m_monNez(longueur), m_monEthnie(uneEthnie), m_maMoustache(nullptr), m_monChapeau(nullptr) {
}

void Visage::setMoustache(int largeur) {
    if (m_maMoustache == nullptr)
        m_maMoustache = new Moustache(largeur);
    else
        *m_maMoustache = Moustache(largeur);
}

void Visage::addBouton(int diametre) {
    m_mesBoutons.push_back(new Bouton(diametre));
}

void Visage::addGrainDeBeaute(int diametre) {
    m_mesGrainsDeBeautes.push_back(GrainDeBeaute(diametre));
}

void Visage::addChapeau(const Chapeau &unChapeau) {
    m_monChapeau = &unChapeau;
}

void Visage::addBijou(const Bijou &unBijou) {
    m_mesBijoux.push_back(&unBijou);
}

const Visage &Visage::operator=(const Visage &unVisage) {
    m_monNez = unVisage.m_monNez;
    // On ne peut pas changer l'ethnie qui est une référence : impossible de faire m_monEthnie = unVisage.m_monEthnie;
    if (unVisage.m_maMoustache != nullptr)
        m_maMoustache = new Moustache(*unVisage.m_maMoustache);
    else
        m_maMoustache = nullptr;
    m_monChapeau = unVisage.m_monChapeau;
    for (auto ptrBouton : m_mesBoutons)
        m_mesBoutons.push_back(new Bouton(*ptrBouton));
    m_mesBijoux = unVisage.m_mesBijoux;

    return *this;
}

Visage::Visage(const Visage &unVisage)
        : m_monNez(unVisage.m_monNez), m_monEthnie(unVisage.m_monEthnie) {
    *this = unVisage;
}

Visage::~Visage() { // on supprime les objets qu'on a créés et seulement ceux là
    if (m_maMoustache != nullptr) delete m_maMoustache;
    for (auto ptrbouton : m_mesBoutons) delete ptrbouton;
}

