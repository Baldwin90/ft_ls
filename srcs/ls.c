#include <dirent.h>
#include <stdio.h>

#include <errno.h>
#include <sys/stat.h>
#include <string.h>

#include "libft.h"
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <limits.h>
#include <sys/xattr.h>

void	print_dir(char *target, t_flagobj *flagobj, int print_name);

char *prog_name;

typedef struct	s_filedata {
	char		*filename;
	char		*full_path;
	char		*owner;
	char		*group;
	char		*links;
	char		*size;
	char		*time_str;
	struct stat	*st_buf;
}				t_filedata;

// Joins multiple strings together.
// Parameter int n specifies number of strings to join.
char	*variable_join(int n, ...) {
	va_list	va;
	int		len;
	char	*ret;
	int		n_cache;
	char	*cpy;

	va_start(va, n);
	len = 0;
	n_cache = n;
	while (n_cache--)
		len += ft_strlen(va_arg(va, char *));
	va_end(va);
	va_start(va, n);
	if ((ret = ft_strnew(len) - 1))
		while (n--) {
			cpy = va_arg(va, char *) - 1;
			while (*(++cpy))
				*(++ret) = *cpy;
		}
	return (ret - len + 1);
}

// Count the number of files in a directory
int		count_files(char *target, int count_hidden) {
	DIR				*d;
	struct dirent	*dir;
	int				count;

	count = 0;
	d = opendir(target);
	if (d)
		while ((dir = readdir(d)) != NULL)
			if (dir->d_name[0] != '.' || count_hidden)
				++count;
	closedir(d);
	return (count);
}

// Counts the total blocksize
long long	count_total_blocksize(t_filedata **tab, int size) {
	long long	blocksize;
	int	i;

	i = -1;
	blocksize = 0;
	while (++i < size) {
		blocksize += tab[i]->st_buf->st_blocks;
	}
	return (blocksize);
}

// Sorts the array based on name
void	sort_array_name(t_filedata **tab, int size) {
	int		i;
	int		is_sorted;
	t_filedata	*cache;

	is_sorted = 1;
	while (is_sorted) {
		is_sorted = 0;
		i = 0;
		while (i < size - 1) {
			if (ft_strcmp(tab[i]->filename, tab[i + 1]->filename) > 0) {
				cache = tab[i];
				tab[i] = tab[i + 1];
				tab[i + 1] = cache;
				is_sorted = 1;
			}
			i++;
		}
	}
}

// Sort the input based on name
void	sort_input_name(char **tab, int size) {
	int		i;
	int		is_sorted;
	char	*cache;

	is_sorted = 1;
	while (is_sorted) {
		is_sorted = 0;
		i = 0;
		while (i < size - 1) {
			if (ft_strcmp(tab[i], tab[i + 1]) > 0) {
				cache = tab[i];
				tab[i] = tab[i + 1];
				tab[i + 1] = cache;
				is_sorted = 1;
			}
			i++;
		}
	}
}

// Sort the array based on time
// If the seconds match, sort by nanoseconds
void	sort_array_time(t_filedata **tab, int size) {
	int		i;
	int		is_sorted;
	t_filedata	*cache;

	is_sorted = 1;
	while (is_sorted) {
		is_sorted = 0;
		i = 0;
		while (i < size - 1) {
			// Sort by nanoseconds if the time matches
			if (tab[i]->st_buf->st_mtime < tab[i + 1]->st_buf->st_mtime || (tab[i]->st_buf->st_mtime == tab[i + 1]->st_buf->st_mtime && tab[i]->st_buf->st_mtimespec.tv_nsec < tab[i + 1]->st_buf->st_mtimespec.tv_nsec)) {
				cache = tab[i];
				tab[i] = tab[i + 1];
				tab[i + 1] = cache;
				is_sorted = 1;
			}
			i++;
		}
	}
}

// Reverses the array
void	reverse_array(t_filedata **tab, int size) {
	t_filedata	*cache;
	int		i;

	i = -1;
	while (++i < size / 2) {
		cache = tab[i];
		tab[i] = tab[size - i - 1];
		tab[size - i - 1] = cache;
	}
}

// New array
t_filedata	**new_file_array(int size) {
	t_filedata	**ret;

	if (!(ret = (t_filedata **)ft_memalloc(sizeof(*ret) * size))) {
		perror(prog_name);
		exit(1);
	}
	return (ret);
}

