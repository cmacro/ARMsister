/**
  *	@file       rtc.h
  * @author     crystalice
  * @version    v1.0
  * @date       2018-4-5
  * @brief      ARM rtc header file
  *
  */
  
#ifndef __MAIN_H__
#define __MAIN_H__



#ifdef __cplusplus
 extern "C" {
#endif
     
     
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
     
     
#ifdef __cplusplus
}
#endif



#endif /* __MAIN_H__ */

