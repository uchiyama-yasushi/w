VPATH += ${ROOT_DIR}/filesystem
VPATH += ${ROOT_DIR}/filesystem/sysvbfs
CFLAGS += -I${ROOT_DIR}/filesystem -DBFS_DEBUG

BFS_CORE_OBJS		= bfs.o bfs_init.o
BFS_DATAFILE_OBJS	= bfs_datafile.o
