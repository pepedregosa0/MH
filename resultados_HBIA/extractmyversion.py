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

    path = Path(target_dir)

    if not path.is_dir():
        print(f"Error, directory '{target_dir}' does not exist", file=sys.stderr)
        sys.exit(1)

    print(f"Working in '{target_dir}'")
    os.chdir(target_dir)
    path = Path('.')

    fnames = [file.name for file in path.iterdir() if "txt" in file.name]
    if not fnames:
        print("No .txt files found.", file=sys.stderr)
        sys.exit(1)

    globaldf = pd.concat([pd.read_csv(fname) for fname in fnames], ignore_index=True)

    def funformat(funid):
        return "F{}".format(int(funid))

    dim_df = globaldf.groupby(['dim'])

    for (dim, df) in dim_df:
        if isinstance(dim, tuple):
            dim = dim[0]

        milestones = sorted([int(mil) for mil in np.unique(df.milestone)])
        
        funs_total = list(range(1, 31))
        columns = ['alg', 'milestone', 'dimension'] + [funformat(f) for f in funs_total]
        output_rows = []

        mean_df = df.groupby(['funcid', 'milestone'])['error'].mean().reset_index()

        for milestone in milestones:
            mildf = mean_df[mean_df.milestone == milestone]
            
            row_dict = {
                'alg': 'HBIA',  # Asegúrate de escribir "HBIA" exactamente igual en la web de TACO
                'milestone': int(milestone), 
                'dimension': int(dim)  # CRÍTICO: Forzar entero para evitar el formato float (10.0) en Excel
            }
            
            for f in funs_total:
                row_dict[funformat(f)] = 1.0
            
            for _, row in mildf.iterrows():
                fun_col = funformat(row.funcid)
                row_dict[fun_col] = float(row.error)
                
            output_rows.append(row_dict)

        output_df = pd.DataFrame(output_rows, columns=columns)
        
        fname = "results_cec2017_{}.xlsx".format(int(dim))
        print(f"Saving {fname}...")
        
        # Guardamos forzando a que los metadatos numéricos no lleven decimales flotantes
        output_df['milestone'] = output_df['milestone'].astype(int)
        output_df['dimension'] = output_df['dimension'].astype(int)
        
        output_df.to_excel(fname, index=False)

if __name__ == '__main__':
    main()