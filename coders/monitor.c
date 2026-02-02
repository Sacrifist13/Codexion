/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkraft <kkraft@student42>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 16:30:10 by sacrifist         #+#    #+#             */
/*   Updated: 2026/02/02 09:41:24 by kkraft           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* -- Monitor - Check Coders Burnout and Coders Compilation Times -- */
/* -- Stop the simulation if Burnout or if Compilation
Times Done for all coders -- */

static int	check_burnout(t_table *table, t_coder *coder)
{
	if (pthread_mutex_lock(&coder->compile_lock) != 0)
		return (0);
	if (coder->last_compile_time + table->time_to_burnout <= get_time_in_ms())
	{
		printf("Coder %d: nb_compiles: %d\n", coder->id, coder->nb_compiles);
		pthread_mutex_unlock(&coder->compile_lock);
		return (1);
	}
	pthread_mutex_unlock(&coder->compile_lock);
	return (0);
}

static int	check_compiles(t_table *table)
{
	int	i;

	i = -1;
	while (++i < table->number_of_coders)
	{
		if (pthread_mutex_lock(&table->coders[i].compile_lock) != 0)
			return (0);
		if (table->coders[i].nb_compiles < table->number_of_compiles_required)
		{
			pthread_mutex_unlock(&table->coders[i].compile_lock);
			return (0);
		}
		if (pthread_mutex_unlock(&table->coders[i].compile_lock) != 0)
			return (0);
	}
	return (1);
}

void	*monitor_routine(void *arg)
{
	t_table	*table;
	int		i;

	table = (t_table *)arg;
	while (1)
	{
		i = -1;
		pthread_mutex_lock(&table->end_lock);
		while (++i < table->number_of_coders)
		{
			if (check_burnout(table, &table->coders[i]))
			{
				table->simulation_end = 1;
				pthread_mutex_unlock(&table->end_lock);
				pthread_mutex_lock(&table->print_lock);
				print_state(&table->coders[i], "burned out");
				return (pthread_mutex_unlock(&table->print_lock), NULL);
			}
		}
		if (check_compiles(table))
		{
			table->simulation_end = 1;
			return (pthread_mutex_unlock(&table->end_lock), NULL);
		}
		pthread_mutex_unlock(&table->end_lock);
		usleep(100);
	}
}