// Creates a string of size spaces
char	*blank_padding(int size) {
	char	*ret;

	ret = (ft_strnew(size));
	ft_memset(ret, ' ', size);
	return (ret);
}

// Does some voodo shit that sets major minor stuff
void	set_major_minor_size(t_filedata *ret) {
	char	*cache1;
	char	*cache2;
	char	*cache3;
	char	*cache4;

	cache1 = ft_llutoa_base(major(ret->st_buf->st_rdev), 10);
	cache2 = blank_padding(3 - ft_strlen(cache1));
	cache3 = ft_llutoa_base(minor(ret->st_buf->st_rdev), 10);
	cache4 = blank_padding(3 - ft_strlen(cache3));
	ret->size = variable_join(5, cache2, cache1, ", ", cache4, cache3);
	free(cache1);
	free(cache2);
	free(cache3);
	free(cache4);
}

// Creates a new file struct
t_filedata	*new_file(char *target, char *filename) {
	t_filedata	*ret;
	char	*cache;

	if (!(ret = (t_filedata *)ft_memalloc(sizeof(*ret)))) {
		perror(prog_name);
		exit(1);
	}
	ret->filename = ft_strdup(filename);
	// Absolute path
	ret->full_path = variable_join(3, target, "/", filename);
	if (!(ret->st_buf = (struct stat *)ft_memalloc(sizeof(*(ret->st_buf))))) {
		free(ret);
		perror(prog_name);
		exit(1);
	}
	lstat(ret->full_path, ret->st_buf);
	ret->owner = getpwuid(ret->st_buf->st_uid)->pw_name;
	ret->group = getgrgid(ret->st_buf->st_gid)->gr_name;
	ret->links = ft_llutoa_base(ret->st_buf->st_nlink, 10);
	if (S_ISBLK(ret->st_buf->st_mode) || S_ISCHR(ret->st_buf->st_mode)) {
		set_major_minor_size(ret);
	} else {
		ret->size = ft_llutoa_base(ret->st_buf->st_size, 10);
	}
	// If the time is one hour in advance of if it is older than 6 months
	if (time(0) - ret->st_buf->st_mtime > 15724800 || time(0) < ret->st_buf->st_mtime - 3600) {
		// If the time (of a future time) exceeds a certain length, do some voodoo shit
		// Else, do some other voodoo shit
		if (ft_strlen(ctime(&(ret->st_buf->st_mtime))) > 25) {
			cache = ft_strdup(ctime(&(ret->st_buf->st_mtime)) + 4);
			cache[ft_strlen(cache) - 15 - (ft_strlen(ctime(&(ret->st_buf->st_mtime))) - 25)] = 0;
			ret->time_str = variable_join(3, cache, " ", ctime(&(ret->st_buf->st_mtime)) + 23);
			ret->time_str[ft_strlen(ret->time_str) - 1] = 0;
			free(cache);
		} else {
			cache = ft_strdup(ctime(&(ret->st_buf->st_mtime)) + 4);
			cache[ft_strlen(cache) - 15] = 0;
			ret->time_str = variable_join(3, cache, " ", ctime(&(ret->st_buf->st_mtime)) + 19);
			ret->time_str[ft_strlen(ret->time_str) - 1] = 0;
			free(cache);
		}
	} else {
		ret->time_str = ft_strdup(ctime(&(ret->st_buf->st_mtime)) + 4);
		ret->time_str[ft_strlen(ret->time_str) - 9] = 0;
	}
	return (ret);
}

void	free_file_array(t_filedata **tab, int size) {
	if (size) {
		while (size--) {
			free(tab[size]->filename);
			free(tab[size]->full_path);
			free(tab[size]->links);
			free(tab[size]->size);
			free(tab[size]->st_buf);
			free(tab[size]->time_str);
			free(tab[size]);
		}
	}
	free(tab);
}

int		get_min_wid(t_filedata **tab, int size, int type) {
	int	ret;
	int	cache;
	ret = 0;
	while (size--) {
		if (type == 0)
			cache = ft_strlen(tab[size]->links); else if (type == 1)
			cache = ft_strlen(tab[size]->owner); else if (type == 2)
			cache = ft_strlen(tab[size]->group); else if (type == 3)
			cache = ft_strlen(tab[size]->size);
		if (cache > ret)
			ret = cache;
	}
	return (ret);
}

