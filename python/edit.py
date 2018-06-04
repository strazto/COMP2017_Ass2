import pandas as pd
import numpy as np
import argparse as ap
import tkinter as tk
from functools import partial

def toggle_df(frame, row, column):
    frame.iloc[row, column] = 1 if (not frame.iloc[row,column]) else 0
    print("Row:" + str(row) + "Col" + str(column))
    print(frame)

df = pd.DataFrame(data={1 : [0, 1], 2 : [0, 0], 3 : [1, 1]}, dtype=np.int8)

print(df)

root = tk.Tk(  )

for c in range(len(df.columns)):
    tk.Label(root, text=str(df.columns[c])).grid(row=0,column=c+1)

for r in df.index:
    tk.Label(root, text=str(df.index[r])).grid(row=r+1,column=0)  
    for c in range(len(df.columns)):
        int_var = tk.IntVar()
        int_var.set(df.iloc[r,c])
        tk.Checkbutton(root, borderwidth=1, var=tk.IntVar(), command=partial(toggle_df,df,r,c)).grid(row=r + 1,column=c + 1)
root.mainloop(  )
