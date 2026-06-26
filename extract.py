from pathlib import Path
import os
import sys
import numpy as np
import pandas as pd

def main():
    if len(sys.argv) == 1:
        target_dir = "."
    else:
        target_dir = sys.argv[1]

    base_path = Path(target_dir)

    if not base_path.is_dir():
        print(f"Error, directory '{target_dir}' does not exist", file=sys.stderr)
        sys.exit(1)

    print(f"Working in '{target_dir}'")
    
    # Definimos las carpetas de los algoritmos que queremos buscar
    target_folders = ["results_HBIA", "results_MHBIA", "results_HBIAExt"]
    
    all_dfs = []
    
    # Recorremos cada carpeta y leemos sus archivos .txt
    for folder_name in target_folders:
        folder_path = base_path / folder_name
        
        if not folder_path.is_dir():
            print(f"Warning: Folder '{folder_name}' not found. Skipping...", file=sys.stderr)
            continue
            
        fnames = [file for file in folder_path.iterdir() if "txt" in file.name]
        
        if not fnames:
            print(f"Warning: No .txt files found in '{folder_name}'.", file=sys.stderr)
            continue
            
        # Al leer, añadimos una columna con el nombre del algoritmo (ej. HBIA, MHBIA, HBIAExt)
        # Limpiamos el prefijo 'results_' para dejar solo el nombre limpio del algoritmo
        alg_name = folder_name.replace("results_", "")
        
        for fname in fnames:
            try:
                df = pd.read_csv(fname)
                df['alg'] = alg_name  # Identificador del algoritmo
                all_dfs.append(df)
            except Exception as e:
                print(f"Error reading {fname}: {e}", file=sys.stderr)

    if not all_dfs:
        print("Error: No data could be loaded from any folder.", file=sys.stderr)
        sys.exit(1)

    # Juntamos todos los datos de todas las carpetas
    globaldf = pd.concat(all_dfs, ignore_index=True)

    def funformat(funid):
        return "F{}".format(int(funid))

    # Agrupamos por dimensión para generar un Excel por cada una
    dim_groups = globaldf.groupby(['dim'])

    for (dim, df_dim) in dim_groups:
        if isinstance(dim, tuple):
            dim = dim[0]

        funs_total = list(range(1, 31))
        columns = ['alg', 'milestone', 'dimension'] + [funformat(f) for f in funs_total]
        output_rows = []

        # Calculamos la media agrupando TAMBIÉN por algoritmo ('alg')
        mean_df = df_dim.groupby(['alg', 'funcid', 'milestone'])['error'].mean().reset_index()

        # El orden en el que se agregarán las filas hacia abajo viene determinado por este array
        # Modifica este orden si prefieres ver un algoritmo antes que otro
        alg_order = ["HBIA", "MHBIA", "HBIAExt"]
        
        # Iteramos respetando rigurosamente el orden de los algoritmos
        for alg in alg_order:
            # Filtramos por el algoritmo actual
            df_alg = mean_df[mean_df.alg == alg]
            if df_alg.empty:
                continue
                
            milestones = sorted([int(mil) for mil in np.unique(df_alg.milestone)])
            
            for milestone in milestones:
                mildf = df_alg[df_alg.milestone == milestone]
                
                row_dict = {
                    'alg': alg, 
                    'milestone': int(milestone), 
                    'dimension': int(dim)
                }
                
                # Inicializar columnas F1-F30 por defecto
                for f in funs_total:
                    row_dict[funformat(f)] = 1.0
                
                # Rellenar con los errores calculados
                for _, row in mildf.iterrows():
                    fun_col = funformat(row.funcid)
                    row_dict[fun_col] = float(row.error)
                    
                output_rows.append(row_dict)

        if not output_rows:
            continue

        output_df = pd.DataFrame(output_rows, columns=columns)
        
        # Forzar enteros en metadatos para evitar el formato float (.0) en Excel
        output_df['milestone'] = output_df['milestone'].astype(int)
        output_df['dimension'] = output_df['dimension'].astype(int)
        
        fname = os.path.join(target_dir, "results_cec2017_{}.xlsx".format(int(dim)))
        print(f"Saving {fname}...")
        output_df.to_excel(fname, index=False)

if __name__ == '__main__':
    main()