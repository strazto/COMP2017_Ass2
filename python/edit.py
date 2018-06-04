import IO
import pandas as pd
import numpy as np
import argparse as ap
import tkinter as tk
from functools import partial

def toggle_df(frame, row, column):
    frame.iloc[row, column] = 1 if (not frame.iloc[row,column]) else 0
    print("Row:" + str(row) + "Col" + str(column))
    print(frame)

def disp_bool_df(root, df, no_diag=1):
	for c in range(len(df.columns)):
	    tk.Label(root, text=str(df.columns[c])).grid(row=0,column=c+1)

	for r in df.index:
	    tk.Label(root, text=str(df.index[r])).grid(row=r+1,column=0)  
	    for c in range(len(df.columns)):
	        if (df.index[r] != df.columns[c] or not no_diag):
		        button = tk.Checkbutton(root, borderwidth=1, var=tk.IntVar(), command=partial(toggle_df,df,r,c))
		        if (df.iloc[r,c]):
		            button.select()
		        button.grid(row=r + 1,column=c + 1)


#df = pd.DataFrame(data={1 : [0, 1], 2 : [0, 0], 3 : [1, 1]}, dtype=np.int8)

#print(df)


matrices = IO.generate_matrices(10, 12)


root = tk.Tk(  )

posts_view = tk.LabelFrame(root, text="POSTS")
posts_view.pack(fill="both", expand="yes")
disp_bool_df(posts_view, matrices['POSTS'])

user_view = tk.LabelFrame(root,text="USERS")
user_view.pack(fill="both", expand="yes")
disp_bool_df(user_view, matrices['USERS'])

user_posts_view = tk.LabelFrame(root,text="USER_POSTS")
user_posts_view.pack(fill="both", expand="yes")
disp_bool_df(user_posts_view, matrices['USER_POSTS'], no_diag=0)

root.mainloop(  )


