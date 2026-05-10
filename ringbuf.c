#include <stdio.h>
#include <stdint.h>

#define BUFFER_SIZE 8

/* Return codes */
#define RB_OK    0
#define RB_FAIL -1

/* Ring Buffer Structure */
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

    /* Clear buffer manually */
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        rb->data[i] = 0;
    }
}


/* Check if buffer is full */
int rb_is_full(RingBuffer *rb)
{
    return (rb->count == BUFFER_SIZE);
}


/* Check if buffer is empty */
int rb_is_empty(RingBuffer *rb)
{
    return (rb->count == 0);
}


/* Return current buffer count */
uint8_t rb_count(RingBuffer *rb)
{
    return rb->count;
}


/*
 * Write one byte into buffer
 * Return:
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

    /*
     * Works only because BUFFER_SIZE is power of 2.
     */
    rb->head = (rb->head + 1) & (BUFFER_SIZE - 1);

    rb->count++;

    return RB_OK;
}


/*
 * Read one byte from buffer Return:
 * RB_OK   -> success
 * RB_FAIL -> buffer empty
 */
int rb_read(RingBuffer *rb, uint8_t *out_byte)
{
    if (rb_is_empty(rb))
    {
        return RB_FAIL;
    }

    *out_byte = rb->data[rb->tail];

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

    /* Write 8 bytes */
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


    /*Try to writing when full */
    printf("Writing when buffer is FULL\n\n");

    result = rb_write(&rb, 0x99);

    if (result == RB_FAIL)
    {
        printf("[WRITE] 0x99 -> FAIL (buffer full)\n");
    }

    printf("\n");


    /*Read the first 3 bytes */
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


    /*Write 3 more bytes */
    printf("Writing 3 more bytes\n\n");

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


    /*Read remaining bytes */
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


    /*Read when empty */
    printf("Reading when buffer is EMPTY\n\n");

    result = rb_read(&rb, &byte_read);

    if (result == RB_FAIL)
    {
        printf("[READ ] FAIL (buffer empty)\n");
    }

    return 0;
}