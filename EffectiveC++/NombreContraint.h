#ifndef NOMBRECONTRAINT_H
#define	NOMBRECONTRAINT_H

#include <iostream>

template <class T>
class NombreContraint {
public:
    NombreContraint(T valeur = 0, T min = 0, T max = 100);
    T getMin() const;
    T getMax() const;
    T getVal() const;
    void setVal(T valeur);
    void saisir(std::istream & entree);
    void afficher(std::ostream & sortie) const;
    operator T() const;

    //JEAN: 1ère façon de surcharger <<, avec Friend, la fonction a acces a toutes les variables de classes
    friend std::ostream& operator<<(std::ostream& sortie, const NombreContraint<T> & n){
        n.afficher(sortie);
        return sortie;
    }

private:

    T m_min;
    T m_max;
    T m_val;

};

//JEAN: 2e façon de surcharger: en exterieur
template <class T>
std::istream& operator>>(std::istream& entree, NombreContraint<T>& n);


//JEAN Ecrire le code du template directement dans le .h ou inclure le c++
#include "NombreContraint.cpp"

#endif	/* ENTIERCONTRAINT_H */