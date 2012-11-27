/* Round FIFO queue */

typedef struct _rfifo_t
{
	int in;
	int out;
	int avl;
	int N;
	void ** table;
}rfifo_t;

void rfifo_init(rfifo_t* queue, int max_size)
{
	queue->avl =0; 
	queue->in = 0; 
	queue->out = 0;
	queue->max = max_size;
	queue->table = (void**)malloc(max_size*sizeof(void*));
}

void rfifo_close(rfifo_t* queue)
{
	int i;
	if (rfifo_empty(queue)) 
		free(table);
	else {
		for (i=0; i<queue->avl; i++) {
			free(queue->table[(queue->out+i)%queue->max]);
		}
		free(queue->table);
	}
	queue->table = NULL;
}

bool rfifo_empty(rfifo_t* queue)
{
	return (queue->avl==0);
}

bool rfifo_put(rfifo_t* queue, void *entry)
{
	if (avl == N) {
		//queue is full
		return false;
	} else {
		queue->table[queue->in] = entry;
		queue->avl++;
		queue->in = (queue->in+1) % queue->N;
		return true;
	}
}

/* return next element */
void* rfifo_get(rfifo_t* queue)
{
	void* get;
	if (queue->avl > 0) {
		get = queue->table[queue->out];
		queue->out = (queue->out+1) % N;
		queue->avl --;
		return get;
	} else {
		return NULL;
	}
}