/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkraft <kkraft@student42>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 16:20:35 by sacrifist         #+#    #+#             */
/*   Updated: 2026/02/02 11:02:03 by kkraft           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include "gc.h"

typedef struct s_request
{
	int			id;
	long long	burnout_time;
}	t_request;

typedef struct s_dongle
{
	pthread_mutex_t	lock;
	pthread_cond_t	cond;
	long long		unlock_time;
	int				is_available;
	int				id;
	t_request		queue[2];
}	t_dongle;

typedef struct s_coder
{
	pthread_mutex_t	nb_compiles_lock;
	pthread_mutex_t	last_compile_lock;
	pthread_t		thread_id;
	int				nb_compiles;
	int				id;
	long long		last_compile_time;
	t_dongle		*right;
	t_dongle		*left;
	struct s_table	*table;
}	t_coder;

typedef struct s_table
{
	pthread_mutex_t	print_lock;
	pthread_mutex_t	end_lock;
	int				number_of_coders;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				number_of_compiles_required;
	int				dongle_cooldown;
	int				scheduler;
	int				simulation_end;
	long long		start_time;
	t_coder			*coders;
	t_dongle		*dongles;
}	t_table;

/* -- PART 1 - PARSING - parsing.c -- */
t_table		*init_simulation_rules(int argc, char **argv);

/* -- PART 2 - THREADS - threads.c -- */
int			start_threads(t_table *table);
int			end_threads(t_table *table);

/* -- PART 3 - CODER'S ROUTINE - coders.c -- */
void		*coder_routine(void *arg);

/* -- PART 4 - MONITOR ROUTINE - monitor.c -- */
void		*monitor_routine(void *arg);

/* -- PART TOOLS - tools.c -- */
long long	get_time_in_ms(void);
void		ft_sleep(long long time_in_ms, t_table *table);
void		print_state(t_coder *coder, char *str);
