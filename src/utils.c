#ifndef _UTILS_H
#define _UTILS_H

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#ifndef _OLD_LINUX_
  #include <crypt.h>
#endif
#include "kernel.h"
#include "utils.h"
#include "errno.h"
#include "log.h"

int makesock(char *addr, int port) {
  int s /*, opt */;
  struct sockaddr_in client;
  
  client.sin_family = AF_INET;
  client.sin_port = htons(port);
  client.sin_addr.s_addr = inet_addr(addr);

  if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    return(-1);
  fcntl(s, F_SETFL, O_NONBLOCK);

 /* opt = 1;
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)); */

  if (connect(s, (struct sockaddr *) &client, sizeof(client)) == -1)
    if (errno != EINPROGRESS) {
      close(s);
      return(-1);
    }
  return(s);
}

void read_line(char *target, int len, FILE *file) {
  int i;

  fgets( target, len, file );
  for( i = 0; i < len && target[i] != '\n'; i++ )
	;
  target[i] = '\0';
  return;
}

char *strcasestr (char *s1, char *s2) {
  static char n1[MAX_COM_LEN * 2], n2[MAX_COM_LEN * 2];
  int j;

  for (j = 0 ; s1[j] != '\0' ; j++)
    n1[j] = tolower (s1[j]);
  n1[j] = '\0';

  for (j = 0 ; s2[j] != '\0' ; j++)
    n2[j] = tolower (s2[j]);
  n2[j] = '\0';

  return (strstr (n1, n2));
}

int fnumlines (char file[200]) {
  FILE *fp;
  char tmp[300];
  int num = 0;

  if ((fp = fopen(file, "rt")) == NULL)
    return -1;

  while (fgets (tmp, sizeof (tmp), fp))
    num++;
  fclose (fp);
  return num;
}

void fileseek (FILE * file, int lines) {
  char tmp[300];
  int loc;

  fseek (file, 0L, SEEK_SET);
  for (loc = 0; loc < lines; loc++)
    fgets (tmp, sizeof (tmp), file);
}

/* is this a valid filename? (letters only, no ..'s to change path) */

Boolean valid_fname(char *name) {
  char *ptr;

  if (!(*name))
    return(False);

  for (ptr = name ; isalpha(*ptr) ; ptr++);
  if (!(*ptr))
    return(True);
  else
    return (False);
}

void add_tree(T_elemptr *tree, char *s, int num) {
  T_elemptr newptr, ptr;

  newptr = (T_elemptr) NEW(T_elem, 1);
  newptr->num = num;
  newptr->str = (char *) NEW(char, (strlen(s) + 1));
  strcpy(newptr->str, s);
  newptr->right = newptr->left = NULL;

  if (*tree == NULL)                             /* make root node */
    *tree = newptr;
  else {
    ptr = *tree;
    while (1) {
      if (strcmp(s, ptr->str) < 0) {
	if (ptr->left == NULL) {              /* a left leaf on the tree */
            ptr->left = newptr;
            break;
	}
          else
            ptr = ptr->left;
      }
      else if (strcmp(s, ptr->str) > 0) {
	if (ptr->right == NULL) {            /* a right leaf on the tree */
            ptr->right = newptr;
            break;
	}
          else
            ptr = ptr->right;
      }
        else                                   /* node already exists */
          return;
    }
  }
}

int tree_lookup(T_elemptr tree, char *s) {
  T_elemptr ptr = tree;

  while (ptr != NULL) {
    if (strncmp(s, ptr->str, strlen(s)) < 0)
      ptr = ptr->left;
    else if (strncmp(s, ptr->str, strlen(s)) > 0)
      ptr = ptr->right;
    else                                      /* ptr == s */
      return ptr->num;
  }
  return -1;
}


Boolean int_is_on_table(int *table, int object) {
  int i;

  for (i = 0; table[i] != -1; i++) 
    if (table[i] == object)
      return True;

  return False;
}

/* matches wildcards on p in the form of <*blah> and <blah*> */

