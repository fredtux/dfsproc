{-# LANGUAGE OverloadedStrings #-}
{-# LANGUAGE TemplateHaskell #-}
{-# LANGUAGE QuasiQuotes #-}

import qualified Language.C.Inline as C
import qualified Language.C.Inline.Internal as C
import qualified Language.C.Types as C
import           Foreign.C.Types

import Foreign.Ptr
import Foreign.ForeignPtr
import Foreign.Storable
import System.IO.Unsafe

C.include "<linux/kernel.h>"
C.include "<sys/syscall.h>"
C.include "<stdio.h>"
C.include "<unistd.h>"
C.include "<string.h>"
C.include "<errno.h>"
C.include "<stdint.h>"
C.include "<stdlib.h>"
C.include "<linux/types.h>"


dfsProc :: IO (ForeignPtr (Ptr CInt))
dfsProc = do
  x <- [C.block| int** {
  typedef unsigned long u64;
typedef unsigned long u64;
struct ProcStruct {
	pid_t procID;
	int32_t level;
	int32_t state;
	unsigned int flags;
	unsigned int ptrace;
	int on_rq;
	int prio;
	int static_prio;
	int normal_prio;
	unsigned int rt_priority;
	unsigned int policy;
	int nr_cpus_allowed;
	unsigned short migration_flags;
	int exit_state;
	int exit_code;
	int exit_signal;
	int pdeath_signal;
	unsigned long jobctl;
	unsigned int personality;
	unsigned sched_reset_on_fork;
	unsigned sched_contributes_to_load;
	unsigned sched_migrated;
	unsigned long atomic_flags;
	pid_t pid;
	pid_t tgid;
	u64 utime;
	u64 stime;
	u64 gtime;
	int nr_dirtied;
	int nr_dirtied_pause;
	unsigned long dirty_paused_when;
	int32_t size_files;
	char files[32][32];
	char name[32];
};
  struct ProcStruct *ps = malloc(sizeof(struct ProcStruct) * 64);
  pid_t pid;
  long x = scanf("%d", &pid);
    
  int tmp = 1;
  int32_t *pidc = &tmp;
  if(syscall(451, pid, 64, pidc, ps) < 0)
      perror("Couldn't run the syscall");

  int **result = malloc(sizeof(int **) * ((*pidc) + 1));

  result[0] = malloc(sizeof (int *));
  result[0][0] = *pidc;

  for(int i = 0; i < *pidc; ++i){
      result[i+1] = malloc(sizeof (int *) * 3);
      result[i+1][0] = ps[i].procID;
      result[i+1][1] = ps[i].level;
      result[i+1][2] = ps[i].state;
  }
	return result;
	} |]
  let finalizer = [C.funPtr| void free2Darray(int** ps) {
   free(ps);
  }|]
  newForeignPtr finalizer x

listToPrettyTree :: [(CInt, String, String)] -> [Char]
listToPrettyTree xs = foldr step [] xs
   where step (pid, level, status) acc = 
                   "|" ++ (replicate (read level) '-') ++ " " ++ show pid ++ " (" ++ status ++ ")" ++ "\n" ++ acc

main :: IO ()
main = do

  dfsProc >>= (`withForeignPtr` \e -> do
    let peekStart = peekElemOff e

    putStr "\nPrint from C\n"
    e1 <- peekElemOff e 0
    e2 <- peekElemOff e1 0


    y <-  [C.block| int {
      int **ptr = (void *)$(int** e); 
      for(int i = 1; i <= ptr[0][0]; ++i) {
        for(int j = 0; j < 3; ++j){
          printf("%d ", ptr[i][j]);
        }

        printf("\n");
      }
        
      return ptr[0][0]; 
      }|]

    print y

    putStr "\nPrint from Haskell\n"

    let listPIDs [] = []; listPIDs (x:xs) = let ez = peekStart x; a = peekElemOff (unsafePerformIO $ ez) 0 in a : listPIDs xs
    let listLevels [] = []; listLevels (x:xs) = let ez = peekStart x; a = peekElemOff (unsafePerformIO $ ez) 1 in a : listLevels xs
    let listStatuses [] = []; listStatuses (x:xs) = let ez = peekStart x; a = peekElemOff (unsafePerformIO $ ez) 2 in a : listStatuses xs
    let numProcs = read (show y)
    
    let allPids = listPIDs [1 ..  numProcs]
    let allLevels = listLevels [1 .. numProcs]
    let allStatuses = listStatuses [1 .. numProcs]

    let dfsList = zip3 (map unsafePerformIO allPids) (map unsafePerformIO allLevels) (map unsafePerformIO allStatuses)

    print dfsList

    putStr "\nTree structure from Haskell\n"

    let printableTree = listToPrettyTree (map (\(pid,level,status) -> (pid, show level, show status)) dfsList)
    putStr printableTree

   )



