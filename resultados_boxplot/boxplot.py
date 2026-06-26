import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import os

def main():
    fnames:list[str] = [f for f in os.listdir(".") if ".csv" in f]
    for fname in fnames:
        df = pd.read_csv(fname)
        df['alg'] = df['alg'].str.upper()
        
        # aspect=2 o 2.5 hace la gráfica más ancha manteniendo la altura
        p = sns.catplot(data=df, x="alg", y="fitness", aspect=1.5, kind="box")
        p.set(xlabel="Algoritmo", ylabel="Fitness")
        
        # Rotamos las etiquetas del eje X 45 grados para que se lean perfectamente
        plt.xticks(rotation=45)
        
        # Ajustamos el layout para asegurarnos de que las etiquetas rotadas no se corten al guardar
        p.tight_layout()
        
        foutput = fname.replace(".csv", ".png")
        p.savefig(foutput)
        
        # Cerramos la figura para liberar memoria y evitar que se superpongan en la siguiente iteración
        plt.close()
        
if __name__ == '__main__':
    main()