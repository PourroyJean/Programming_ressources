//
// Created by Jean Pourroy on 16/10/2018.
//

#ifndef EFFICIENTC_EFFICIENTEXCEPTION_H
#define EFFICIENTC_EFFICIENTEXCEPTION_H

#include <iostream>
#include <sstream>

using namespace std;
class EfficientException : public exception {
public:
    const char *what() const noexcept {
        return "Une Exception";
    }
};


class EfficientException_DesignPattern : public EfficientException{
public:
    EfficientException_DesignPattern(const string &m_libelle) : m_libelle(m_libelle) {}

    const char * what() const noexcept {
        // static car sinon est détruit à la fin de cette fonction et innutilisable à l'exterieur
        static stringstream ss;
        ss <<string(EfficientException::what())  <<  " + de type DesignPattern (" << m_libelle << ")>";
        return ss.str().c_str();
    }
private:
    string m_libelle;
};


#endif //EFFICIENTC_EFFICIENTEXCEPTION_H