Boolean match (char *a, const char *b) {
  char *ptr, *p, *q;
  size_t len;
  Boolean result = False;

  p = strdup(a);                              /* ignore case */
  q = strdup(b);
  if (!p || !q) { free(p); free(q); return False; }
  for (ptr = p ; *ptr ; ptr++)
    *ptr = tolower((unsigned char)*ptr);
  for (ptr = q ; *ptr ; ptr++)
    *ptr = tolower((unsigned char)*ptr);

  len = strlen(p);
  if (len && *(p + len - 1) == '\n') {        /* remove newline */
    *(p + len - 1) = 0;
    len--;
  }
  if (*p == '*') {                            /* (*blah) */
    ptr = strstr(q, p + 1);
    if (ptr && strlen(ptr) == len - 1)        /* lengths match */
      result = True;
  }
  else if (len && *(p + len - 1) == '*') {     /* (blah*) */
    *(p + len) = 0;
    if (!strncmp(q, p, len - 1))
      result = True;
  }
  else if (!strcmp(q, p))
    result = True;
  free(p); free(q);
  return result;
}

#ifdef SYS_EQBUG
/*
 * Copyright (c) 1987 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that this notice is preserved and that due credit is given
 * to the University of California at Berkeley. The name of the University
 * may not be used to endorse or promote products derived from this
 * software without specific written prior permission. This software
 * is provided ``as is'' without express or implied warranty.
 */

/*
 * This array is designed for mapping upper and lower case letter
 * together for a case independent comparison.  The mappings are
 * based upon ascii character sequences.
 */
static u_char charmap[] =
{
  '\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
  '\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
  '\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
  '\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
  '\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
  '\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
  '\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
  '\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
  '\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
  '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
  '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
  '\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137',
  '\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
  '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
  '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
  '\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177',
  '\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
  '\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
  '\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
  '\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
  '\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
  '\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
  '\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
  '\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
  '\300', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
  '\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
  '\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
  '\370', '\371', '\372', '\333', '\334', '\335', '\336', '\337',
  '\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
  '\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
  '\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
  '\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377',
};

int
strcasecmp (char *s1, char *s2)
{
  register u_char *cm = charmap, *us1 = (u_char *) s1, *us2 = (u_char *) s2;

  while (cm[*us1] == cm[*us2++])
    if (*us1++ == '\0')
      return (0);
  return (cm[*us1] - cm[*--us2]);
}

int
strncasecmp (const char *s1, const char *s2, register size_t n)
{
  register u_char *cm = charmap, *us1 = (u_char *) s1, *us2 = (u_char *) s2;

  while (--n >= 0 && cm[*us1] == cm[*us2++])
    if (*us1++ == '\0')
      return (0);
  return (n < 0 ? 0 : cm[*us1] - cm[*--us2]);
}

/*
 * **  END OF BERKELEY INCLUDE
 */
#endif


int glookup (char *elem, int n, char **table,
	     int (*strcmpfun) (const char *s1, const char *s2, size_t n)) {
  char **t;
  int x;

  for (t = table, x = 0; *t != TABLE_END; ++t, ++x) {
    if (*t == NULL)
      continue;
    if (strcmpfun (elem, *t, n) == 0)
      return x;
  }
  return (-1);
}

int xstrcasecmp (const char *s1, const char *s2, size_t n) {
  return strcasecmp (s1, s2);
}

char *lowercase (char *str) {
  char *p;

  for (p = str; *p; p++)
    if (isupper(*p))
      *p = tolower(*p);
  return str;
}

char *uppercase (char *str) {
  char *p;

  for (p = str; *p; p++)
    if (islower (*p))
      *p = toupper (*p);
  return str;
}

void *xmalloc (int nelem, int elem_size) {
  if (nelem < 0 || elem_size <= 0) memory_failure();
  return memory_alloc((size_t)nelem, (size_t)elem_size);
}

/* Should be identical to the one in config.c */

