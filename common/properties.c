#include "properties.h"
#include <stdint.h>
#include "log.h"
#include <stdio.h>
#include <stdlib.h>

int teardown_example_properties(void ** state)
{
	ex_props_t * properties = *((ex_props_t**)state);
	teardown_posts(properties->posts, properties->n_posts);
	teardown_users(properties->users, properties->n_users);
	free(properties);
	return 0;
}

void teardown_users(user * users, size_t n_users)
{
	if (!users)
	{
		LOG_E("Null pointer given %c", '!');
		return;
	}
	size_t i = 0;
	for (i = 0; i < n_users; i++)
	{
		if (users[i].n_followers)
		{
			free(users[i].follower_idxs);
			users[i].n_followers = 0;
		}
		if (users[i].n_following)
		{
			free(users[i].following_idxs);
			users[i].n_following = 0;
		}
		if (users[i].n_posts)
		{
			free(users[i].post_idxs);
			users[i].n_posts = 0;
		}
	}
	free(users);
}

void teardown_posts(post * posts, size_t size)
{
	if (!posts)
	{
		LOG_E("Null pointer given! %c", ' ');
		return;
	}
	size_t i = 0;
	for (i = 0; i < size ; i++)
	{
		
		if (posts[i].reposted_idxs)
			{
				free(posts[i].reposted_idxs);
				posts[i].reposted_idxs = NULL;
			} 
	}
	free(posts);
}