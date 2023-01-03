**Warning:** Code presented in this file is a simplified version, which should be easy for reading and understanding the basic concept of AIO, but it is lacking of many error handling or including necessary header files. Those are required for the proper solution (don't forget about it during laboratories). Files with complete code may be found in the bottom of this document.

# Reusable functions

Before proceeding to the real tasks, some pieces of codes, which may be used in many aio tasks, will be presented. They will be used in following tasks too.

## `aiocb_init`

This function allows to avoid bothersome and 'space-consuming' initialization of `aiocb` structure. In case some fields are not needed in specific task, they may be removed from the parameters list and code (`memeset` will set default values nevertheless). Setup of `aio_sigevent` is not incuded, due to different setup depending on type of action (thread, signal or none). It is set to `SIGEV_NONE` as default, and user should manually change it if needed.

```c
void aiocb_init(struct aiocb *aiocb, int fd, off_t offset, volatile void *buf, size_t nbytes, int lio_code)
{
    memset(aiocb, 0, sizeof *aiocb);
    aiocb->aio_fildes = fd;
    aiocb->aio_offset = offset;
    aiocb->aio_buf = buf;
    aiocb->aio_nbytes = nbytes;
    aiocb->aio_lio_opcode = lio_code;
    aiocb->aio_sigevent.sigev_notify = SIGEV_NONE;
}
```


# Task 1

## Motivation
Asynchronous I/O provides the most efficient way of handling large amount of data. Instead of waiting for the data to be read from the storage, then processed and possibly written back, we may do processing and I/O at the same time. Moreover, it allows to utilize full bandwidth of copying data between two hard drives when both are doing I/O at the same time (it won't work for one device, since it can only read or write at a time).

## Task description
Our goal is to prepare a program, which will decipher multiple files and save the results. Since those messages are super time-sensitive, their must be printed to the stdin as soon as possible (in small chunks, which already are decrypted). This task should be handled in optimal way, hence async I/O is the best choice.

All messages are encrypted with the Vigenere cipher. It is described well [here](https://www.britannica.com/topic/Vigenere-cipher). In short, we have a message and key, which is a word. To encode, we repeat the key until it is as long as the message. Then we add the corresponding letters of the key and the message. If the sum is greater than 26, we subtract 26. The result is the encoded letter (e.g. msg: "hello world", key: "cat", encrypted: "jeeno pqref"). In our task, we handle only small letters, but the whitespaces and special characters must be preserved (we don't 'waste' key for them, it is used for next character e.g. msg: "hi! what's up?", key: "abc", "hj! yhbw's wr?" -'w' was coded with 'c' letter ).

Program should have the following interface:

```c
void usage(char *filename) {
    printf("Usage: %s nb sb msg1 key1 [msg2 key2 ...]\n", filename);
    printf("nb - number of aio blocks per each file\n");
    printf("sb - size of buffers in aio control blocks\n");
    printf("msg? - name/path of the file containing encrypted message\n");
    printf("key? - name/path of the file containing key used to encrypt the relevant (same number) message\n");
}
```

## Step 1 - Arguments parsing

Before we proceed to the main part of the program, we need to parse the arguments. We will use the following code:

```c
int main(int argc, char **argv) {
    if (argc < 5) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    int nb = atoi(argv[1]);
    int sb = atoi(argv[2]);
    int nfiles = (argc - 3) / 2;

    char **msg_files = (char **)malloc(nfiles * sizeof(char *));
    char **key_files = (char **)malloc(nfiles * sizeof(char *));
    for (int i = 0; i < nfiles; i++) {
        msg_files[i] = argv[3 + 2 * i];
        key_files[i] = argv[4 + 2 * i];
    }

    decryptor(nb, sb, msg_files, key_files, nfiles);
}
```

where `decryptor` is a function, which will handle all the processing. (**warning** - remember that this code is not complete, find the complete version at the bottom of this document).

## Step 2 - Boilerplate

At the beginning, we may prepare some functions which should be useful for this and simmilar tasks. The intention is to make the often repeated operations out of the specific task logic (better readibility and less error-prone) and make them reusable. Here are they:




# Task 2

AIO is also great for servers communication with many clients.