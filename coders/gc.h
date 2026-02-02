/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sacrifist <kkraft@student42>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/26 14:06:50 by kkraft            #+#    #+#             */
/*   Updated: 2026/01/31 16:30:33 by sacrifist        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

typedef struct s_stack
{
	void			*adress;
	struct s_stack	*next;
}	t_stack;

void	*ft_malloc(size_t size);
void	ft_free(void *adress);
