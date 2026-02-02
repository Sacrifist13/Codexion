/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkraft <kkraft@student42>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/01 08:59:08 by sacrifist         #+#    #+#             */
/*   Updated: 2026/02/02 09:58:09 by kkraft           ###   ########.fr       */
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
	if (d->queue[0].id != -1 && d->queue[1].id != -1 && coder->table->scheduler
		&& d->is_available)
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
	d->queue[0] = d->queue[1];
	d->queue[1].id = -1;
	d->unlock_time = get_time_in_ms() + coder->table->dongle_cooldown;
	d->is_available = 1;
}

static int	simulation_ending(t_coder *coder)
{
	if (pthread_mutex_lock(&coder->table->end_lock) != 0)
		return (0);
	if (coder->table->simulation_end)
	{
		pthread_mutex_unlock(&coder->table->end_lock);
		return (1);
	}
	pthread_mutex_unlock(&coder->table->end_lock);
	return (0);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;

	while (1)
	{
		if (coder->id % 2)
		{
			pthread_mutex_lock(&coder->right->lock);
			pthread_mutex_lock(&coder->left->lock);
		}
		else
		{
			pthread_mutex_lock(&coder->left->lock);
			pthread_mutex_lock(&coder->right->lock);
		}
		push_sort(coder, coder->right);
		push_sort(coder, coder->left);
		if (coder->right->queue[0].id == coder->id && coder->right->unlock_time <= get_time_in_ms()
			&& coder->left->queue[0].id == coder->id && coder->left->unlock_time <= get_time_in_ms())
		{
			coder->right->is_available = 0;
			coder->left->is_available = 0;
			pthread_mutex_unlock(&coder->right->lock);
			pthread_mutex_unlock(&coder->left->lock);
			pthread_mutex_lock(&coder->table->print_lock);
			print_state(coder, "has taken a dongle");
			print_state(coder, "has taken a dongle");
			print_state(coder, "is compiling");
			pthread_mutex_unlock(&coder->table->print_lock);
			pthread_mutex_lock(&coder->compile_lock);
			coder->last_compile_time = get_time_in_ms();
			pthread_mutex_unlock(&coder->compile_lock);
			ft_sleep(coder->table->time_to_compile, coder->table);
			pthread_mutex_lock(&coder->compile_lock);
			coder->nb_compiles++;
			pthread_mutex_unlock(&coder->compile_lock);
			pthread_mutex_lock(&coder->right->lock);
			pthread_mutex_lock(&coder->left->lock);
			pop(coder, coder->right);
			pop(coder, coder->left);
			pthread_mutex_unlock(&coder->right->lock);
			pthread_mutex_unlock(&coder->left->lock);
		}
		else
		{
			pthread_mutex_unlock(&coder->right->lock);
			pthread_mutex_unlock(&coder->left->lock);
			usleep(100);
		}
		if (simulation_ending(coder))
			return (NULL);
	}
}
