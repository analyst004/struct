#ifndef _QUEUE_FIFO_H_
#define _QUEUE_FIFO_H_

typedef struct _fifo
{
  volatile unsigned int tail;
  volatile unsigned int head;
  volatile unsigned int max;
  void** table;
}fifo_t;

static inline void fifo_init(fifo_t* fifo, int max)
{
  fifo->tail = 0;
  fifo->head = 0;
  fifo->max = max + 1;
  fifo->table = (void**)malloc(fifo->max*sizeof(void*));
}

static inline void fifo_close(fifo_t* fifo)
{
  free(fifo->table);
  fifo->table = NULL;
}

static inline unsigned int increment(unsigned int idx, unsigned int max)
{
   // increment or wrap
   // =================
   //    index++;
   //    if(index == array.lenght) -> index = 0;
   //
   //or as written below:   
   //    index = (index+1) % array.length
   idx = (idx+1) % max;
   return idx;
}

static inline bool fifo_put(fifo_t* fifo, void* entry)
{

  int next_tail = increment(fifo->tail, fifo->max);
  if (next_tail != fifo->head) {
    fifo->table[fifo->tail] = entry;
    fifo->tail = next_tail;
    return true;
  }

  /*queue is full*/
  return false;
}

static inline void* fifo_get(fifo_t* fifo)
{
    if (fifo->head == fifo->tail)
      /*empty queue*/
      return NULL;
    void *item = fifo->table[fifo->head];
    fifo->head = increment(fifo->head, fifo->max);
    return item;
}

static inline bool fifo_empty(fifo_t* fifo)
{
  return (fifo->head == fifo->tail);
}

static inline bool fifo_full(fifo_t* fifo)
{
  int check_tail = (fifo->tail+1) % fifo->max;
  return (check_tail == fifo->head); 
}

#endif /* _QUEUE_FIFO_H_ */