int		get_slash_idx(char *bleach_for_you_to_fucking_drink) {
	int	fucking_kill_yourself;
	int	i;

	i = 0;
	fucking_kill_yourself = 0;
	bleach_for_you_to_fucking_drink--;
	while (*(++bleach_for_you_to_fucking_drink)) {
		++i;
		if (*bleach_for_you_to_fucking_drink == '/')
			fucking_kill_yourself = i;
	}
	return (fucking_kill_yourself);
}

void	print_filedata_array(t_filedata **file_array, int file_count, t_flagobj *flagobj, int print_name) {
	int	i;

	sort_array_name(file_array, file_count); /*Always sort by name, as if two files have the same time changed, it must be lexographicly sorted*/
	if (ft_hasflag('t', flagobj->flags)) {
		sort_array_time(file_array, file_count);
	}
	if (ft_hasflag('r', flagobj->flags)) {
		reverse_array(file_array, file_count);
	}
	i = -1;
	if (ft_hasflag('l', flagobj->flags)) {
		while (++i < file_count) {
			char *file_type = ft_strnew(11) - 1;
			if (S_ISLNK(file_array[i]->st_buf->st_mode)) {
				*(++file_type) = 'l';
			} else if (S_ISDIR(file_array[i]->st_buf->st_mode)) {
				*(++file_type) = 'd';
			} else if (S_ISSOCK(file_array[i]->st_buf->st_mode)) {
				*(++file_type) = 's';
			} else if (S_ISBLK(file_array[i]->st_buf->st_mode)) {
				*(++file_type) = 'b';
			} else if (S_ISCHR(file_array[i]->st_buf->st_mode)) {
				*(++file_type) = 'c';
			} else if (S_ISFIFO(file_array[i]->st_buf->st_mode)) {
				*(++file_type) = 'p';
			} else if (S_ISREG(file_array[i]->st_buf->st_mode)) {
				*(++file_type) = '-';
			} else {
				*(++file_type) = ' ';
			}
			*(++file_type) = (file_array[i]->st_buf->st_mode & S_IRUSR) ? 'r' : '-';
			*(++file_type) = (file_array[i]->st_buf->st_mode & S_IWUSR) ? 'w' : '-';
			if (file_array[i]->st_buf->st_mode & S_ISUID) {
				*(++file_type) = (file_array[i]->st_buf->st_mode & S_IXUSR) ? 's' : 'S';
			} else {
				*(++file_type) = (file_array[i]->st_buf->st_mode & S_IXUSR) ? 'x' : '-';
			}
			*(++file_type) = (file_array[i]->st_buf->st_mode & S_IRGRP) ? 'r' : '-';
			*(++file_type) = (file_array[i]->st_buf->st_mode & S_IWGRP) ? 'w' : '-';
			if (file_array[i]->st_buf->st_mode & S_ISGID) {
				*(++file_type) = (file_array[i]->st_buf->st_mode & S_IXGRP) ? 's' : 'S';
			} else {
				*(++file_type) = (file_array[i]->st_buf->st_mode & S_IXGRP) ? 'x' : '-';
			}
			*(++file_type) = (file_array[i]->st_buf->st_mode & S_IROTH) ? 'r' : '-';
			*(++file_type) = (file_array[i]->st_buf->st_mode & S_IWOTH) ? 'w' : '-';
			if (file_array[i]->st_buf->st_mode & S_ISVTX) {
				*(++file_type) = (file_array[i]->st_buf->st_mode & S_IXOTH) ? 't' : 'T';
			} else {
				*(++file_type) = (file_array[i]->st_buf->st_mode & S_IXOTH) ? 'x' : '-';
			}
			*(++file_type) = (listxattr(file_array[i]->full_path, 0, 0 , 0) > 0) ? '@' : ' ';
			ft_printf("%s %*s %-*s  %-*s %*s %s %s", file_type - 10, get_min_wid(file_array, file_count, 0), file_array[i]->links, get_min_wid(file_array, file_count, 1), file_array[i]->owner, get_min_wid(file_array, file_count, 2), file_array[i]->group, get_min_wid(file_array, file_count, 3) + 1, file_array[i]->size, file_array[i]->time_str, file_array[i]->filename);
			if (S_ISLNK(file_array[i]->st_buf->st_mode)) {
				char link_path[PATH_MAX];
				for (int x = 0; x < PATH_MAX; x++) {
					link_path[x] = 0;
				}
				readlink(file_array[i]->full_path, link_path, PATH_MAX);
				ft_printf(" -> %s", link_path);
			}
			ft_putstr("\n");
			free (file_type - 10);
		}
	} else {
		while (++i < file_count) {
			ft_printf("%s\n", file_array[i]->filename);
		}
	}
	i = -1;
	while (++i < file_count) {
		if (ft_hasflag('R', flagobj->flags) && S_ISDIR(file_array[i]->st_buf->st_mode) && ft_strcmp("..", file_array[i]->filename) && ft_strcmp(".", file_array[i]->filename)) {
			ft_printf("\n%s:\n", file_array[i]->full_path);
			print_dir(file_array[i]->full_path, flagobj, print_name);
		}
	}
}

