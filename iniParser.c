/*
Bastian Ruppert
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "iniParser.h"


/*
 * trim: get rid of trailing and leading whitespace...
 *       ...including the annoying "\n" from fgets()
 */
static char * trim (char * s)
{
  // Initialize start, end pointers 
  char *s1 = s, *s2 = &s[strlen (s) - 1];

  // Trim and delimit right side 
  while ( (isspace (*s2)) && (s2 >= s1) )
    s2--;
  *(s2+1) = '\0';

  // Trim left side 
  while ( (isspace (*s1)) && (s1 < s2) )
    s1++;

  // Copy finished string 
  strcpy (s, s1);
  return s;
}

int iniParser_getParam(char * file,char * param,char * target,int targetLen)
{
  char *s, buff[64];
  FILE *fp = fopen (file, "r");
  int ret=-1;

  if(targetLen<64)
    return -1;

  if (fp == NULL)
  {
    return -1;
  }

  // Read next line 
  while ((s = fgets (buff, sizeof buff, fp)) != NULL)
  {
    // Skip blank lines and comments 
    if (buff[0] == '\n' || buff[0] == '#')
      continue;

    // Parse name/value pair from line 
    char name[64], value[64];
    s = strtok (buff, "=");
    if (s==NULL)
      continue;
    else{
      strncpy (name, s, 63);
      trim (name);
    }
    s = strtok (NULL, "=");
    if (s==NULL)
      continue;
    else
      strncpy (value, s, 63);
    trim (value);

    //compare to requested param 
    if (strcmp(name,param)==0)
      {
	strncpy (target, value, 63);
	ret = 0;
	break;
      }
  }
  // Close file 
  fclose (fp);
  return ret;
}  

int iniParser_getParamList(char * file,struct t_iniParserParams * parameter)
{
  char *s, buff[64];
  int ret=0;
  FILE *fp = fopen (file, "r");
  struct t_iniParserParams * param = parameter;
  
  
  if (fp == NULL)
  {
    return -1;
  }

  // Read next line 
  while ((s = fgets (buff, sizeof buff, fp)) != NULL)
  {
    // Skip blank lines and comments 
    if (buff[0] == '\n' || buff[0] == '#')
      continue;

    // Parse name/value pair from line 
    char name[64], value[64];
    s = strtok (buff, "=");
    if (s==NULL)
      continue;
    else{
      strncpy (name, s, 63);
      trim (name);
    }
    s = strtok (NULL, "=");
    if (s==NULL)
      continue;
    else
      strncpy (value, s, 63);
    trim (value);

    //compare to requested param
    param = parameter;
    while(param->key)
    {
      if(!param->valid)
	if (strcmp(name,param->key)==0)
	  {
	    strncpy (param->value, value, 63);
	    param->valid = 1;
	    break;
	  }
      param++;
    }
  }
  //operation successfull ?
  param = parameter;
  while(param->key)
    {
      if(!param->valid)
	  {
	    ret = -2;
	    break;
	  }
      param++;
    }
  // Close file 
  fclose (fp);
  return ret;
} 
