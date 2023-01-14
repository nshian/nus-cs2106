#include "zc_io.h"
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>

// The zc_file struct is analogous to the FILE struct that you get from fopen.
struct zc_file {
  // Insert the fields you need here.
  int fd;
  void *base;
  size_t size;
  size_t offset;
  sem_t mutexRead;
  sem_t mutexWrite;
  int numReaders;
};

/**************
 * Exercise 1 *
 **************/

zc_file *zc_open(const char *path) {
  // To implement
  zc_file *file = calloc(1, sizeof(zc_file));
  sem_init(&file->mutexRead, 0, 1);
  sem_init(&file->mutexWrite, 0, 1);
  file->numReaders = 0;
  struct stat buf;
  int fd = open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  fstat(fd, &buf);
  file->size = buf.st_size;
  file->offset = 0;
  if (fd >= 0) {
    if (file->size == 0) {
      file->size = 1;
    }
    file->base = mmap(NULL, file->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    file->fd = fd;
    return file;
  }
  return NULL; //error
}

int zc_close(zc_file *file) {
  // To implement
  munmap(file->base, file->size);
  int status = close(file->fd); //if success, status = 0. otherwise it is -1
  free(file);
  return status;
}

const char *zc_read_start(zc_file *file, size_t *size) {
  // To implement
  if (file->offset > file->size) { //error checking
    return NULL;
  }
  sem_wait(&file->mutexRead);
  file->numReaders++;
  if (file->numReaders == 1)
    sem_wait(&file->mutexWrite);
  sem_post(&file->mutexRead);
  char *required = (char *) file->base + file->offset;
  if (file-> offset + *size > file->size) //not enough bytes to read
    *size = file->size - file->offset; //update the size to be what is left
  file->offset += *size;
  return required;
}

void zc_read_end(zc_file *file) {
  // To implement
  sem_wait(&file->mutexRead);
  file->numReaders--;
  if (file->numReaders == 0)
    sem_post(&file->mutexWrite);
  sem_post(&file->mutexRead);
}

/**************
 * Exercise 2 *
 **************/

char *zc_write_start(zc_file *file, size_t size) {
  // To implement
  sem_wait(&file->mutexWrite);
  if (file->offset + size < file->size) { //normal case
    char *required = (char *) file->base + file->offset;
    file->offset += size;
    return required;
  }
  //if func reaches this point, file is not big enough to write so we need to trunc and remap
  ftruncate(file->fd, file->offset + size);
  void *base = mremap(file->base, file->size, file->offset + size, MREMAP_MAYMOVE);
  if (base == (void *) MAP_FAILED)
    return NULL;
  file->base = base;
  char *required = (char *) file->base + file->offset;
  file->offset += size;
  return required;
}

void zc_write_end(zc_file *file) {
  // To implement
  msync(file->base, file->size, MS_SYNC);
  sem_post(&file->mutexWrite);
}

/**************
 * Exercise 3 *
 **************/

off_t zc_lseek(zc_file *file, long offset, int whence) {
  // To implement
  sem_wait(&file->mutexWrite);
  if (whence == SEEK_SET) {
    if (offset < 0) { //out of bounds, go back to before start
      sem_post(&file->mutexWrite);
      return -1;
    }
    file->offset = offset;
    sem_post(&file->mutexWrite);
  }
  else if (whence == SEEK_CUR) {
    if ((long)file->offset + offset < 0) { //out of bounds, go back to before start
      sem_post(&file->mutexWrite);
      return -1;
    }
    file->offset += offset;
    sem_post(&file->mutexWrite);
  }
  else { //SEEK_END
    if ((long)file->size + offset < 0) { //out of bounds, go back to before start
      sem_post(&file->mutexWrite);
      return -1;
    }
    file->offset = file->size + offset;
    sem_post(&file->mutexWrite);
  }
  return file->offset;
}

/**************
 * Exercise 4 *
 **************/

int zc_copyfile(const char *source, const char *dest) {
  // To implement
  zc_file *sourceFile = zc_open(source);
  zc_file *destFile = zc_open(dest);
  ftruncate(destFile->fd, sourceFile->size);
  const char *readBuf = zc_read_start(sourceFile, &(sourceFile->size));
  zc_read_end(sourceFile);
  char *writeBuf = zc_write_start(destFile, sourceFile->size);
  /*for(size_t i = 0; i < sourceFile->size; i++) { //causes segfault
    writeBuf[i] = readBuf[i];
  }*/
  memcpy(writeBuf, readBuf, sourceFile->size);
  zc_write_end(destFile);
  if (zc_close(destFile) == -1) {
    return -1;
  }
  if (zc_close(sourceFile) == -1) {
    return -1;
  }
  return 0;
}
