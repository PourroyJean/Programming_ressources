

g_Var = 1

#Definition et allocation d'une fonction lambda
f_plus = lambda x, y: x + y

def mafonction ():
    global g_Var
    g_Var =2
    res_var = """Result of 
     {var0}
    +{v1}
    ===
     {v2}""".format(var0=g_Var, v1=2, v2=f_plus(g_Var,2))
    print(res_var)

def lesListes ():
    ma_liste = list () #liste vide
    ma_liste = [1, 2, ['a','b','c']]  # Une liste avec cinq objets

    print(type(ma_liste))
    for elt, i in enumerate(ma_liste): # i est l'indice dans la liste, elt l'élément
        print("À l'indice {} se trouve {}.".format(i, elt))
    print(ma_liste[2][1]) # affiche b

    # À la différence des listes, les tuples, une fois créés, ne peuvent être modifiés : on ne peut plus y ajouter d'objet ou en retirer.
    # Un tuple se définit comme une liste, sauf qu'on utilise comme délimiteur des parenthèses au lieu des crochets.
    mon_tuple = (1,2,3)
    print(type(mon_tuple))

    liste_des_parametres = [1, 4, 9, 16, 25, 36]
    print(liste_des_parametres)
    print(*liste_des_parametres)

# Lancée seulement lorsque ce fichier est exécutée, par lors d'un import
if __name__ == "__main__":

    #mafonction()
    assert  g_Var == 1

    lesListes()

    print ("salut",
           "a")

    liste = ['abricot',2,'chat',4]
    for i in liste:
        print ("La valeur dans la liste est ", i)
        if i == 'chat':
            print ("c'est un chat")