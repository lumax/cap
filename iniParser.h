/* 
Bastian Ruppert
*/

#ifndef __iniParser_h__
#define __iniParser_h__
#ifdef __cplusplus
extern "C" {
#endif

/*! \brief parse init File for param
 *
 * @file: iniFile
 * @param: The param searching for (max param length = 64 byte)
 * @target: place to copy the param in
 * @targetLen: parameter can be max 64 byte long
 */
int iniParser_getParam(char * file,char * param,char * target,int targetLen);


struct t_iniParserParams{
  int valid;
  const char * key;
  char value[64];
};

/*! \brief parse init File for param
 *
 * @file: iniFile
 * @parameter: List of parameter searching for, the last element key must be 0.
 *
 * static struct t_iniParserParams params[] = {
 *  [0] = {
 *   .key = "MyAmazingKey",
 *   .value="Amazing"
 *  },
 *  [1] = {
 *   .key = 0,
 *   },
 *  };
 */
int iniParser_getParamList(char * file,struct t_iniParserParams * parameter);

#ifdef __cplusplus
}
#endif
#endif /* __iniParser_h__ */

