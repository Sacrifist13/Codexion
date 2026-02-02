/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkraft <kkraft@student42>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/26 13:33:29 by kkraft            #+#    #+#             */
/*   Updated: 2026/01/26 14:07:24 by kkraft           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gc.h"

static t_stack	**get_collector(void)
{
	static t_stack	*head;

	return (&head);
}

void	*ft_malloc(size_t size)
{
	t_stack	**head;
	t_stack	*new_node;
	void	*new_element;

	head = get_collector();
	new_element = malloc(size);
	if (new_element == NULL)
		return (NULL);
	new_node = malloc(sizeof(t_stack));
	if (!new_node)
	{
		free(new_element);
		return (NULL);
	}
	new_node->adress = new_element;
	new_node->next = *head;
	*head = new_node;
	return (new_element);
}

void	ft_free(void *adress)
{
	t_stack	**head;
	t_stack	*tmp;
	t_stack	*prev_tmp;

	head = get_collector();
	if (!head)
		return ;
	tmp = *head;
	prev_tmp = NULL;
	while (tmp != NULL)
	{
		if (tmp->adress == adress)
		{
			if (!prev_tmp)
				*head = (*head)->next;
			else
				prev_tmp->next = tmp->next;
			free(tmp->adress);
			free(tmp);
			return ;
		}
		prev_tmp = tmp;
		tmp = tmp->next;
	}
	return ;
}

__attribute__((destructor)) static void	cleaning(void)
{
	t_stack	**head;
	t_stack	*tmp;

	head = get_collector();
	while (*head != NULL)
	{
		tmp = *head;
		*head = (*head)->next;
		free(tmp->adress);
		free(tmp);
	}
	return ;
}
