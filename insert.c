#include "apc.h"
{
    int insert_first(Dlist **head, Dlist **tail, int data)
{
    //creating new node
    Dlist *new = malloc(sizeof(Dlist));
    if(new == NULL)
    {
       return FAILURE;
    }
    new->data = data;
    new -> prev = NULL;
    new -> next = *head;
    if(*head == NULL)
    {
        *head = new;
        *tail = new;
        return SUCCESS;
    }
    (*head)->prev = new;
    *head = new;
    return SUCCESS;
}
}