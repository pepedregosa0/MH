import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os
import glob

# Configuración de estilo similar a la Figura 1 del guion
sns.set_theme(style="ticks")

DIR_RESULTADOS = "./resultados_boxplot/"
DIR_GRAFICAS = "./graficas_generadas/"

# Crear carpeta para las gráficas si no existe
os.makedirs(DIR_GRAFICAS, exist_ok=True)

# Buscar todos los archivos CSV generados por tu script Bash
archivos_csv = glob.glob(DIR_RESULTADOS + "*.csv")

for archivo in archivos_csv:
	# Extraer el nombre base (ej: resultados_zoo_15)
	nombre_base = os.path.basename(archivo).replace(".csv", "")

	# Leer el CSV (Asumiendo que tiene columnas: Algoritmo, Fitness, etc.)
	# Ajusta los nombres de las columnas según cómo las imprima tu main.cpp
	try:
		df = pd.read_csv(archivo)
		
		plt.figure(figsize=(12, 6))
		
		# Crear el boxplot (Asumiendo que df tiene columnas 'Algoritmo' y 'Fitness')
		# Si tu CSV tiene los algoritmos como columnas y las ejecuciones como filas, 
		# habría que hacer un pd.melt(df) antes.
		ax = sns.boxplot(x="Algoritmo", y="Fitness", data=df, color="#3274A1")
		
		# Mejorar la estética
		plt.title(f"Distribución del Fitness - {nombre_base.upper().replace('_', ' ')}", fontsize=14)
		plt.xticks(rotation=45)
		plt.tight_layout()
		
		# Eliminar bordes superior y derecho (como en la Figura 1 del guion)
		sns.despine()
		
		# Guardar la figura
		ruta_salida = f"{DIR_GRAFICAS}{nombre_base}.png"
		plt.savefig(ruta_salida, dpi=300)
		plt.close()
		
		print(f"Gráfica generada: {ruta_salida}")
		
	except Exception as e:
		print(f"Error procesando {archivo}: {e}")

print("¡Todas las gráficas generadas con éxito!")