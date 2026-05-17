import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import os

def main():
    # Lista para ir guardando las medias de todos los archivos
    datos_agrupados = []
    
    # Buscamos todos los archivos CSV
    fnames = [f for f in os.listdir(".") if f.endswith(".csv")]
    
    if not fnames:
        print("No se encontraron archivos .csv en el directorio actual.")
        return

    for fname in fnames:
        df = pd.read_csv(fname)
        df['alg'] = df['alg'].str.upper()
        
        # Filtramos para quedarnos solo con los dos AGG que te interesan
        df_filtrado = df[df['alg'].isin(['AGG-UN', 'AGG-SF'])]
        
        if df_filtrado.empty:
            continue
            
        # Calculamos la media del fitness agrupando por algoritmo
        df_medias = df_filtrado.groupby('alg')['fitness'].mean().reset_index()
        
        # Extraemos el nombre del escenario del nombre del archivo
        # Asumiendo que tus archivos se llaman "resultados_zoo_30.csv", etc.
        escenario = fname.replace('.csv', '').replace('resultados_', '')
        df_medias['escenario'] = escenario
        
        # Añadimos este trocito de datos a la lista global
        datos_agrupados.append(df_medias)
        
    if not datos_agrupados:
        print("No se encontraron datos de AGG-UN o AGG-SF en los CSVs.")
        return

    # Unimos todos los dataframes pequeños en uno solo grande
    df_final = pd.concat(datos_agrupados, ignore_index=True)
    
    # ----- CONFIGURACIÓN DE LA GRÁFICA -----
    
    # Hacemos la figura bastante ancha
    plt.figure(figsize=(14, 7))
    
    # Ponemos un estilo de fondo con líneas horizontales (como en tu foto)
    sns.set_theme(style="whitegrid")
    
    # Dibujamos las barras (sns.barplot dibuja medias por defecto si le das datos crudos, 
    # pero como ya hemos calculado la media, simplemente dibujará el valor exacto)
    grafica = sns.barplot(
        data=df_final, 
        x="escenario", 
        y="fitness", 
        hue="alg",
        palette="Set2" # Una paleta de colores suave
    )
    
    # Añadimos títulos y etiquetas
    grafica.set_xlabel("Escenario (Dataset_Restricciones)", fontsize=12, labelpad=10)
    grafica.set_ylabel("Fitness Medio (Menor es mejor)", fontsize=12)
    grafica.set_title("Comparativa de Fitness Medio: AGG-UN vs AGG-SF", fontsize=16, pad=15)
    
    # El truco para poner los valores encima de cada barra
    for contenedor in grafica.containers:
        grafica.bar_label(
            contenedor, 
            fmt='%.3f',      # 3 decimales
            padding=3,       # Separación de la barra
            rotation=90,     # Texto en vertical
            fontsize=10
        )
        
    # Movemos la leyenda fuera del gráfico para que no tape las barras
    plt.legend(title='Algoritmo', bbox_to_anchor=(1.01, 1), loc='upper left')
    
    # Ajustamos márgenes para que no se corte nada
    plt.tight_layout()
    
    # Guardamos la imagen final
    plt.savefig("comparativa_AGGs.png", dpi=300)
    print("Gráfica guardada exitosamente como 'comparativa_AGGs.png'")
    
    # Cerramos
    plt.close()

if __name__ == '__main__':
    main()