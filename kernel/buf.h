/******************************************************************
 * @struct buf
 * @brief Represents a buffer in the buffer cache.
 *
 * This structure is used to manage buffers that hold data read from or written to disk.
 * Buffers are used by the file system to cache disk blocks in memory, reducing the number
 * of disk reads and providing a synchronization point for disk blocks used by multiple processes.
 * The buffer cache uses a least-recently-used (LRU) policy to recycle buffers.
 * 
 * The buffer structure contains the following fields:
 * - `valid`  : Indicates whether the buffer contains valid data read from disk.
 * - `disk`   : Indicates whether the buffer is owned by the disk.
 * - `dev`    : The device number of the disk containing the buffer.
 * - `blockno`: The block number of the disk block stored in the buffer.
 * - `refcnt` : The reference count of the buffer.
 * - `lock`   : A sleep lock used to synchronize access to the buffer.
 * - `prev`   : A pointer to the previous buffer in the LRU cache list.
 * - `next`   : A pointer to the next buffer in the LRU cache list.
 * - `data`   : The data stored in the buffer.
 * 
 *****************************************************************/

struct buf
{
  int valid;
  int disk;
  uint dev;
  uint blockno;
  uint refcnt;
  struct sleeplock lock;
  struct buf *prev;
  struct buf *next;
  uchar data[BSIZE];
};