char *my_crypt (char *buf, char *pw) {
  char *s = crypt (pw, "Mu");
  *(s + PASSWD_LEN - strlen(PASSWD_PREFIX) - 1) = 0;
  sprintf(buf, "%s%s", PASSWD_PREFIX, s);
  return(buf);
}

void *resize_array (void *start, int elem_size, int oldlen, int newlen) {
  void *p;
  if (oldlen < 0 || newlen < 0 || elem_size <= 0) memory_failure();
  p = memory_alloc((size_t)newlen, (size_t)elem_size);
  if (start && oldlen && newlen)
    memcpy(p, start, (size_t)min(oldlen, newlen) * (size_t)elem_size);
  free(start);
  return p;
}

void init_intset (int_set * p, int len) {
  p->len = 0;
  p->list = NEW(int, len);
  p->maxlen = len;
}

void free_intset (int_set * p) {
  free(p->list);
  p->list = NULL;
  p->len = p->maxlen = 0;
}

Boolean add_int (int n, int_set * p) {
  int i;

  check_for_possible_resize(p);

  for (i = 0; i < p->len; i++)
    if (p->list[i] == n)
      return(False);

  p->list[p->len] = n;
  p->len++;
  return(True);
}

Boolean remove_int(int n, int_set * p) {
  int i;

  if (p->list == NULL || p->len == 0)
    return False;

  for (i = 0; i < p->len && p->list[i] != n; i++) ;
  if (i == p->len) return False;

  p->list[i] = p->list[p->len - 1];   /* copy last elem to deleted elem */
  (p->len)--;                         /* make list shorter */

  check_for_possible_resize(p);
  return(True);
}


int find_int (int n, int_set * p) {
  int i;

  if (p->list == NULL)
    return -1;

  for (i = 0; i < p->len && p->list[i] != n; i++) ;

  return i < p->len ? i + 1 : 0;
}

int foreach_int (int_set * p, int (*func) (int)) {
  int i;
  int n = 0;

  for (i = 0; i < p->len; i++)
    if (func (p->list[i]))
      n++;

  return n;
}

Boolean check_for_possible_resize (int_set *p) {
  int next = p->maxlen;
  if (p->len == next) {
    if (p->len > INT_MAX - 25) memory_failure();
    next = p->len < 20 ? 2 * (p->len + 1) : p->len + 25;
  } else if (next > 0 && p->len < next / 5) next /= 2;
  else return False;
  p->list = resize_array(p->list, sizeof(int), p->maxlen, next);
  p->maxlen = next;
  return True;
}


void init_inttable (int_table * p, int size) {
  p->table = NEW (table_entry *, p->len = size);

  while (--size >= 0)
    p->table[size] = NULL;
}


static table_entry *new_entry (long int key, long int value, table_entry * next)
{
  table_entry *p = NEW (table_entry, 1);

  p->key = key;
  p->value = value;
  p->next = next;

  return p;
}

static void unlink_entry (table_entry ** entry) {
  table_entry *tmp;

  tmp = *entry;
  *entry = (*entry)->next;

  FREE (tmp);
}


/* Return a pointer to the pointer to the entry that represents 'key',
 * starting the search at the entry to which *p points.
 * If it does not exist, return a pointer to the pointer to the entry that
 * would succede 'key', had it existed.
 */

static table_entry **find_position (long int key, table_entry ** p) {
  while (*p != NULL && (*p)->key > key)
    p = &(*p)->next;

  return p;
}

static table_entry *find_entry (long int key, int_table * p) {
  table_entry **r = find_position (key, &p->table[hash (key, p->len)]);

  return (*r != NULL && (*r)->key == key) ? *r : NULL;
}

Boolean insert_entry (long int key, long int value, int_table * p) {
  table_entry **q = find_position (key, &p->table[hash (key, p->len)]);

  if ((*q) == NULL) {
    *q = new_entry (key, value, NULL);
  } else if ((*q)->key < key) {
    *q = new_entry (key, value, *q);
  } else
    return False;

  return True;
}


