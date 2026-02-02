/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkraft <kkraft@student42>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 16:34:06 by sacrifist         #+#    #+#             */
/*   Updated: 2026/02/02 07:48:10 by kkraft           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_time_in_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void	ft_sleep(long long time_in_ms, t_table *table)
{
	long long	sleep;

	sleep = get_time_in_ms() + time_in_ms;
	while (get_time_in_ms() < sleep)
	{
		if (pthread_mutex_lock(&table->end_lock) != 0)
			return ;
		if (table->simulation_end)
		{
			pthread_mutex_unlock(&table->end_lock);
			return ;
		}
		if (pthread_mutex_unlock(&table->end_lock) != 0)
			return ;
		usleep(100);
	}
	return ;
}

void	print_state(t_coder *coder, char *str)
{
	long long	time;

	time = get_time_in_ms() - coder->table->start_time;
	printf("%lli %d %s\n", time, coder->id, str);
}
