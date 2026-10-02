#ifndef _UTILS_H_
#define _UTILS_H_

typedef struct {
  int  len;	/* # of elements in the array */
  int  maxlen;	/* Length of array */
  int *list;	/* Pointer to the array */
} int_set;

typedef struct _hash_table {
  char *key;
  int val;
  struct _hash_table *next;
} HASH_TABLE;

int     int2idx(int, int);
int     idx2int(int, int);
int     idtxt2int(char *, int);
int 	hashfun(char *);
void	ht_add(HASH_TABLE[], char *, int val);
int 	ht_lookup(HASH_TABLE[], char *, int, int, int (eqfun) (int, int));
void    ht_remove(HASH_TABLE[], char *, int);
int	addordel(char file_name[80], char check[80]);
int	addname(char file_name[80], char name[80]);
int	delname(char file_name[80], char name[80]);
char    *strcasestr(char *, char *);
Boolean valid_fname(char *name);
void    read_line( char *target, int len, FILE *file );
int     makesock(char *, int);
void	init_rand(void);
char	*strip_char (char *string, char *data, char chk);
Boolean check_for_possible_resize (int_set * p);
int	randperc(void);
Boolean	match(char *p, const char *q);
Boolean	infile(char *file, const char *line);
int     fnumlines(char file[200]);
void    fileseek(FILE *file, int lines);

#ifdef SYS_EQBUG
int strcasecmp(char *s1, char *s2);
int strncasecmp(const char *s1, const char *s2, size_t n);
#endif

#define TABLE_END ((char *)(-1))
#define INT_LEN  10             /* maximum digits in a loc/obj/mob number */

Boolean int_is_on_table( int *table, int object );
int	glookup(char *elem,int n,char **table,
		int (*strcmpfun)(const char *s1, const char *s2, size_t n));
char	*lowercase(char *str);
char	*uppercase(char *str);
void	*xmalloc(int nelem, int elem_size);
int     xstrcasecmp (const char *s1, const char *s2, size_t n);
void	insertch(char *s, char ch, int i);
char	*my_crypt(char *buf,char *pw);

void *resize_array(void *start, int elem_size, int oldlen, int newlen);

#define SET_END -65536

/* int_set interface */
void	init_intset(int_set *p, int len);
void	free_intset(int_set *p);
Boolean	add_int(int n, int_set *p);
Boolean	remove_int(int n, int_set *p);
int	find_int(int n, int_set *p);
int	foreach_int(int_set *p, int (*func)(int));

#define first_int(P) ((P)->len == 0 ? SET_END : (*(P)->list))
#define int_number(I, P) ((P)->list[I])
#define set_size(P) ((P)->len)
#define is_empty(P) (set_size(P) == 0)

/* int_table interface */
typedef struct TABLE_ENTRY {
  long int key;
  long int value;
  struct TABLE_ENTRY *next;
} table_entry;

typedef struct {
  int  len;
  table_entry **table;
} int_table;

#define NOT_IN_TABLE -65536L
#define BASE_SIZE 1000       /* base size for hash tables (doesn't grow) */

void	init_inttable(int_table *p, int size);
void	free_inttable(int_table *p);

Boolean	insert_entry(long int key, long int value, int_table *p);
Boolean	remove_entry(long int key, int_table *p);
long	lookup_entry(long int key, int_table *p);
long	change_entry(long int key, long int new_value, int_table *p);

#ifdef DEBUG_MEM
  #define FREE(x) x ? free(x) : ;
#else
  #define FREE(x) free(x)
#endif

#endif
