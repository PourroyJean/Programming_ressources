//
// Created by Jean Pourroy on 25/09/2018.
//

#ifndef EFFECTIVEC_VISAGE_H
#define EFFECTIVEC_VISAGE_H


/*
 * File:   main.cpp
 * Author: martin
 *
 * Created on 13 septembre 2015, 18:18
 */

#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>

using namespace std;


class Nez {
private:
    int m_longueur;
public:

    Nez(int longueur) : m_longueur(longueur) {
    }
};


class Ethnie {
private:
    string m_type;
public:
    inline const string &get_type() const {
        return m_type;
    }

public:

    Ethnie(string typ) : m_type(typ) {
    }
};


class Moustache {
private:
    int m_largeur;
public:

    Moustache(int largeur) : m_largeur(largeur) {
    }
};


class Chapeau {
private:
    int m_poids;
public:

    Chapeau(int poids) : m_poids(poids) {
    }
};


class Bouton {
private:
    int m_diametre;
public:

    Bouton(int diametre) : m_diametre(diametre) {
    }
};

class GrainDeBeaute {
private:
    int m_diametre;
public:

    GrainDeBeaute(int diametre) : m_diametre(diametre) {
    }
};

class Bijou {
private:
    int m_prix;
public:
    int get_prix() const {
        return m_prix;
    }

public:

    Bijou(int prix) : m_prix(prix) {
    }
};

class Visage {
private:

    // composition 1..1
    // -- Ne vis pas en dehors de visage
    // -- Pas besoin d'être nul (pas de *)
    Nez m_monNez;

    // agregation 1..1
    // -- Existe en dehors (& ou *)
    // -- Mais ne peut pas être nul (&)
    // -- On veut pas pouvoir modifier l'ethnie depuis le visage (const &)
    const Ethnie &m_monEthnie;

    // agregation 0..1
    // -- Existe en dehors (& ou *)
    // -- mais peut être nul donc *
    // -- On ne veut pas pouvoir modifier le chapeau depuis le visage (const *)
    const Chapeau *m_monChapeau;

    // composition 0..1
    // -- Ne vis pas en dehors de visage, pas de référence
    // -- peut être nul (*)
    // -- peut être modifié depuis le visage (pas const)
    Moustache *m_maMoustache;

    // composition 0..*
    // -- Ne vis pas en dehors de visage, pas de référence
    // -- C'est les notre pas de const
    // -- Vecteur de pointeur ou de bouton: pointeur plus performant lors d'un push back (pas de duplication du bouton lors de l'ajout)
    vector<Bouton *> m_mesBoutons;

    // agregation 0..*
    // -- Le vecteur de mes bijoux ne vis pas en dehors (bien que les bijoux existent)
    // -- Ce ne sont pas les notres (const)
    // -- Vecteur de pointeur ou de bijoux : ils existent déjà, on stocke un pointeur
    vector<const Bijou *> m_mesBijoux;

    //Test
    vector<GrainDeBeaute> m_mesGrainsDeBeautes;     // composition 0..*

public:
    Visage(int longueur,
           const Ethnie &uneEthnie); //Le constructeur doit obligatoirement prendre une ethnie en paramètre pour la construire, comme c'est une référence, peut pas être null même temporairement

    void setMoustache(int largeur);

    void addBouton(int diametre);

    void addGrainDeBeaute(int diametre);

    void addChapeau(const Chapeau &unChapeau);

    void addBijou(const Bijou &unBijou);

    const Visage &operator=(const Visage &unVisage);

    Visage(const Visage &unVisage);

    ~Visage();

    friend std::ostream &operator<<(std::ostream &sortie, const Visage &V) {
        std::cout << " -- Affichage d'un visage -- \n"
                  << "Ethnie     : " << V.m_monEthnie.get_type() << "\n"
                  << "Mes bijoux : ";
        for (auto b : V.m_mesBijoux) { cout<< b->get_prix()  << " " ; }
        cout << "\n";
        return sortie;

    }
};


#endif //EFFECTIVEC_VISAGE_H
