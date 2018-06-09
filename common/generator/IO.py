import pandas as pd
import numpy as np
import argparse as ap
import pathlib

kw_user = 'USERS'
kw_posts = 'POSTS'
kw_uposts = 'USER_POSTS'
kw_header = 'HEADER'
matrices_keys = [kw_header,kw_user, kw_posts, kw_uposts]

kw_n_users = 'N_USERS'
kw_n_posts = 'N_POSTS'

sum_col = 'SUM'
id_col = 'IDS'
meta_cols = [sum_col, id_col]

def generate_matrix(n_elements):
	idx = range(n_elements)
	out = pd.DataFrame(index=idx, columns=meta_cols+list(idx), dtype=np.int8)
	out[:] = 0x00
	return out


def generate_matrices(n_posts, n_users):
	users = generate_matrix(n_users)
	posts = generate_matrix(n_posts)
	user_posts = pd.DataFrame(index=users.index,columns=['SUM'] + list(posts.index), dtype=np.int8)
	user_posts[:] = 0
	out = {
	kw_user 	: users,
	kw_posts 	: posts,
	kw_uposts	: user_posts
	}
	out[kw_header] = generate_header_frame(out)
	return out

def write_matrices(matrices, dest_folder):
	no_meta = pd.DataFrame()
	for k,frame in matrices.items():
		for meta in set(meta_cols).intersection(set(frame.columns)):
			no_meta = frame.drop(columns=meta)
			
		frame[sum_col] = no_meta.sum(axis=1)
		pathlib.Path(dest_folder).mkdir(parents=True,exist_ok=True)
		frame.to_csv(dest_folder + '/' + k + '.csv')

def read_matrices(source_folder):
	read_path = source_folder + '/'
	out = {}
	for k in matrices_keys:
		out[k] = pd.read_csv(read_path + '/' + k + '.csv', index_col=0)
	return out

def generate_header_frame(matrices):
	n_users = len(matrices[kw_user].index)
	n_posts = len(matrices[kw_posts].index)
	out = pd.DataFrame(data= {
		kw_n_posts : [n_posts],
		kw_n_users : [n_users]
		})
	return out