Boolean remove_entry (long int key, int_table * p) {
  table_entry **q = find_position (key, &p->table[hash (key, p->len)]);

  if ((*q) != NULL && (*q)->key == key) {
    unlink_entry (q);
    return True;
  } 
  else
    return False;
}

long int lookup_entry (long int key, int_table * p) {
  table_entry *q = find_entry (key, p);

  return (q != NULL) ? (q)->value : NOT_IN_TABLE;
}

long int change_entry (long int key, long int new_value, int_table * p) {
  table_entry *q = find_entry (key, p);

  if (q != NULL) {
    long int v = q->value;

    q->value = new_value;
    return v;
  }
  else
    return NOT_IN_TABLE;
}
#endif

void ht_add(HASH_TABLE ht[], char *key, int val) {
  HASH_TABLE *ptr;

  if (!(*key)) {
    printf("ht_add: Storing an empty string is not allowed.\n");
    return;
  }

  for (ptr = &ht[hashfun(key)] ; ptr->next ; ptr = ptr->next);

  if (ptr->key) {
    ptr->next = (HASH_TABLE *) NEW(HASH_TABLE, 1);
    ptr->next->key = COPY(key);
    ptr->next->val = val;
  }
  else {                       /* first item added */
    ptr->key = COPY(key);
    ptr->val = val;
  }
}

void ht_remove(HASH_TABLE ht[], char *key, int val) {
  HASH_TABLE *ptr;
  int bucket;

  bucket = hashfun(key);
  for (ptr = &ht[bucket] ; ptr ; ptr = ptr->next)
    if (EQ(ptr->key, key) && (ptr->val == val)) {
      FREE(ptr->key);                               /* silly i know */
      ptr->key = COPY("");
    }
}

int hashfun(char *str) {
  char *p;
  int sum;

  for (p = str, sum = 0 ; *p ; p++)
    sum += tolower(*p);

  return(sum % BASE_SIZE);
}

int ht_lookup(HASH_TABLE ht[], char *key, int n, int z,
              int (eqfun) (int a, int b)) {
  HASH_TABLE *ptr;
  int bucket;

  bucket = hashfun(key);
  for (ptr = &ht[bucket] ; ptr ; ptr = ptr->next)
    if (ptr->key && EQ(ptr->key, key)) {
      if (eqfun(ptr->val, z))
        if (n-- == 0)
          return(ptr->val);
    }

  return(-1);
}

/* converts an integer to a unified game index, where 1, 4, 7 are MOBS, *
 * 2, 5, 8 are LOCS, and 3, 6, 9 are OBJS                               */

int int2idx(int i, int type) {

  switch(type) {
    case MOB:
      return((3 * i) + 1 + i/3);
    case LOC:
      return((3 * i) + 2 + i/3);
    case OBJ:
      return((3 * i) + 3 + i/3);
    default:
      return(-1);
  }
}


int idtxt2int(char *number, int type) {
  char *p;
  int n;

  if (isdigit(*number)) {
    for (p = number ; *p ; p++) {
      if (!isdigit(*p))
        return(-1);
    }
    if (p - number > INT_LEN)
      return(-1);

    n = atoi(number);
    return(idx2int(n, type));
  }
  return(-1);
}

/* converts a mob, loc, or obj number (unified format) to the proper int */

int idx2int(int n, int type) {
  int z, x;

  z = n % 10;                    /* z is last digit */
  x /= 10;                       /* round off last digit */
  x *= 10;
  x = ((n - 1)  - n/10) / 3;     /* calculate index */

  switch (z) {
    case 3: case 6: case 9:
      if (type == OBJ && x > -1 && x < numobs)
        return(x);
      break;
    case 2: case 5: case 8:
      if (type == LOC && x > -1 && x < numloc)
        return(x);
      break;
    case 1: case 4: case 7:
      if (type == MOB && x > -1 && x < numchars)
        return(x);
      break;
    default:
      break;
  }
  return(-1);
}
