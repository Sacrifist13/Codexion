/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkraft <kkraft@student42>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 16:30:10 by sacrifist         #+#    #+#             */
/*   Updated: 2026/02/02 17:50:18 by kkraft           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* -- Monitor - Check Coders Burnout and Coders Compilation Times -- */
/* -- Stop the simulation if Burnout or if Compilation
Times Done for all coders -- */

static int	check_burnout(t_table *table, t_coder *coder)
{
	long long last_compile_time;
	
	pthread_mutex_lock(&coder->last_compile_lock);
	last_compile_time = coder->last_compile_time;
	pthread_mutex_unlock(&coder->last_compile_lock);
	if (last_compile_time + table->time_to_burnout <= get_time_in_ms())
		return (1);
	return (0);
}

static int	check_compiles(t_table *table)
{
	int	i;
	int	nb_compiles;

	i = -1;
	while (++i < table->number_of_coders)
	{
		pthread_mutex_lock(&table->coders[i].nb_compiles_lock);
		nb_compiles = table->coders[i].nb_compiles;
		pthread_mutex_unlock(&table->coders[i].nb_compiles_lock);

		if (nb_compiles < table->number_of_compiles_required)
			return (0);
	}
	pthread_mutex_lock(&table->end_lock);
	table->simulation_end = 1;
	pthread_mutex_unlock(&table->end_lock);
	return (1);
}

void	*monitor_routine(void *arg)
{
	t_table	*table;
	int		i;
	int		simulation_end;

	table = (t_table *)arg;
	while (1)
	{
		i = -1;
		pthread_mutex_lock(&table->end_lock);
		simulation_end = table->simulation_end;
		pthread_mutex_unlock(&table->end_lock);
		if (simulation_end)
			return (NULL);
		while (++i < table->number_of_coders)
		{
			if (check_burnout(table, &table->coders[i]))
			{
				pthread_mutex_lock(&table->end_lock);
				table->simulation_end = 1;
				pthread_mutex_unlock(&table->end_lock);
				print_state(&table->coders[i], "burned out");
				return ( NULL);
			}
		}
		if (check_compiles(table))
			return (NULL);
	}
}
