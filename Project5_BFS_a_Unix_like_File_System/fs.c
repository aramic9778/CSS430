// ============================================================================
// fs.c - user FileSytem API
// ============================================================================

#include "bfs.h"
#include "fs.h"

// ============================================================================
// Close the file currently open on file descriptor 'fd'.
// ============================================================================
i32 fsClose(i32 fd) { 
  i32 inum = bfsFdToInum(fd);
  bfsDerefOFT(inum);
  return 0; 
}



// ============================================================================
// Create the file called 'fname'.  Overwrite, if it already exsists.
// On success, return its file descriptor.  On failure, EFNF
// ============================================================================
i32 fsCreate(str fname) {
  i32 inum = bfsCreateFile(fname);
  if (inum == EFNF) return EFNF;
  return bfsInumToFd(inum);
}



// ============================================================================
// Format the BFS disk by initializing the SuperBlock, Inodes, Directory and 
// Freelist.  On succes, return 0.  On failure, abort
// ============================================================================
i32 fsFormat() {
  FILE* fp = fopen(BFSDISK, "w+b");
  if (fp == NULL) FATAL(EDISKCREATE);

  i32 ret = bfsInitSuper(fp);               // initialize Super block
  if (ret != 0) { fclose(fp); FATAL(ret); }

  ret = bfsInitInodes(fp);                  // initialize Inodes block
  if (ret != 0) { fclose(fp); FATAL(ret); }

  ret = bfsInitDir(fp);                     // initialize Dir block
  if (ret != 0) { fclose(fp); FATAL(ret); }

  ret = bfsInitFreeList();                  // initialize Freelist
  if (ret != 0) { fclose(fp); FATAL(ret); }

  fclose(fp);
  return 0;
}


// ============================================================================
// Mount the BFS disk.  It must already exist
// ============================================================================
i32 fsMount() {
  FILE* fp = fopen(BFSDISK, "rb");
  if (fp == NULL) FATAL(ENODISK);           // BFSDISK not found
  fclose(fp);
  return 0;
}



// ============================================================================
// Open the existing file called 'fname'.  On success, return its file 
// descriptor.  On failure, return EFNF
// ============================================================================
i32 fsOpen(str fname) {
  i32 inum = bfsLookupFile(fname);        // lookup 'fname' in Directory
  if (inum == EFNF) return EFNF;
  return bfsInumToFd(inum);
}



// ============================================================================
// Read 'numb' bytes of data from the cursor in the file currently fsOpen'd on
// File Descriptor 'fd' into 'buf'.  On success, return actual number of bytes
// read (may be less than 'numb' if we hit EOF).  On failure, abort
// ============================================================================
i32 fsRead(i32 fd, i32 numb, void *buf) {
  i32 inum = bfsFdToInum(fd); // getting the inum based on fd
  int not_read =
      numb;       // not readed bytes equal the number of bytes should be readed
  int readed = 0; // 0 bytes are readed so far
  int else_loop_counter = 0;             // variable for counting else loops
  while (not_read > 0) {                 // loop until not read equal 0
    i32 cursor = bfsTell(fd);            // cursor location
    i32 offset = cursor % BYTESPERBLOCK; // offset
    i32 fbn = cursor / BYTESPERBLOCK;    // fbn number
    i8 bio[BYTESPERBLOCK] = {0};         // empty bio buffer
    if (numb <= BYTESPERBLOCK) {         // if statement for the last loop
      bfsRead(inum, fbn, bio);           // read into bio based on fbn
      if (else_loop_counter == 0)        // if this is first and last loop
        memcpy(buf, bio + offset, numb); // copy to main buffer
      else                               // else if it is not last loop
        memcpy(buf + else_loop_counter * BYTESPERBLOCK, bio + offset,
               numb); // doing the same thing but copying based on else_loop
                      // counter
      bfsSetCursor(inum, cursor + numb); // setting cursor to new location
      readed = readed + numb;            // updating readed
      not_read = 0;                      // last loop
    } else {
      bfsRead(inum, fbn, bio);                    // read into bio
      memcpy(buf, bio + offset, BYTESPERBLOCK);   // copy to buf
      bfsSetCursor(inum, cursor + BYTESPERBLOCK); // set cursor
      numb -= BYTESPERBLOCK;                      // decreasing numb by 512
      readed += BYTESPERBLOCK;                    // inreasing read by 512
      else_loop_counter++;                        // increment counter
    }
  }
  return readed;
}


