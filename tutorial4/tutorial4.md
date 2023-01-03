# Asynchronous I/O

**Warning:** Code presented in this file is a simplified version, which should be easy for reading and understanding the basic concept of AIO, but it is lacking of many error handling or including necessary header files. Those are required for the proper solution (don't forget about it during laboratories). Files with complete code may be found in the bottom of this document.

## Task 1

### Motivation
Asynchronous I/O provides the most efficient way of handling large amount of data. Instead of waiting for the data to be read from the storage, then processed and possibly written back, we may do processing and I/O at the same time. Moreover, it allows to utilize full bandwidth of copying data between two hard drives when both are doing I/O at the same time (it won't work for one device, since it can only read or write at a time).

### Task description
Our goal is to prepare a program, which will decipher multiple files and save the results. Since those messages are super time-sensitive, their must be printed to the stdin as soon as possible (in small chunks, which already are decrypted). This task should be handled in optimal way, hence async I/O is the best choice.

All messages are encrypted with the Vigenere cipher. It is described well [here](https://www.britannica.com/topic/Vigenere-cipher). In short, we have a message and key, which is a word. To encode, we repeat the key until it is as long as the message. Then we add the corresponding letters of the key and the message. If the sum is greater than 26, we subtract 26. The result is the encoded letter (e.g. msg: "hello world", key: "cat", encrypted: "jeeno pqref"). In our task, we handle only small letters, but the whitespaces and special characters must be preserved (we don't 'waste' key for them, it is used for next character e.g. msg: "hi! what's up?", key: "abc", "hj! yhbw's wr?" -'w' was coded with 'c' letter ).

Program have to accept the arguments as follows:

```c
void usage(char *filename) {
    printf("Usage: %s nb sb msg1 key1 [msg2 key2 ...]\n", filename);
    printf("nb - number of aio blocks per each file\n");
    printf("sb - size of buffers in aio control blocks\n");
    printf("msg? - name/path of the file containing encrypted message\n");
    printf("key? - name/path of the file containing key used to encrypt the relevant (same number) message\n");
}
```



### File structure and arguments parsing

Before we will implement AIO itself, we need to handle arguments and prepare files for reading and writing. 

```c





## Task 2

AIO is also great for servers communication with many clients.