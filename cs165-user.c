#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "cs165-user.h"

system_t users;
FILE *ifile;
FILE *ofile;


/*********************************
  
  Task 2-4 functions
  See function prototypes in cs165-user.h

  Example for "student_new" function for
  creating a student_t object provided.

**********************************/

int student_new( student_t *s, char *name ) 
{
  strcpy( s->name, name );
  s->add = student_add;
  s->remove = student_remove;
  s->change = student_change;
  s->join = student_join;
  s->leave = student_leave;
  s->display = student_display;
  
  return 0;
}

int student_add( student_t *s, char *name ) {
  if (strlen(name) < MAX_STRING) {
    strcpy( s->secret , name );
    return 0;
  } else {
    return -1;
  }
}

int student_change( student_t *s, char *name ) {
  if (strlen(name) < MAX_STRING) {
    strcpy( s->name , name );
    return 0;
  } else {
    return -1;
  }
  return 0;
}

int student_remove( student_t *s ) {
  // figure out which student type it is: student anon or spy
  // write helper function to clean up data fields of specific type
  // free pointer
  memset( s->index , 0, MAX_STRING );
  memset( s->name , 0, MAX_STRING );
  memset( s->secret , 0, MAX_STRING );
  memset( s->group , 0, MAX_STRING );
  
  free(s);

  return 0;
}

int student_display( student_t *s ) {
  return s->index;
}

int student_join( student_t *s, char *group_index ) {
  strcpy( s->join , group_index );

  return 0;
}

int student_leave( student_t *s ) {
  return 0;
}

/******************************************************************************

   Help for Tasks 2-4

   Creating a new user object

*******************************************************************************/

extern int free_slot( user_t *user_array[] );

user_t *system_user_new( char *user_index, char *user_type, char *user_input )
{
  int index = atoi(user_index);
  int type = atoi(user_type);
  student_t *s;
  anon_t *a;
  spy_t *m;
  user_t *new_user;
  int id;

  if ( find_user( user_index )) return (user_t *)NULL;
  
  switch(type) {
  case STUDENT_USER:
    s = (student_t *) malloc(sizeof(student_t));
    student_new(s, user_input);
    new_user = (user_t *)s;
    break;
  case ANON_USER:
    a = (anon_t *) malloc(sizeof(anon_t));
    id = atoi(user_input);
    anon_new(a, id);
    new_user = (user_t *)a;
    break;
  case SPY_USER:
    m = (spy_t *) malloc(sizeof(spy_t));
    id = atoi(user_input);
    spy_new(m, id);
    new_user = (user_t *)m;
    break;
  default:
    return (user_t *)NULL;
    break;
  }

  int slot = free_slot(users.members);
  if ( slot < 0 ) {
    free( new_user );
    return (user_t *)NULL;  // no free slot to add a new user
  }
  users.members[slot] = new_user;
  new_user->index = index;
  new_user->type = type;

  return new_user;
}


int free_slot( user_t *user_array[] )
{
  for ( int i = 0; i < MAX_USERS; i++ ) {
    if ( users.members[i] == NULL ) {
      return i;
    }
  }

  return -1;
}


/******************************************************************************

High-level Command Processing Functions: provided

*******************************************************************************/

/******************************************************************************

Function: find_command
Input Args:
          cmdbuf - buffer for the command line from the input file
          len - length of the cmdbuf
Returns: a command index or an error value (<0)
 
find_command determines the specific command and checks the argument
count.

*******************************************************************************/

int find_command( char *cmdbuf, int len )
{
  int ct = strct( cmdbuf, len );

  switch( cmdbuf[0] ) {
  case 'u':   // new user 
    if ( ct != 4 ) {
      return -1;
    }
    return USER_NEW;
    break;
  case 'a':   // add user-specific info
    if ( ct != 4 ) {
      return -1;
    }
    return USER_ADD;
    break;
  case 'r':   // remove
    if ( ct != 2 ) {
      return -1;
    }
    return USER_REMOVE;
    break;
  case 'c':   // change
    if ( ct != 4 ) {
      return -1;
    }
    return USER_CHANGE;
    break;
  case 'j':   // join group 
    if ( ct != 3 ) {
      return -1;
    }
    return USER_JOIN;
    break;
  case 'l':   // leave group 
    if ( ct != 2 ) {
      return -1;
    }
    return USER_LEAVE;
    break;
  case 'd':   // display
    if ( ct != 2 ) {
      return -1;
    }
    return USER_DISPLAY;
    break;
  case 'g':   // new group
    if ( ct != 2 ) {
      return -1;
    }
    return GROUP_NEW;
    break;
  case 'p':   // delete (purge) group
    if ( ct != 2 ) {
      return -1;
    }
    return GROUP_PURGE;
    break;
  case 's':   // group display members
    if ( ct != 2 ) {
      return -1;
    }
    return GROUP_SHOW;
    break;
  case '#':   // comment lines
  case '%':
  case '\n':
    return SYSTEM_SKIP;
    break;
  }

  return -1;
}


