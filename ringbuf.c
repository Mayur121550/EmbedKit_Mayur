

#include <stdio.h>
#include <stdint.h>
#include <string.h>


#define BUFFER_SIZE 8

/* Return codes */
#define RB_OK    0
#define RB_FAIL -1

/* Ring Buffer */
typedef struct
{
    uint8_t data[BUFFER_SIZE];
    uint8_t head;
    uint8_t tail;
    uint8_t count;

} RingBuffer;


/* Initialize the buffer */
void rb_init(RingBuffer *rb)
{
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;

    memset(rb->data, 0, sizeof(rb->data));
}


/* Check the buffer is  full */
int rb_is_full(RingBuffer *rb)
{
    return (rb->count == BUFFER_SIZE);
}


/* Check the buffer is empty */
int rb_is_empty(RingBuffer *rb)
{
    return (rb->count == 0);
}


/* Return  the current buffer count */
uint8_t rb_count(RingBuffer *rb)
{
    return rb->count;
}


/*
 * Write one byte into buffer its return
 * RB_OK   -> success
 * RB_FAIL -> buffer full
 */
int rb_write(RingBuffer *rb, uint8_t byte)
{
    if (rb_is_full(rb))
    {
        return RB_FAIL;
    }

    rb->data[rb->head] = byte;

    rb->head = (rb->head + 1) & (BUFFER_SIZE - 1);

    rb->count++;

    return RB_OK;
}



int rb_read(RingBuffer *rb, uint8_t *out_byte)
{
    if (rb_is_empty(rb))
    {
        return RB_FAIL;
    }

    *out_byte = rb->data[rb->tail];

    /* Circular wrap-around using bitwise AND */
    rb->tail = (rb->tail + 1) & (BUFFER_SIZE - 1);

    rb->count--;

    return RB_OK;
}


int main(void)
{
    RingBuffer rb;

    uint8_t byte_read;

    int result;

    rb_init(&rb);

   
    printf("Writing 8 bytes\n\n");

    uint8_t write_data[] =
    {
        0x41,
        0x42,
        0x43,
        0x44,
        0x45,
        0x46,
        0x47,
        0x48
    };

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        result = rb_write(&rb, write_data[i]);

        if (result == RB_OK)
        {
            printf("[WRITE] 0x%02X -> OK (count=%d)",
                   write_data[i],
                   rb_count(&rb));

            if (rb_is_full(&rb))
            {
                printf(" FULL");
            }

            printf("\n");
        }
    }

    printf("\n");


    printf("Writing when buffer is FULL\n\n");

    result = rb_write(&rb, 0x99);

    if (result == RB_FAIL)
    {
        printf("[WRITE] 0x99 -> FAIL (buffer full)\n");
    }

    printf("\n");


    printf("Reading first 3 bytes\n\n");

    for (int i = 0; i < 3; i++)
    {
        result = rb_read(&rb, &byte_read);

        if (result == RB_OK)
        {
            printf("[READ ] 0x%02X <- OK (count=%d)\n",
                   byte_read,
                   rb_count(&rb));
        }
    }

    printf("\n");


   
    printf(" Writing 3 more bytes\n\n");

    uint8_t new_data[] =
    {
        0x49,
        0x4A,
        0x4B
    };

    for (int i = 0; i < 3; i++)
    {
        result = rb_write(&rb, new_data[i]);

        if (result == RB_OK)
        {
            printf("[WRITE] 0x%02X -> OK (count=%d)",
                   new_data[i],
                   rb_count(&rb));

            if (rb_is_full(&rb))
            {
                printf(" FULL");
            }

            printf("\n");
        }
    }

    printf("\n");


   
    printf("Reading remaining bytes\n\n");

    while (!rb_is_empty(&rb))
    {
        result = rb_read(&rb, &byte_read);

        if (result == RB_OK)
        {
            printf("[READ ] 0x%02X <- OK (count=%d)",
                   byte_read,
                   rb_count(&rb));

            if (rb_is_empty(&rb))
            {
                printf(" EMPTY");
            }

            printf("\n");
        }
    }

    printf("\n");


    printf("Reading when buffer is EMPTY\n\n");

    result = rb_read(&rb, &byte_read);

    if (result == RB_FAIL)
    {
        printf("[READ ] FAIL (buffer empty)\n");
    }


    return 0;
}