import IO
import UI
import argparse as ap
import numpy as np
import pandas as pd

def main():
	parser = ap.ArgumentParser(description="Build your own supergraph datasets! Specify a folder to read from, or the number of posts/users to gen")
	
	io_args = parser.add_mutually_exclusive_group(required=True)
	io_args.add_argument("-f","--file", help="Specify the folder from which to read")
	io_args.add_argument("-g", "--generate", help="Generate new data. users_n and posts_n must be set.",action='store_true')
	parser.add_argument("-u", "--users_n", help="Specify Number of users to make", default=0, type=np.int64)
	parser.add_argument("-p", "--posts_n", help="Specify Number of posts to make", default=0, type=np.int64)
	parser.add_argument("-r", "--randomize_ids", help="Randomize the ID values for users and posts", action='store_true')
	args = parser.parse_args()
	matrices = {}
	if (args.generate):
		if not (args.users_n or args.posts_n):
			print("No users/posts specified for generate!")
			return
		matrices = IO.generate_matrices(args.posts_n,args.users_n)
	else:
		matrices = IO.read_matrices(args.file)
	
	UI.display_ui(matrices)

	if (args.randomize_ids):
		for df_kw in [IO.kw_user, IO.kw_posts]:
			df = matrices[df_kw]
			df[IO.id_col] = np.random.permutation(range(len(df.index)))

	dirout = input("Enter an output file directory name, or nothing:\n").strip()

	if dirout:
		IO.write_matrices(matrices, dirout)
		return
	print("Nothing written, exiting")

if __name__ == '__main__':
	main()