/******************************************************************************

Function: find_user
Input Args:
          user_index_str - the index as a string
          cmd - command number
Returns: a user reference or NULL
        
find_user find the user correpsonding to the index value from
<user_index_str>, if one exists.

*******************************************************************************/

user_t *find_user( char *user_index_str )
{
  int user_index = atoi( user_index_str );  
  int i;

  if ( user_index > 0 ) {
    for ( i = 0; i < MAX_USERS; i++ ) {
      user_t *test_user = users.members[i];
      if (( test_user ) && ( test_user->index == user_index )) {
	return test_user;
      }
    }
  }

  return (user_t *) NULL;
}


/******************************************************************************

Function: apply_command
Input Args:
          user - user object (may be NULL for USER_NEW)
          cmd - command number
          args - arguments string for the command
Returns: 0 on success and <0 on error
 
Run the respective command on the user object with the exception of
USER_NEW, which creates a user object.

*******************************************************************************/


int apply_command( user_t *user, int cmd, char *args )
{
  char arg1[MAX_STRING], arg2[MAX_STRING], arg3[MAX_STRING];
  int res = 0;

#if 0
  student_t *s = NULL;
  anon_t *a = NULL;
  spy_t *m = NULL;

  if ( user ) {
    switch (user->type) {
    case STUDENT_USER:
      s = (student_t *)user;
      break;
    case ANON_USER:
      a = (anon_t *)user;
      break;
    case SPY_USER:
      m = (spy_t *)user;
      break;
    default:
      return -1;
    }
  }
#endif
  
  
  switch( cmd ) {
  case USER_NEW:
    if (( sscanf( args, "%s %s %s", arg1, arg2, arg3 )) != 3 ) {
	return -1;
    }
    user = system_user_new( arg1, arg2, arg3 );
    if ( user ) {
      fprintf( ofile, "USER_NEW for index %s type %s and name/id %s: result %d\n", arg1, arg2, arg3, res );
      return 0;
    }
    else {
      return -1;
    }
    break;
  case USER_ADD:
    if (( sscanf( args, "%s %s", arg1, arg2 )) != 2 ) {
      return -1;
    }
    res = USER_FN_ADD( user, atoi(arg1), arg2 );
    fprintf( ofile, "USER_ADD for user %d and type %s to add info %s: result %d\n", user->index, arg1, arg2, res );
    return res;
    break;
  case USER_REMOVE:
    fprintf( ofile, "USER_REMOVE for user %d(%d) : result %d\n", user->index, user->type, res );
    res = user_remove( user );
    if ( res < 0 ) {
      fprintf( ofile, "USER_REMOVE failure : result %d\n", res );
    }
    return res;
    break;
  case USER_CHANGE:
    if (( sscanf( args, "%s %s", arg1, arg2 )) != 2 ) {
	return -1;
    }
    res = USER_FN_CHANGE( user, atoi(arg1), arg2 );
    fprintf( ofile, "USER_CHANGE for user %d and type %s to add secret %s : result %d\n", user->index, arg1, arg2, res );
    return res;
    break;
  case USER_JOIN:
    if (( sscanf( args, "%s", arg1 )) != 1 ) {
	return -1;
    }
    res = USER_FN_JOIN( user, arg1 );
    fprintf( ofile, "USER_JOIN for user %d(%d) to join group %s : result %d\n", user->index, user->type, arg1, res );
    return res;
    break;
  case USER_LEAVE:
    res = USER_FN_LEAVE( user );
    fprintf( ofile, "USER_LEAVE for user %d(%d) to leave group %d: result %d\n", user->index, user->type, ( (user->group) ? (user->group->index) : -1) , res );
    return res;
    break;
  case USER_DISPLAY:
    res = USER_FN_DISPLAY( user );
    fprintf( ofile, "USER_DISPLAY for user %d(%d) : result %d\n", user->index, user->type, res );
    return res;
    break;
  default:
    fprintf( ofile, "Err: apply_command: Invalid command\n");
    return -1;
    break;
  }
}

