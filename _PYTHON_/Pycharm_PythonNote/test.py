

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


# Lancée seulement lorsque ce fichier est exécutée, par lors d'un import
if __name__ == "__main__":
    mafonction()
    assert  g_Var == 2
