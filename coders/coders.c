/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkraft <kkraft@student42>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/01 08:59:08 by sacrifist         #+#    #+#             */
/*   Updated: 2026/02/02 11:54:56 by kkraft           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	push_sort(t_coder *coder, t_dongle *d)
{
	long long	burnout_time;
	t_request	tmp;

	burnout_time = coder->table->time_to_burnout;
	if (d->queue[0].id == coder->id || d->queue[1].id == coder->id)
		return ;
	if (d->queue[0].id == -1)
	{
		d->queue[0].id = coder->id;
		d->queue[0].burnout_time = coder->last_compile_time + burnout_time;
	}
	else
	{
		d->queue[1].id = coder->id;
		d->queue[1].burnout_time = coder->last_compile_time + burnout_time;
	}
	if (d->queue[0].id != -1 && d->queue[1].id != -1 && coder->table->scheduler)
	{
		if (d->queue[0].burnout_time > d->queue[1].burnout_time)
		{
			tmp = d->queue[0];
			d->queue[0] = d->queue[1];
			d->queue[1] = tmp;
		}
	}
}

void	pop(t_coder *coder, t_dongle *d)
{
	if (d->queue[0].id == coder->id)
	{
		d->queue[0] = d->queue[1];
		d->queue[1].id = -1;
	}
	else
		d->queue[1].id = -1;
	d->unlock_time = get_time_in_ms() + coder->table->dongle_cooldown;
	pthread_mutex_lock(&d->lock);
	d->is_available = 1;
	pthread_mutex_unlock(&d->lock);
}

static int	can_compile(t_coder *coder)
{
	long long time;

	time = get_time_in_ms();
	if (coder->right->is_available && coder->left->is_available
		&& coder->right->queue[0].id == coder->id 
		&& coder->left->queue[0].id == coder->id
		&& coder->left->unlock_time <= time
		&& coder->right->unlock_time <= time
	)
		return (1);
	return (0);
}


void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (1)
	{
		push_sort(coder, coder->right);
		push_sort(coder, coder->left);
		if (can_compile(coder))
		{
			pthread_mutex_lock(&coder->right->lock);
			print_state(coder, "has taken a dongle");
			coder->right->is_available = 0;
			pthread_mutex_unlock(&coder->right->lock);
			pthread_mutex_lock(&coder->left->lock);
			print_state(coder, "has taken a dongle");
			coder->left->is_available = 0;
			pthread_mutex_unlock(&coder->left->lock);
			print_state(coder, "is_compiling");
			pthread_mutex_lock(&coder->last_compile_lock);
			coder->last_compile_time = get_time_in_ms();
			pthread_mutex_unlock(&coder->last_compile_lock);
			usleep(coder->table->time_to_compile * 1000);
			pthread_mutex_lock(&coder->nb_compiles_lock);
			coder->nb_compiles++;
			pthread_mutex_unlock(&coder->nb_compiles_lock);
			pop(coder, coder->right);
			pop(coder, coder->left);
		}
	}
}