// ============================================================================
// Move the cursor for the file currently open on File Descriptor 'fd' to the
// byte-offset 'offset'.  'whence' can be any of:
//
//  SEEK_SET : set cursor to 'offset'
//  SEEK_CUR : add 'offset' to the current cursor
//  SEEK_END : add 'offset' to the size of the file
//
// On success, return 0.  On failure, abort
// ============================================================================
i32 fsSeek(i32 fd, i32 offset, i32 whence) {

  if (offset < 0) FATAL(EBADCURS);
 
  i32 inum = bfsFdToInum(fd);
  i32 ofte = bfsFindOFTE(inum);
  
  switch(whence) {
    case SEEK_SET:
      g_oft[ofte].curs = offset;
      break;
    case SEEK_CUR:
      g_oft[ofte].curs += offset;
      break;
    case SEEK_END: {
        i32 end = fsSize(fd);
        g_oft[ofte].curs = end + offset;
        break;
      }
    default:
        FATAL(EBADWHENCE);
  }
  return 0;
}



// ============================================================================
// Return the cursor position for the file open on File Descriptor 'fd'
// ============================================================================
i32 fsTell(i32 fd) {
  return bfsTell(fd);
}



// ============================================================================
// Retrieve the current file size in bytes.  This depends on the highest offset
// written to the file, or the highest offset set with the fsSeek function.  On
// success, return the file size.  On failure, abort
// ============================================================================
i32 fsSize(i32 fd) {
  i32 inum = bfsFdToInum(fd);
  return bfsGetSize(inum);
}



// ============================================================================
// Write 'numb' bytes of data from 'buf' into the file currently fsOpen'd on
// filedescriptor 'fd'.  The write starts at the current file offset for the
// destination file.  On success, return 0.  On failure, abort
// ============================================================================
i32 fsWrite(i32 fd, i32 numb, void *buf) {
  int not_read = numb;        // not readed bytes equal an numb argument
  int wrote = 0;              // wrote numbers are 0
  int else_loop_counter = 0;  // counter of else loop
  int lastoffset;             // lastoffset value
  i32 inum = bfsFdToInum(fd); // getting the inum based on fd
  while (not_read > 0)        // loop until all bytes are readed
  {
    i32 cursor = bfsTell(fd);            // cursor location
    i32 offset = cursor % BYTESPERBLOCK; // offset value
    i32 fbn = cursor / BYTESPERBLOCK;    // fbn number
    i8 bio[BYTESPERBLOCK] = {0};         // empty bio buffer
    if (numb <= BYTESPERBLOCK)           // if less than 512
    {
      i32 dbn = bfsFbnToDbn(inum, fbn);  // dbn number
      bfsRead(inum, fbn, bio);           // reading into bio
      if (else_loop_counter == 0)        // if first and last loop
        memcpy(bio + offset, buf, numb); // copying memory
      else                               // if not the last loop
        memcpy(bio + offset,
               buf + else_loop_counter * (BYTESPERBLOCK - lastoffset),
               numb); // copying memory
      bioWrite(dbn, bio);                // write
      bfsSetCursor(inum, cursor + numb); // setting cursor
      not_read = 0;                      // loop is done
    } else {
      i32 dbn = bfsFbnToDbn(inum, fbn); // dbn number
      bfsRead(inum, fbn, bio);          // reading into bio
      memcpy(bio + offset, buf,
             BYTESPERBLOCK - offset); // memcopy to the main buffer
      bioWrite(dbn, bio);             // writing
      bfsSetCursor(inum, cursor + BYTESPERBLOCK - offset); // setting cursor
      numb -= (BYTESPERBLOCK - offset);                    // decreasing numbers
      else_loop_counter++;                                 // increment by one
      lastoffset = offset;                                 // update lastoffset
    }
  }
  return 0;
}