int mynoop()
{
  printf("ERR: running in noop - some kind of bug in the user object\n");
  return 0;
}



/******************************************************************************

Function: apply_user_command
Input Args:
          cmd - command number
          cmdstr - string for command
Returns: 0 on success and <0 on error
 
Prepares the command string (cmdstr) into individual arguments (args)
to run the command on user objects in apply_command.

*******************************************************************************/


int apply_user_command( int cmd, char *cmdstr )
{
  user_t *user = NULL;
  char user_index_str[MAX_STRING];  
  char *args;
  int res = 0;
  
  args = cmdstr;

  if ( cmd != USER_NEW ) {
    sscanf( cmdstr, "%s", user_index_str );  
    user = find_user( user_index_str );  

    // skip over user_index_str arg
    while (( (++args)[0] != ' ' ) && (args[0] != '\n'));
    args++;

    if ( user == NULL ) {
      return -1;
    }
  }

  // apply the command
	  
  res = apply_command( user, cmd, args );

  if ( res < 0 ) {
    fprintf( ofile, "ERR: failed command: cmd %d for line %s\n", cmd, cmdstr );
  }

  return res;
}


/******************************************************************************

Function: apply_group_command
Input Args:
          cmd - command number
          cmdstr - string for command
Returns: 0 on success and <0 on error
 
Prepares the command string (cmdstr) to run the respective command (cmd) on
the group object.

*******************************************************************************/

int apply_group_command( int cmd, char *cmdstr )
{
  group_t *group = NULL;
  char group_index_str[MAX_STRING];  
  int res = 0;
  
  sscanf( cmdstr, "%s", group_index_str );  
  if ( cmd != GROUP_NEW ) {
    group = find_group( group_index_str );  

    if ( group == NULL ) {
      return -1;
    }
  }

  // apply the command
	  
  switch( cmd ) {
  case GROUP_NEW:
    group = (group_t *)malloc(sizeof(group_t));
    group->index = atoi(group_index_str);
    group->members = (list_t *)malloc(sizeof(list_t));
    group->members->head = (elt_t *)NULL;
    fprintf( ofile, "GROUP_NEW: added new group %s\n", group_index_str );
    break;
  case GROUP_PURGE:
    fprintf( ofile, "GROUP_PURGE: removing group %d: res: %d\n", group->index, res );
    res = group_purge( group );
    break;
  case GROUP_SHOW:
    fprintf( ofile, "GROUP_SHOW: showing group %d: res: %d\n", group->index, res );
    res = group_show( group );
    return res;
    break;
  }

  if ( res < 0 ) {
    fprintf( ofile, "ERR: failed command: cmd %d for group %s for line %s\n", cmd, group_index_str, cmdstr );
  }

  return res;
}


/******************************************************************************

Function: main

Open the input and output files
Obtain the command lines one-by-one from the input file
Determine the command to run in "find_command"
Run the command in one of the "apply_command" functions

*** Task 1: complete the input processing in main

*******************************************************************************/



int main( int argc, char *argv[] )
{
	FILE *file;
	char *line;
	int cmd;
	int res = 0;
	char *cmdstr;
	size_t len = MAX_LINE;


	// initialize system
	users.groups = (list_t *)malloc(sizeof(list_t));
	users.groups->head = (elt_t *)NULL;
	memset( users.members, 0, sizeof(user_t *) * MAX_USERS );
	
	// check usage
	if ( argc != 3 ) {
	  printf( "Main: Err: Incorrect usage: %s\n", USAGE );
	}

	// open fds
	if (( file = fopen( argv[1], "r" )) == NULL ) {
	  exit(-1);
	}

	ifile = file;

	if (( file = fopen( argv[2], "w+" )) == NULL ) {
	  exit(-1);
	}

	ofile = file;
	 
	// get input
	line = (char *)malloc( len );
	while (1)  // Task 1: get a line of input from the input file
	  {
	  
	  cmdstr = line;
	  
	  // determine command from command string (first arg in line)

	  cmd = find_command( cmdstr, strlen( cmdstr ));
	  if ( cmd == SYSTEM_SKIP ) {
	    continue;
	  }	  
	  if ( cmd < 0 ) {
	    fprintf( ofile, "ERR: invalid command line: %s", line );
	    continue;
	  }

	  while ( (++cmdstr)[0] != ' ' );
	  cmdstr++;

	  if ( cmd >= GROUP_NEW ) { // if a group command
	    res = apply_group_command( cmd, cmdstr );
	  }
	  else {
	    res = apply_user_command( cmd, cmdstr );
	  }
	}

	exit( res );
}



