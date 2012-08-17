///////////////////////////////////////////////////////////////////////////////
// dllist.h
//
// Copyright (C) 2000-2005 Ake Hedman, eurosource, <akhe@eurosource.se>
//
// This software is placed into
// the public domain and may be used for any purpose.  However, this
// notice must not be changed or removed and no warranty is either
// expressed or implied by its publication or distribution.
//
// $RCSfile: dllist.h,v $                                       
// $Date: 2005/01/05 12:18:27 $                                  
// $Author: akhe $                                              
// $Revision: 1.3 $ 
///////////////////////////////////////////////////////////////////////////////

#ifndef H_DLLIST_H
#define H_DLLIST_H


#ifndef BOOL
#define BOOL int
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif


#ifdef __cplusplus
extern "C" {
#endif

/*!
   A base class to derive from for object storage
*/

typedef struct _dllnode {
  /*!
     String key used for alphanumeric sort
  */
	char *pstrKey;	
  
/*!
    Numeric key used for numeric sort
*/
	unsigned long *pKey;

/*!
	Sort key (can be used if no other key is abailabe ). pKey should
	be pointing to this key if used.
*/
	unsigned long Key;

  /*!
     A pointer to the embedded node object
  */
  void *pObject;

  /*!
	A long that can be used in any way a user likes.
  */
  unsigned long obid;
  
  /*!
     Pointer to the next node
  */
  struct _dllnode *pNext;
  
  
  /*!
     Pointer to the previous node
  */
  struct _dllnode *pPrev;

} dllnode;

/*!
   Methods to handle a node for the double linked list
*/

/*!
   Sorttypes for the list
*/
//enum _sorttype { SORT_NONE = 0, SORT_STRING, SORT_NUMERIC };
#define SORT_NONE			0
#define SORT_STRING			1
#define SORT_NUMERIC		2

typedef struct {
  /*!
     Sort order
     ==========

     0 - not sorted.
     1 - sort on string key.
     2 - sort on numeric key.
     
     Note that items can be retrived in accending/decending order by 
     retriving them from the head or from the tail of the list.
  */	
  unsigned char nSortOrder;

  /*!
     Pointer to the head of the linked list
  */
  dllnode *pHead;
  
  /*!
     Pointer to the tail of the linked list
  */
  dllnode *pTail;

  /*!
	Number of elements in list
  */
  unsigned long nCount;

} DoubleLinkedList;
 

/*!
   Initialize the double linked list
*/
void dll_init( DoubleLinkedList *, unsigned char );
  
/*!
   Add a node to the front of the linked list.
*/
BOOL dll_addNodeHead( DoubleLinkedList *, dllnode * );


/*!
   Add a node to the end of the linked list.
*/
BOOL dll_addNodeTail( DoubleLinkedList *, dllnode * );

  
/*!
   Add a node before another node
*/
BOOL dll_addNodeBefore( DoubleLinkedList *pdll, 
												dllnode *pNode,
												dllnode *pInsertNode ); 

/*!
   Add a node after another node.
*/
BOOL dll_addNodeAfter(	DoubleLinkedList *pdll, 
												dllnode *pNode,
												dllnode *pInsertNode );


/*!
   Add a node sorted by the current sort order 
*/					
BOOL dll_addNode( DoubleLinkedList *, dllnode * );
  
  
/*!
   Get the node count (calculating version).
*/
unsigned int dll_getNodeCount( DoubleLinkedList * );


/*!
   Remove all nodes form the linked list.
*/									
BOOL dll_removeAllNodes( DoubleLinkedList * );

/*!
   Get node from its numerical key
   
   @param Numerical key for node.
   @return Found Object
   @return NULL if no match found.
*/
dllnode * dll_findNodeFromID( DoubleLinkedList *, unsigned long );

/*!
   Get node from its string key
   
   @param String key for node.
   @return Found Object
   @return NULL if no match found.
*/
dllnode * dll_findNodeFromString( DoubleLinkedList *, char *  );
  
/*!
   Insert a node and an object between two other nodes 
*/
BOOL dll_insertNode( DoubleLinkedList *pdll, 
											dllnode *pNode1, 
											dllnode *pNode2 );

/*!
	Remove a node from the double linked list
*/
BOOL dll_removeNode( DoubleLinkedList *pdll, 
											dllnode *pNode );

/*!
	Get the node count for the list
*/
unsigned int dll_getNodeCount( DoubleLinkedList *pdll );



#ifdef __cplusplus
}
#endif

#endif // H_DDLIST_H
