/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/11 13:54:58 by root              #+#    #+#             */
/*   Updated: 2023/03/11 14:39:42 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

//  -- switch between echo and write ----
#define USE_ECHO_AND_CAT 0
// ----

#define ECHO_PATH "/usr/bin/echo"
#define CAT_PATH "/usr/bin/cat"

#define ECHO_MSG "From echo"
#define WRITE_MSG "From write"

#define BUFFER_SIZE 0xFF
#define ANY_CHILD_PROCESS -1
#define NO_CHILD_PROCESS_LEFT -1
#define ERROR -1

// -- util ----
size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}
// ----

// -- pid ----
static int	is_first_child(pid_t pid[2])
{
	return (pid[0] == 0);
}

static int	is_second_child(pid_t pid[2])
{
	return (pid[1] == 0);
}

static int	is_a_child(pid_t pid[2])
{
	return (is_first_child(pid) || is_second_child(pid));
}
// ----

static void	create_children(pid_t pid[2])
{
	int		i;

	i = 0;
	while (i < 2)
	{
		pid[i] = fork();
		if (pid[i] == 0)
			break ;
		i++;
	}
}

static void	first_child(pid_t pid[2], int pipe_fd[2])
{
	close(pipe_fd[0]);
	dup2(pipe_fd[1], STDOUT_FILENO);
	if (USE_ECHO_AND_CAT
		&& execve(ECHO_PATH, (char *[]){ECHO_PATH, ECHO_MSG, NULL}, NULL)
		== ERROR)
	{
		perror("execve");
		exit(EXIT_FAILURE);
	}	
	write(STDOUT_FILENO, WRITE_MSG, ft_strlen(WRITE_MSG));
}

static void	second_child(pid_t pid[2], int pipe_fd[2])
{
	char	buffer[BUFFER_SIZE] = {'\0'};
	
	close(pipe_fd[1]);
	dup2(pipe_fd[0], STDIN_FILENO);
	if (USE_ECHO_AND_CAT
		&& execve(CAT_PATH, (char *[]){NULL}, NULL)
		== ERROR)
	{
		perror("execve");
		exit(EXIT_FAILURE);
	}
	read(STDIN_FILENO, buffer, BUFFER_SIZE);
	printf("%s\n", buffer);
}

static void	main_process(int pipe_fd[2])
{
	close(pipe_fd[0]);
	close(pipe_fd[1]);
	while (waitpid(ANY_CHILD_PROCESS, NULL, 0) != NO_CHILD_PROCESS_LEFT)
		;
}

int main(void)
{
	int		pipe_fd[2];
	pid_t	pid[2];

	if	(pipe(pipe_fd) == -1)
		return (EXIT_FAILURE);
	create_children(pid);
	if (is_first_child(pid))
		first_child(pid, pipe_fd);
	else if (is_second_child(pid))
		second_child(pid, pipe_fd);
	else if (!is_a_child(pid))
		main_process(pipe_fd);
	return 0;
}

