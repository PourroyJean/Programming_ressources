//JEAN: attention, meme nom que dans le .h
#ifdef NOMBRECONTRAINT_H

using namespace std;

template <class T>
NombreContraint<T>::NombreContraint(T valeur, T min, T max) {
  if (min > max) throw "Intervalle min..max incorrect";
  m_min=min; m_max=max;
  setVal(valeur);
}

template <class T>
void NombreContraint<T>::setVal(T valeur) {
  if ((valeur < getMin()) || (valeur > getMax()))
    throw ("Valeur nombre contraint hors intervalle");
  m_val = valeur;
}

template <class T>
T NombreContraint<T>::getMin() const {
  return m_min;
}

template <class T>
T NombreContraint<T>::getMax() const {
  return m_max;
}

template <class T>
T NombreContraint<T>::getVal() const {
  return m_val;
}

template <class T>
void NombreContraint<T>::saisir(istream& entree) {
  T val;
  entree >> val;
  setVal(val);
}

template <class T>
void NombreContraint<T>::afficher(ostream& sortie) const {
  sortie << getVal();
}

template <class T>
NombreContraint<T>::operator T() const {
  return getVal();
}

//JEAN :  On utilise bien une reference vers 'n', sinon on modifie une copie
template <class T>
istream& operator>>(istream& entree, NombreContraint<T> & n) {
  n.saisir(entree);
  return entree;
}

#endif