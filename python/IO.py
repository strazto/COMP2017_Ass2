import pandas as pd
import numpy as np
import argparse as ap

def generate_matrix(n_elements):
	idx = range(n_elements)
	out = pd.DataFrame(index=idx, columns=['IDS']+list(idx), dtype=np.int8)
	out[:] = 0x00
	return out


def generate_matrices(n_posts, n_users):
	users = generate_matrix(n_users)
	posts = generate_matrix(n_posts)
	user_posts = pd.DataFrame(index=users.index,columns=posts.index, dtype=np.int8)
	user_posts[:] = 0
	out = {
	'USERS' 	: users,
	'POSTS' 	: posts,
	'USER_POSTS': user_posts
	}
	return out




frame = generate_matrix(7)
print(frame)