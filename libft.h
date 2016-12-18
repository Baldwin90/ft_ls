/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achi <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/11/01 15:47:15 by achi              #+#    #+#             */
/*   Updated: 2016/11/01 15:47:16 by achi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
# define LIBFT_H
# include <stdlib.h>
# include <string.h>
# include <stdarg.h>
# include <unistd.h>
# include <wchar.h>
# include <stdint.h>

# define TRUE	1
# define FALSE	0

/*
** FLAG PARSER
** V V V V V V
*/

/*
** TODO LIST:
**
** line 15 - change this to contain only the allowed flags for this program
** line 16 - change this to contain the characters that deliniate a valid
**			 input that starts with a '-', like negative numbers
** line 17 - change what the usage of the program is
*/

#define FLAG_LIST	"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define FLAG_EXEMPT	"0123456789"
#define USAGE		"[list of unique integers]"

typedef struct	s_flagobj
{
	long long	flags;
	int			args_start;
}				t_flagobj;

/*
** return the flag bit field
** the second function argument is actually the...
** ...index where the non-flag arguments start
*/
t_flagobj		ft_getflags(char *argv[]);

/*
** looks if the character (needle) is in the bit field (haystack).
** if the needle isn't in the FLAG_LIST or it's not in the haystack then...
** ...FALSE (0) is returned.
** note that it doesn't return any special error if it's not in FLAG_LIST...
** ...even though that means the needle couldn't possibly be in the haystack.
** TRUE (1) is returned if it is found.
*/
int				ft_hasflag(char needle, long long haystack);


/*
** GET NEXT LINE
** V V V V V V V
*/

int				get_next_line(const int fd, char **line);


/*
** PRINTF
** V V V V
*/

typedef struct	s_flags
{
	int			hashtag;
	int			zero;
	int			minus;
	int			plus;
	int			space;
	int			last_pos;
	int			width;
	int			precision;
	int			length;
	int			sign;
	uintmax_t	num;
	char		conversion;
	va_list		ap;
	int			printed_width;
}				t_flags;

# define MAX(a, b) ((a) > (b) ? (a) : (b))
# define MIN(a, b) ((a) > (b) ? (b) : (a))

int				ft_printf(const char *format, ...);

void			parse_arguments(char **pos, va_list *args, t_flags *data);
void			parse_flags(char **pos, t_flags *data);
void			parse_width(char **pos, va_list *args, t_flags *data);
void			parse_precision(char **pos, va_list *args, t_flags *data);
void			parse_length(char **pos, t_flags *data);

void			join_repeating_char(char **str, char c, int len, char is_prep);
void			join_str_target(char **target, char *dst, char *src);
char			*unicode_to_utf8(wchar_t c);
char			*get_signed_int(va_list *args, t_flags *data, int base);
char			*get_unsigned_int(va_list *args, t_flags *data, int base);

int				convert_hexa(char *str, t_flags *data, int fmt);
int				convert_octa(char *str, t_flags *data);
int				convert_int(char *str, t_flags *data);

int				print_string(va_list *args, t_flags *data);
int				print_char(va_list *args, t_flags *data);
int				print_invalid_converter(t_flags *data);
void			clear_string(char **str);

typedef struct	s_l
{
	void		*content;
	size_t		content_size;
	struct s_l	*next;
}				t_list;


/*
** LIBFT
** V V V
*/

int				ft_toupper(int c);
int				ft_tolower(int c);
char			*ft_strtrim(char const *s);
char			*ft_strsub(char const *s, unsigned int start, size_t len);
char			*ft_strsub(char const *s, unsigned int start, size_t len);
char			*ft_strstr(const char *big, const char *little);
char			**ft_strsplit(char const *s, char c);
char			*ft_strrchr(const char *s, int c);
char			*ft_strnstr(const char *big, const char *little, size_t n);
char			*ft_strnew(size_t size);
int				ft_strnequ(char const *s1, char const *s2, size_t n);
char			*ft_strncpy(char *dst, const char *src, size_t len);
int				ft_strncmp(const char *s1, const char *s2, size_t n);
char			*ft_strncat(char *s1, const char *s2, size_t n);
char			*ft_strmapi(char const *s, char (*f)(unsigned int, char));
char			*ft_strmap(char const *s, char (*f)(char));
size_t			ft_strlen(const char *s);
size_t			ft_strlcat(char *dst, const char *src, size_t size);
char			*ft_strjoin(char const *s1, const char *s2);
void			ft_striteri(char *s, void (*f)(unsigned int, char *));
void			ft_striter(char *s, void (*f)(char *));
int				ft_strequ(char const *s1, char const *s2);
char			*ft_strdup(const char *s);
void			ft_strdel(char **as);
char			*ft_strcpy(char *dst, const char *src);
int				ft_strcmp(const char *s1, const char *s2);
void			ft_strclr(char *s);
char			*ft_strchr(const char *s, int c);
char			*ft_strcat(char *s1, const char *s2);
void			ft_putstr(char const *s);
void			ft_putstr_fd(char const *s, int fd);
void			ft_putnbr(int n);
void			ft_putnbr_fd(int n, int fd);
void			ft_putendl(char const *s);
void			ft_putendl_fd(char const *s, int fd);
void			ft_putchar(char c);
void			ft_putchar_fd(char c, int fd);
void			*ft_memset(void *b, int c, size_t len);
void			*ft_memmove(void *dst, const void *src, size_t len);
void			ft_memdel(void **ap);
char			*ft_memcpy(void *dst, const void *src, size_t n);
int				ft_memcmp(const void *s1, const void *s2, size_t n);
void			*ft_memchr(const void *s, int c, size_t n);
void			*ft_memccpy(void *dst, const void *src, int c, size_t n);
void			*ft_memalloc(size_t size);
char			*ft_itoa(int n);
int				ft_isprint(int c);
int				ft_isdigit(int c);
int				ft_isascii(int c);
int				ft_isalpha(int c);
int				ft_isalnum(int c);
int				ft_isnumeric(char *str);
void			ft_bzero(void *s, size_t n);
int				ft_atoi(const char *nptr);
void			ft_lstadd(t_list **alst, t_list *new);
void			ft_lstdel(t_list **alst, void (*del)(void *, size_t));
void			ft_lstdelone(t_list **alst, void (*del)(void *, size_t));
void			ft_lstiter(t_list *lst, void (*f)(t_list *elem));
t_list			*ft_lstmap(t_list *lst, t_list *(*f)(t_list *elem));
t_list			*ft_lstnew(void const *content, size_t content_size);
double			ft_sqrt(double n);
int				ft_ceil(double n);
char			*ft_strndup(const char *s1, size_t n);
int				ft_strcontains(char const *s, char c);
size_t			ft_strclen(const char *s, char c);
size_t			ft_strsplice(char **head, size_t tgt, size_t amt, char *src);
void			*ft_memjoin(void const *s1, void const *s2, int dlen, int slen);
char			*ft_llutoa_base(unsigned long long n, int base);
void			ft_strupcase(char *str);
void			*ft_realloc(void *old_ptr, size_t old_size, size_t new_size);
int				ft_char_index(char c, char *str);
#endif
