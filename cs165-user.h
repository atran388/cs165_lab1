#define USER_ADD 0
#define USER_REMOVE 1
#define USER_CHANGE 2
#define USER_JOIN 4
#define USER_LEAVE 5
#define USER_NEW 6
#define USER_DISPLAY 7
#define GROUP_NEW 8
#define GROUP_PURGE 9
#define GROUP_SHOW 10
#define SYSTEM_SKIP 11

#define STUDENT_USER 0
#define ANON_USER 1
#define SPY_USER 2

#define MAX_USERS 100
#define MAX_STRING 30
#define MAX_LINE 100

#define USAGE "cs165-p1 cmd-file output-file"

typedef struct elt {
  struct elt *next;
  struct elt *prev;
  void *obj;
} elt_t;

typedef struct list {
  elt_t *head;
} list_t;

/********************

  Important data structures 

*********************/


typedef struct group {
        int index;
        list_t *members;  // list of users
} group_t;


typedef struct user {
	int index;
        int type;
        group_t *group;
        int (*add) ( struct user *u );
        int (*change) ( struct user *u, char *val );
        int (*remove) ( struct user *u );
        int (*partner) ( struct user *u, struct user *other );  
        int (*join)( struct user *u, char *group_index );
        int (*leave)( struct user *u );
        int (*display) ( struct user *u );
} user_t;


typedef struct student {
	int index;
        int type;
        group_t *group;
        char name[MAX_STRING];
        char secret[MAX_STRING];
        int (*add) ( struct student *a, char *secret );
        int (*change) ( struct student *s, char *name );
        int (*remove) ( struct student *s );
        int (*partner) ( struct student *s, user_t *other );
        int (*join)( struct student *s, char *group_index );
        int (*leave)( struct student *s );
        int (*display) ( struct student *s );
} student_t; 


typedef struct anon {
	int index;
        int type;
        group_t *group;
        int id;
        int (*add) ( struct anon *a, char *arg );
        int (*change) ( struct anon *a, char *id_str );
        int (*remove) ( struct anon *a );
        int (*partner) ( struct anon *a, user_t *other );
        int (*join)( struct anon *a, char *group_index );
        int (*leave)( struct anon *a );
        int (*display) ( struct anon *a );
} anon_t; 


typedef struct spy {
	int index;
        int type;
        group_t *group;
        int id;
        user_t *real;
        int (*add) ( struct spy *m, char *arg);
        int (*change) ( struct spy *m, char *id_str );
        int (*remove) ( struct spy *m );
        int (*partner) ( struct spy *m, user_t *other );
        int (*join)( struct spy *m, char *group_index );
        int (*leave)( struct spy *m );
        int (*display) ( struct spy *m );
} spy_t; 


typedef struct system {
        user_t *members[MAX_USERS];
        list_t *groups;  // list of groups
} system_t;



/********************

  Project Macros and Inlines

*********************/
	

#define SPACE_CHAR( str,i ) \
	(( (str)[(i)] == ' ' ) || ( (str)[(i)] == '\t' ) || ( (str)[(i)] == '\n' ))


extern int mynoop();

#define USER_FN_ADD( u, type, arg )					\
  (( (type == STUDENT_USER) ? ((student_t *)u)->add(((student_t *)u), arg) : \
     (type == ANON_USER) ? ((anon_t *)u)->add(((anon_t *)u), arg) : \
     (type == SPY_USER) ? ((spy_t *)u)->add(((spy_t *)u), arg) : mynoop() ))

#define USER_FN_CHANGE( u, type, arg )					\
  (( (type == STUDENT_USER) ? ((student_t *)u)->change(((student_t *)u), arg) : \
     (type == ANON_USER) ? ((anon_t *)u)->change(((anon_t *)u), arg) : \
     (type == SPY_USER) ? ((spy_t *)u)->change(((spy_t *)u), arg) : mynoop() ))

#define USER_FN_JOIN( u, arg ) \
  (( (u->type == STUDENT_USER) ? ((student_t *)u)->join(((student_t *)u), arg) : \
     (u->type == ANON_USER) ? ((anon_t *)u)->join(((anon_t *)u), arg) : \
     (u->type == SPY_USER) ? ((spy_t *)u)->join(((spy_t *)u), arg) : mynoop() ))

#define USER_FN_LEAVE( u ) \
  (( (u->type == STUDENT_USER) ? ((student_t *)u)->leave(((student_t *)u)) : \
     (u->type == ANON_USER) ? ((anon_t *)u)->leave(((anon_t *)u)) : \
     (u->type == SPY_USER) ? ((spy_t *)u)->leave(((spy_t *)u)) : mynoop() ))

#define USER_FN_DISPLAY( u ) \
  (( (u->type == STUDENT_USER) ? ((student_t *)u)->display(((student_t *)u)) : \
     (u->type == ANON_USER) ? ((anon_t *)u)->display(((anon_t *)u)) : \
     (u->type == SPY_USER) ? ((spy_t *)u)->display(((spy_t *)u)) : mynoop() ))



static inline int strct( char *str, int len )
{
  int ct = 0;
  int instr = 0;
  int i;

  for ( i = 0; i < len; i++ ) {
    if (( !SPACE_CHAR( str, i )) && instr );
    else if (( !SPACE_CHAR( str, i )) && !instr ) { instr = 1, ct++; }
    else if ( SPACE_CHAR( str, i )) { instr = 0; }
    else { return -1; }
  }

  return ct;
}


/********************

  P1 external functions 

* to be implemented for the project

*********************/

// system
extern user_t *system_user_new( char *user_index, char *user_type, char *user_input );

// group
extern group_t *find_group( char *group_index );
extern int group_create( group_t *g, int index );
extern int group_add( group_t *g, user_t *u );
extern int group_remove( group_t *g, user_t *u );
extern int group_show( group_t *g );
extern int group_purge( group_t *g );

// user
extern int user_add( user_t *user, char *user_type, char *user_input );
extern int user_remove( user_t *user );
extern int user_change( user_t *user, char *user_input );
extern int user_partner( user_t *user, char *partner_index );
extern int user_display( user_t *user );
extern user_t *find_user( char *user_index_str );

// special user ops
extern int student_new( student_t *s, char *name );
extern int student_add( student_t *s, char *name );
extern int student_change( student_t *s, char *name );
extern int student_remove( student_t *s );
extern int student_display( student_t *s );
extern int student_join( student_t *s, char *group_index );
extern int student_leave( student_t *s );
//
extern int anon_new( anon_t *a, int id );
extern int anon_add( anon_t *a, char *arg );
extern int anon_change( anon_t *a, char *id_str );
extern int anon_remove( anon_t *a );
extern int anon_display( anon_t *a );
extern int anon_join( anon_t *a, char *group_index );
extern int anon_leave( anon_t *s );
//
extern int spy_new( spy_t *m, int id );
extern int spy_add( spy_t *m, char *arg );
extern int spy_change( spy_t *m, char *id_str );
extern int spy_remove( spy_t *a );
extern int spy_display( spy_t *m );
extern int spy_join( spy_t *a, char *group_index );
extern int spy_leave( spy_t *s );