void	print_dir(char *target, t_flagobj *flagobj, int print_name) {
	DIR				*d;
	struct dirent	*dir;
	char			*cache;
	t_filedata			**file_array;
	int				file_count;
	int				i;

	if (print_name) {
		ft_printf("%s:\n", target);
	}
	d = opendir(target);
	if (!d && errno) {
		cache = variable_join(3, prog_name, ": ", target);
		perror(cache);
		free(cache);
		return ;
	}
	file_count = count_files(target, ft_hasflag('a', flagobj->flags));
	file_array = new_file_array(file_count);
	i = -1;
	while ((dir = readdir(d)) != NULL) {
		if (dir->d_name[0] != '.' || ft_hasflag('a', flagobj->flags)) {
			file_array[++i] = new_file(target, dir->d_name);
		}
	}
	closedir(d);
	if (ft_hasflag('l', flagobj->flags)) {
		if (file_count) {
			ft_printf("total %lli\n", count_total_blocksize(file_array, file_count));
		}
	}
	print_filedata_array(file_array, file_count, flagobj, print_name);
	free_file_array(file_array, file_count);
}

int		main(int a, char *b[]) {
	int			i;
	char		*cache;
	t_flagobj	flagobj;
	t_filedata		**file_array;
	int			file_count;
	int			f_count;
	int			print_name;
	struct stat	st_buf;

	prog_name = b[0];
	flagobj = ft_getflags(b);
	if (a == flagobj.args_start) {
		print_dir(".", &flagobj, 0);
	} else {
		sort_input_name(&(b[flagobj.args_start]), a - flagobj.args_start);
		file_count = 0;
		f_count = 0;
		file_array = new_file_array(a - flagobj.args_start);
		i = flagobj.args_start - 1;
		while (++i < a) {
			if (!ft_strlen(b[i])) {
				cache = variable_join(2, prog_name + get_slash_idx(prog_name), ": fts_open: No such file or directory\n");
				ft_putstr_fd(cache, 2);
				free(cache);
				free_file_array(file_array, file_count);
				return (1);
			}
			errno = 0;
			++f_count;
			if (ft_hasflag('l', flagobj.flags)) {
				lstat(b[i], &st_buf);
			} else {
				stat(b[i], &st_buf);
				if (errno) {
					errno = 0;
					lstat(b[i], &st_buf);
				}
			}
			if (errno) {
				cache = variable_join(3, prog_name + get_slash_idx(prog_name), ": ", b[i]);
				perror(cache);
				free(cache);
				continue ;
			} else if (!S_ISDIR(st_buf.st_mode)) {
				if (*b[i] == '/') {
					file_array[file_count++] = new_file("", b[i]);
				} else {
					file_array[file_count++] = new_file(".", b[i]);
				}
			} else {
				--f_count;
			}
		}
		if (file_count) {
			print_filedata_array(file_array, file_count, &flagobj, 0);
			if ((a - flagobj.args_start) - f_count) {
				ft_printf("\n");
			}
		}
		print_name = (file_count || (a - flagobj.args_start) - file_count > 1) ? 1 : 0;
		free_file_array(file_array, file_count);
		i = flagobj.args_start - 1;
		while (++i < a) {
			errno = 0;
			if (ft_hasflag('l', flagobj.flags)) {
				lstat(b[i], &st_buf);
			} else {
				stat(b[i], &st_buf);
			}
			if (errno) {
				continue;
			}
			if (S_ISDIR(st_buf.st_mode)) {
				print_dir(b[i], &flagobj, print_name);
				if ((a - flagobj.args_start) - ++f_count) {
					ft_printf("\n");
				}
			}
		}
	}
	return (0);
}
