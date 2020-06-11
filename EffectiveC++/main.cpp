#include <iostream>
#include "NombreContraint.h"
#include "Corps.h"
#include "DesignPatternComposite.h"

using namespace std;

void testReference();

void testEntierContraint();

void testCorps();

void testPatternComposite ();


int main() {

//    testEntierContraint();
//    testCorps();
    testPatternComposite();
    return 0;

}



int &F(int &a, int b) {

    //Une reference pointera toujours vers la même variable
    //On veut changer sa valeur, mais pas la variable poitnée
    int i = 10;
    int j = 20;
    int &r = i;
    r = j;
    r = 3000;
    cout << "r pointe toujours vers i, en modifiant r, la valeur de i est " << i << endl;

    //a est passé en référence, le modifié dans la fonction, le modifie aussi dans l'appelant
    a = 1000;
    b = 2000;

    int c = 4000;

    // warning: reference to stack memory associated with local variable 'c'
    return c;
}

void testReference() {

    //int &ref;    #impossible
    int a = 1;
    int b = 2;
    int &c = F(a, b); //FAUX
    cout << "a == " << a << " # a est modifié dans la fonction car passé par référence" << endl;
    cout << "b == " << b << " # pas b car passé par copie" << endl;
    cout << "c == " << c
         << " # c est une reference vers une variable locale a la fonction appelé qui a disparu a la fin de son appel == valeur fausse";

}

void testEntierContraint() {
    cout << "DEBUT testEntierContraint" << endl << endl;

    NombreContraint<int> ec1;
    NombreContraint<int> ec2(2, 1, 3);

    cout << "ec1 contient : ";
    ec1.afficher(cout);
    cout << " (devrait afficher \"0 dans [0;100]\")" << endl;
    cout << "ec2 contient : ";
    ec2.afficher(cout);
    cout << " (devrait afficher \"2 dans [1;3]\")" << endl;

    cout << endl << "Modification de ec1 :" << endl;
    ec1.setVal(50);
    cout << "ec1 contient : ";
    ec1.afficher(cout);
    cout << " (devrait afficher \"50 dans [0;100]\")" << endl;

    cout << endl << "Tentative de min à 100 et max à  0 :" << endl;
    try {
        ec1.setVal(120);
        cout << "L'exception n'a pas été levée correctement." << endl;
    } catch (const char *e) {
        cout << "On a eu chaud, tout marche : " << e << endl;
    }

//    cout << endl << "Saisir et afficher ec1 via les surcharges d'opérateur : " << endl;
//    cin >> ec1;
//    cout << "ec1 contient : " << ec1;

    cout << endl << endl << "FIN testEntierContraint" << endl << endl;
}

void testCorps() {

    //"caucasien" est de type const string.
    // Si on le passe dans le constructeur Ethnie (string s)
    //          --> pas de problème car on fait une copie de la chaine de caractère
    // Si on veut utiliser une référence Ethnie (string & s)
    //          --> impossible car "caucasien" est de type const string.
    //          --> par contre on peut le passer avec un objet string S = "caucasien";
    // Si on veut permettre de passer "caucasien" on utilise un constructeur qui prend une reference const string
    //          --> Ethnie (string & s)
    Ethnie uneEthnie("caucasien");


    Chapeau unChapeau(100);
    Bijou boucle1(10);
    Bijou boucle2(20);
    Visage unVisage(7, uneEthnie);
    unVisage.setMoustache(8);
    unVisage.addChapeau(unChapeau);
    unVisage.addBijou(boucle1);
    unVisage.addBijou(boucle2);
    unVisage.addBouton(1);
    unVisage.addBouton(2);

    cout << unVisage << endl;


}

void testPatternComposite (){
    try {
        cout << " --- PACK COMPOSE HYGIENE CONTENANT UN SAVON ET UN DENTIFRICE --- " << endl;
        ColisSimple *savon = new ColisSimple("Savon", 200, 3.0); // Savon 200g 3€
        ColisSimple *dentifrice = new ColisSimple("Dentifrice", 50, 2.0); //
        ColisCompose *hygiene = new ColisCompose("Pack Hygiène", 25);
        hygiene->ajoute(savon);
        hygiene->ajoute(dentifrice);

        savon->affiche();
        dentifrice->affiche();
        hygiene->affiche();


        cout << endl << " --- PACK COMPOSE LUXE CONTENANT LE PACK HYGIENE PLUS UN PARFUM  --- " << endl;
        ColisSimple *parfum = new ColisSimple("Parfum", 100, 95);
        ColisCompose *luxe = new ColisCompose("Pack Luxe", 25, 0.1);
        luxe->ajoute(hygiene);
        luxe->ajoute(parfum);

        parfum->affiche();
        luxe->affiche();
        parfum->ajoute(savon); // Doit lever une exception
    }
    //On catch par référence pour que le polymorphisme puisse fonctionner et appeler la fonction what() de la classe fille
    catch (EfficientException & s ) {
        cout << "EfficientException caught --> ";
        cout << s.what();
    }
}


