/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sacrifist <kkraft@student42>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/26 16:42:54 by kkraft            #+#    #+#             */
/*   Updated: 2026/02/01 10:20:09 by sacrifist        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_valid_positive_integer(char *str)
{
	int			int_max;
	int			i;
	long int	n;

	int_max = 2147483647;
	n = 0;
	i = 0;
	if (str[0] == '+')
		i++;
	while (str[i])
	{
		if (!(str[i] >= '0' && str[i] <= '9'))
			return (0);
		else
		{
			n = n * 10 + (str[i] - '0');
			if (n > int_max)
				return (0);
		}
		i++;
	}
	if (n == 0)
		return (0);
	return (1);
}

static int	validate_input_syntax(int argc, char **argv)
{
	int	i;

	if (argc != 9)
		return (0);
	if (strcmp(argv[8], "fifo") && strcmp(argv[8], "edf"))
		return (0);
	i = 1;
	while (i < 8)
	{
		if (!is_valid_positive_integer(argv[i++]))
			return (0);
	}
	return (1);
}

t_table	*init_simulation_rules(int argc, char **argv)
{
	t_table	*table;

	if (!validate_input_syntax(argc, argv))
		return (NULL);
	table = ft_malloc(sizeof(t_table));
	if (table == NULL)
		return (NULL);
	table->number_of_coders = atoi(argv[1]);
	table->time_to_burnout = atoi(argv[2]);
	table->time_to_compile = atoi(argv[3]);
	table->time_to_debug = atoi(argv[4]);
	table->time_to_refactor = atoi(argv[5]);
	table->number_of_compiles_required = atoi(argv[6]);
	table->dongle_cooldown = atoi(argv[7]);
	table->scheduler = 1;
	if (!strcmp(argv[8], "fifo"))
		table->scheduler = 0;
	table->simulation_end = 0;
	return (table);
}
