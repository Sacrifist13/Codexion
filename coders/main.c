/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkraft <kkraft@student42>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/26 17:28:29 by kkraft            #+#    #+#             */
/*   Updated: 2026/02/02 09:39:39 by kkraft           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* -- DEBUG --------------------------------------- */
void	print_debug_parsing(t_table *rules)
{
	printf("✅ PARSING RÉUSSI ! Voici les données stockées :\n");
	printf("--------------------------------------------\n");
	printf("number_of_coders            : %d\n", rules->number_of_coders);
	printf("time_to_burnout             : %d ms\n", rules->time_to_burnout);
	printf("time_to_compile             : %d ms\n", rules->time_to_compile);
	printf("time_to_debug               : %d ms\n", rules->time_to_debug);
	printf("time_to_refactor            : %d ms\n", rules->time_to_refactor);
	printf("number_of_compiles_required : ");
	printf("%d\n", rules->number_of_compiles_required);
	printf("dongle_cooldown             : %d ms\n", rules->dongle_cooldown);
	if (rules->scheduler == 0)
		printf("scheduler                   : FIFO (valeur stockée: 0)\n");
	else
		printf("scheduler                   : EDF (valeur stockée: 1)\n");
	printf("--------------------------------------------\n");
}

void	print_error_msg(void)
{
	fprintf(stderr, "❌ Parsing error : Invalid inputs.\n");
	fprintf(stderr, "run: ./run <number_of_coder> <time_to_burnout> ");
	fprintf(stderr, "<time_to_compile> <time_to_debug> <time_to_refactor> ");
	fprintf(stderr, "<number_of_compiles_required> ");
	fprintf(stderr, "<dongle_cooldown scheduler>");
	return ;
}
/* -------------------------------------------------*/

int	main(int argc, char **argv)
{
	t_table		*table;

	table = init_simulation_rules(argc, argv);
	if (table == NULL)
	{
		print_error_msg();
		return (1);
	}
	//print_debug_parsing(table);
	if (!start_threads(table))
		return (1);
	if (!end_threads(table))
		return (1);
	return (0);
}
