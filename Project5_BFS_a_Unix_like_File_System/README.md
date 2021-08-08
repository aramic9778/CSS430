The filesystem comprises 3 layers.
From top to bottom, these are:
1. fs – user-level filesystem, with functions like fsOpen, fsRead, fsSeek, fsClose.
2. bfs – functions internal to BFS such as bfsFindFreeBlock, bfsInitFreeList.
3. bio - lowest level block IO functions: bioRead and bioWrite.
