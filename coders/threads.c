/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkraft <kkraft@student42>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 16:31:56 by sacrifist         #+#    #+#             */
/*   Updated: 2026/02/02 11:09:40 by kkraft           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* -- Init Coders (Memory and mutex) -- */
/* -- Init Dongles (Memory and mutex) -- */
/* -- 
	Start threads:
		- (init table mutex)
		- (create all coders threads) 
		- (create monitor threads)
		- (join monitor)

	End threads:
		- (join all coders's threads)
		- (destroy all elements)
-- */

static int	init_dongles(t_table *table)
{
	int	i;

	table->dongles = ft_malloc(sizeof(t_dongle) * table->number_of_coders);
	if (!table->dongles)
		return (0);
	i = 0;
	while (i < table->number_of_coders)
	{
		table->dongles[i].queue[0].id = -1;
		table->dongles[i].queue[1].id = -1;
		table->dongles[i].is_available = 1;
		table->dongles[i].unlock_time = 0;
		table->dongles[i].id = i;
		if (pthread_mutex_init(&table->dongles[i].lock, NULL) != 0)
			return (0);
		if (pthread_cond_init(&table->dongles[i].cond, NULL) != 0)
			return (0);
		i++;
	}
	return (1);
}

static int	init_coders(t_table	*table)
{
	int	i;
	int	nb_coders;

	nb_coders = table->number_of_coders;
	table->coders = ft_malloc(sizeof(t_coder) * nb_coders);
	if (!table->coders)
		return (0);
	i = 0;
	while (i < nb_coders)
	{
		table->coders[i].id = i + 1;
		table->coders[i].nb_compiles = 0;
		table->coders[i].right = &table->dongles[i];
		if (i == 0)
			table->coders[i].left = &table->dongles[nb_coders - 1];
		else
			table->coders[i].left = &table->dongles[i - 1];
		table->coders[i].table = table;
		if (pthread_mutex_init(&table->coders[i].nb_compiles_lock, NULL) != 0)
			return (0);
		if (pthread_mutex_init(&table->coders[i].last_compile_lock, NULL) != 0)
			return (0);
		i++;
	}
	return (1);
}

int	start_threads(t_table *table)
{
	pthread_t	monitor_id;
	int			i;
	long long	start_time;

	if (!init_dongles(table) || !init_coders(table))
		return (0);
	if (pthread_mutex_init(&table->end_lock, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&table->print_lock, NULL) != 0)
		return (0);
	i = -1;
	start_time = get_time_in_ms();
	table->start_time = start_time;
	while (++i < table->number_of_coders)
	{
		table->coders[i].last_compile_time =  get_time_in_ms();
		if (pthread_create(&table->coders[i].thread_id, NULL,
				&coder_routine, (void *)&table->coders[i]) != 0)
			return (0);
	}
	if (pthread_create(&monitor_id, NULL, &monitor_routine, (void *)table) != 0)
		return (0);
	if (pthread_join(monitor_id, NULL) != 0)
		return (0);
	return (1);
}

int	end_threads(t_table *table)
{
	int	i;

	i = -1;
	while (++i < table->number_of_coders)
	{
		if (pthread_join(table->coders[i].thread_id, NULL) != 0)
			return (0);
	}
	i = -1;
	while (++i < table->number_of_coders)
	{
		if (pthread_cond_destroy(&table->dongles[i].cond) != 0)
			return (0);
		if (pthread_mutex_destroy(&table->dongles[i].lock) != 0)
			return (0);
		if (pthread_mutex_destroy(&table->coders[i].nb_compiles_lock) != 0)
			return (0);
		if (pthread_mutex_destroy(&table->coders[i].last_compile_lock) != 0)
			return (0);
	}
	return (1);
}
