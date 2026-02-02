/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkraft <kkraft@student42>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/01 08:59:08 by sacrifist         #+#    #+#             */
/*   Updated: 2026/02/02 18:15:47 by kkraft           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

// static int	is_simulation_over(t_coder *c)
// {
// 	int		over;
// 	long long last_compile_time;

// 	pthread_mutex_lock(&c->table->end_lock);
// 	over = c->table->simulation_end;
// 	pthread_mutex_unlock(&c->table->end_lock);
// 	if (!over)
// 	{
// 		pthread_mutex_lock(&c->last_compile_lock);
// 		last_compile_time = c->last_compile_time;
// 		pthread_mutex_unlock(&c->last_compile_lock);
// 		if (last_compile_time + c->table->time_to_burnout <= get_time_in_ms())
// 		{
// 			pthread_mutex_lock(&c->table->end_lock);
// 			c->table->simulation_end = 1;
// 			pthread_mutex_unlock(&c->table->end_lock);
// 			return (1);
// 		}
// 		return (0);
// 	}
// 	return (1);
// }

static void	add_request_edf(t_dongle *d, int id, long long burnout_time)
{
	t_request	tmp;

	if (d->queue[0].id == id || d->queue[1].id == id)
		return ;
	if (d->queue[0].id == -1)
	{
		d->queue[0].id = id;
		d->queue[0].burnout_time = burnout_time;
	}
	else
	{
		d->queue[1].id = id;
		d->queue[1].burnout_time = burnout_time;
	}
	if (d->queue[0].id != -1 && d->queue[1].id != -1)
	{
		if (d->queue[0].burnout_time > d->queue[1].burnout_time)
		{
			tmp = d->queue[0];
			d->queue[0] = d->queue[1];
			d->queue[1] = tmp;
		}
	}
}

static int	is_active(t_coder *coder)
{
	int	nb_compiles;

	pthread_mutex_lock(&coder->nb_compiles_lock);
	nb_compiles = coder->nb_compiles;
	pthread_mutex_unlock(&coder->nb_compiles_lock);
	if (nb_compiles >= coder->table->number_of_compiles_required)
		return (0);
	pthread_mutex_lock(&coder->table->end_lock);
	if (coder->table->simulation_end)
		return (pthread_mutex_unlock(&coder->table->end_lock), 0);
	return (pthread_mutex_unlock(&coder->table->end_lock), 1);
}

static void	add_request_fifo(t_dongle *d, int id)
{
	if (d->queue[0].id == id || d->queue[1].id == id)
		return ;
	if (d->queue[0].id == -1)
		d->queue[0].id = id;
	else
		d->queue[1].id = id;
}


static int try_take_dongle(t_coder *coder, t_dongle *d)
{
	long long last_compile_time;
	while (is_active(coder))
	{
		pthread_mutex_lock(&d->lock);
		if (coder->table->scheduler)
		{
			pthread_mutex_lock(&coder->last_compile_lock);
			last_compile_time = coder->last_compile_time;
			pthread_mutex_unlock(&coder->last_compile_lock);
			add_request_edf(d, coder->id, last_compile_time + coder->table->time_to_burnout);
		}
		else
			add_request_fifo(d, coder->id);
		if (d->is_available && d->queue[0].id == coder->id 
			&& d->unlock_time <= get_time_in_ms())
		{
			d->is_available = 0;
			return (pthread_mutex_unlock(&d->lock), 1);
		}
		pthread_mutex_unlock(&d->lock);
	}
	return (0);
}

static void	pop(t_coder *coder, t_dongle *d)
{
	pthread_mutex_lock(&d->lock);
	if (d->queue[0].id == coder->id)
	{
		d->queue[0] = d->queue[1];
		d->queue[1].id = -1;
	}
	else if (d->queue[1].id == coder->id)
		d->queue[1].id = -1;
	d->unlock_time = get_time_in_ms() + coder->table->dongle_cooldown;
	d->is_available = 1;
	pthread_mutex_unlock(&d->lock);
}

static int compile(t_coder *c, t_dongle *first, t_dongle *second)
{
	long long	last_compile_time;
	long		time_to_sleep;

	print_state(c, "has taken a dongle");
	print_state(c, "is compiling");
	pthread_mutex_lock(&c->last_compile_lock);
	c->last_compile_time = get_time_in_ms();
	last_compile_time = c->last_compile_time;
	pthread_mutex_unlock(&c->last_compile_lock);
	time_to_sleep = c->table->time_to_compile;
	if (last_compile_time + c->table->time_to_burnout < get_time_in_ms() + c->table->time_to_compile)
	{
		time_to_sleep = last_compile_time + c->table->time_to_burnout - get_time_in_ms();
		usleep(time_to_sleep * 1000);
		return (0);
	}
	usleep(time_to_sleep * 1000);
	pthread_mutex_lock(&c->nb_compiles_lock);
	c->nb_compiles++;
	pthread_mutex_unlock(&c->nb_compiles_lock);
	pop(c, first);
	pop(c, second);
	return (1);
}

static int	debug(t_coder *c)
{
	long long	last_compile_time;
	long		time_to_sleep;
	
	if (!print_state(c, "is debugging"))
		return (0);
	pthread_mutex_lock(&c->last_compile_lock);
	last_compile_time = c->last_compile_time;
	pthread_mutex_unlock(&c->last_compile_lock);
	time_to_sleep = c->table->time_to_debug;
	if (last_compile_time + c->table->time_to_burnout < get_time_in_ms() + c->table->time_to_debug)
	{
		time_to_sleep = last_compile_time + c->table->time_to_burnout - get_time_in_ms();
		usleep(time_to_sleep * 1000);
		return (0);
	}
	usleep(c->table->time_to_debug * 1000);
	return (1);
}

static int	refactor(t_coder *c)
{
	long long	last_compile_time;
	long		time_to_sleep;
	
	if (!print_state(c, "is refactoring"))
		return (0);
	pthread_mutex_lock(&c->last_compile_lock);
	last_compile_time = c->last_compile_time;
	pthread_mutex_unlock(&c->last_compile_lock);
	time_to_sleep = c->table->time_to_refactor;
	if (last_compile_time + c->table->time_to_burnout < get_time_in_ms() + c->table->time_to_refactor)
	{
		time_to_sleep = last_compile_time + c->table->time_to_burnout - get_time_in_ms();
		usleep(time_to_sleep * 1000);
		return (0);
	}
	usleep(c->table->time_to_refactor * 1000);
	return (1);
}

void	*coder_routine(void *arg)
{
	t_coder		*c;
	t_dongle	*first;
	t_dongle	*second;

	c = (t_coder *)arg;
	if (c->id % 2 == 0)
	{
		first = c->left;
		second = c->right;
	}
	else
	{
		first = c->right;
		second = c->left;
	}
	while (1)
	{
		if (try_take_dongle(c, first))
		{
			if (!print_state(c, "has taken a dongle"))
				break ;
			if (try_take_dongle(c, second))
			{
				if (!compile(c, first, second))
					break ;
				if (!debug(c))
					break ;
				if (!refactor(c))
					break ;
			}
			else
			{
				pthread_mutex_lock(&first->lock);
				first->is_available = 1;
				pthread_mutex_unlock(&first->lock);
				return (NULL);
			}
		}
		else
			return (NULL);
	}
	return (NULL);
}

/* -- FACTORISATION DE LA FONCTION void *coder_routine-- */

/* -- VERIFICATION DU FLAG DE SIMULATION_END DE TABLE A CHAQUE ETAPE -- */

/* -- SLEEP PLUS PETIT SI ON VA BURNOUT -- */