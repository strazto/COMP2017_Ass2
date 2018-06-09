import IO
import pandas as pd
import numpy as np
import argparse as ap
import tkinter as tk
from functools import partial
import sys

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


def display_ui(matrices):
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




# matrices = IO.generate_matrices(10, 12)

# test_folder = "./TEST_OUT"
# IO.write_matrices(matrices, test_folder)

# m2 = IO.read_matrices(test_folder)

# for k in m2:
# 	print(m2[k])

def main(argv):
	parser = ap.ArgumentParser(description="Build your own supergraph datasets! Specify a folder to read from, or the number of posts/users to gen")
	
	io_args = parser.add_mutually_exclusive_group(required=True)
	io_args.add_argument("-f","--file", help="Specify the folder from which to read")
	io_args.add_argument("-g", "--generate", help="Generate new data. users_n and posts_n must be set.",action='store_true')
	parser.add_argument("-u", "--users_n", help="Specify Number of users to make", default=0, type=np.int64)
	parser.add_argument("-p", "--posts_n", help="Specify Number of posts to make", default=0, type=np.int64)

	args = parser.parse_args()
	matrices = {}
	if (args.generate):
		if not (args.users_n or args.posts_n):
			print("No users/posts specified for generate!")
			return
		matrices = IO.generate_matrices(args.posts_n,args.users_n)
	else:
		matrices = IO.read_matrices(args.file)

	display_ui(matrices)

	dirout = input("Enter an output file directory name, or nothing").strip()

	if dirout:
		IO.write_matrices(matrices, dirout)
		return
	print("Nothing written, exiting")
	
if __name__ == '__main__':
	main(sys.argv)