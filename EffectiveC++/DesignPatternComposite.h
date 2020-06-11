//
// Created by Jean Pourroy on 02/10/2018.
//

#ifndef TD_DESIGNPATTERNCOMPOSITE_H
#define TD_DESIGNPATTERNCOMPOSITE_H



////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include "NombreContraint.h"
#include "EfficientException.h"

class Colis {
public:
    Colis(const std::string &libelle, const int poids);

    // -- Destructeur virtuel
    // Pourquoi:  - on utilise destructeur virtuel dans une classe X
    //            - c'est nécessairement parce que la classe X servira d'abstraction pour d'autres classes
    // on ne spécifie un destructeur virtuel que si cette classe a au moins une autre méthode virtuelle.
    // Si une classe n'expose aucune méthode virtuelle, alors elle ne peut servir d'abstraction polymorphique
    virtual ~Colis() {}

    // -- Fonction virtuelle pure
    // virtuelle    - méthode qui est destinée à être redéfinie dans les classes qui en héritent
    // pure         - syntaxe spéciale ( = 0 ), aucun objet ne peut etre instantié
    virtual float getPrix() const = 0;

    // -- Fonction même comportement dans les deux classes filles
    // Les colis et colis composés ont un poids propre.
    virtual inline int getPoids() const { return m_poids; }

    // -- Fonction dans seulement une des deux classes
    // Virtuel qui throw une erreur
    // si n'est pas redéfini, n'est pas utilisable pas les autres classes
    virtual inline void ajoute(Colis *c) { throw EfficientException_DesignPattern (c->m_libelle); }

    void affiche() const;

protected:
    NombreContraint<int> m_poids;
private:
    std::string m_libelle;
    static const int POIDS_MIN;
    static const int POIDS_MAX;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ColisSimple : public Colis {
public:
    ColisSimple(const std::string &libelle, const int poids, const float prix);

    // -- Fonction virtuelle pure
    // Doit être redéfinie
    virtual float getPrix() const;

private:
    NombreContraint<float> m_prix;
    static const float PRIX_MIN;
    static const float PRIX_MAX;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ColisCompose : public Colis {
public:
    ColisCompose(const std::string &libelle, const int poids, const float reduction = REDUC_MIN);

    virtual void ajoute(Colis *c);

    // -- Surcharge de getPoids
    // en plus de m_poids, on doit ajouter les colis
    virtual int getPoids() const;

    // -- Fonction virtuelle pure
    // Doit être redéfinie
    virtual float getPrix() const;

private:
    NombreContraint<float> m_reduction;
    std::vector<Colis *> m_colis;
    static const float REDUC_MIN;
    static const float REDUC_MAX;
};


#endif //TD_DESIGNPATTERNCOMPOSITE_H
