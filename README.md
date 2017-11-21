# Simple 2 Phased Readers-Writer Lock
A readers-wrier(RW) lock is a synchronization method that solves <a href="http://cs.nyu.edu/~lerner/spring10/MCP-S10-Read04-ReadersWriters.pdf"> readers-writers problems</a>. An RW lock allows concurrent access for multiple read-only operations, while write operations require exclusively. As the writer lock is acquired, all other readers or writers will be blocked until it is released.  
<br>

## 2 Phased Lock
>  Two-phase locking (2PL) is a concurrency control method that guarantees serializability.  

Philip A. Bernstein, Vassos Hadzilacos, Nathan Goodman (1987): Concurrency Control and Recovery in Database Systems, Addison Wesley Publishing Company, ISBN 0-201-10715-5  
Gerhard Weikum, Gottfried Vossen (2001): Transactional Information Systems, Elsevier, ISBN 1-55860-508-8  

<br>
There are two phases. 

``` 
Expanding(Growing) phase - locks are acquired and no locks are released  
Shrinking phase - locks are released and no locks are acquired.  
```
<br>

## Performance
### Enviornment
Intel(R) Core(TM) i5-4460  CPU @ 3.20GHz 
<br>


| N |  R  |    E    | with -O0 (ms) | -with -O3 (ms) |
|:-:|:---:|:-------:|:-------------:|:--------------:|
| 4 | 100 |    1000 |            14 |             18 |
| 4 | 100 |   10000 |           184 |            126 |
| 4 | 100 |  100000 |           978 |            601 |
| 4 |  50 | 1000000 |         10386 |           4687 |
| 4 | 100 | 1000000 |          9656 |           4758 |
| 4 | 200 | 1000000 |         16643 |           7162 |
| 4 | 400 | 1000000 |         29528 |          11580 |

<br>

## Convention
The following variables and expressions may be used without further definition in this document and the code.  
```
global_execution_order - the most recent commit ID
N - the number of threads spawned  
R - the number of records (=the number of log files)
E - the maximum of global_exeuction_order  
acquired - The lock is issued. You may freely read or write.
pending - The lock is in the queue. It can be rejected if any deadlocks are detected.
requested - acquired or pending
```
<br>

## Deadlock Detection Algorithm
Pseudo Code
```
bool checkDeadLock(node) {
    enque all locks requested by this thread
    while( !q.empty()) 
        cur = deque()
        if (cur->prv->tid = node->tid) return true
        if (cur->prv is not visited) enque(cur->prv)
        for each lock in (locks requested by this thread)
              if (lock != node) enque(lock)

    return false;
}
```
Its time complexity O(NR) but, practically, it can be done in fewer steps.
             
<br>

## Creating Transaction Manager
The following code will generate a *N* threaded manager with *R* records to keep doing transaction until commit ID reaches to *E*. The commit log will output to *thread[thread ID].txt*. (*thread ID* here is 1-based)
```
TransactionManager tm(N, R, E);

 // wait
tm.join();
```

<br>

## Modifying Transaction Manager

### Acquiring Readers Lock
In *transaction* method,

```
if (lkTbl.trySharedLock(tid, rid) == false) {
    //Deadlock detected

    // Rollback the installs after the last commit in reverse order.
    /* records[j].write(records[i].read())
                   ...      */
   
assert(lkTbl.releaseAndWait(tid, [record id 1], [record id 2], ...));
```

We now support releasing upto 3 at once but you may call for many times or modify the code inside the function. It is recommended to wrap with an assertion.
<br>

### Commit

```
long long commitId = ++global_execution_order;
if (commitId > E) {
    /* Rollback example */
    records[j].write(records[j].read() - (valI + 1));
    records[k].write(records[k].read() + valI);

    /*  Releasing locks example */
    assert(lkTbl.release(tid, i,j,k) );
  
   return false;
}
/*  Releasing locks example */
assert(lkTbl.release(tid, i,j,k));

// Output commit log
```

<br>

## Key notion
#### `bool isGrowing`
```
true - in Growing Phase
false - in Shrinking Phase
```

<br>

#### `std::condition_variable cv`  
Waiting on *cv  * when  
i) trying to acquire lock  
ii) until there are no requested locks  

Notifying *cv* when  
i) deadlock detected  
ii) no requested locks remained (in Shrinking Phase)  

each i) and ii) are corresponded.
<br>

#### `std::mutex lk` in *LockTable*
It works as *global mutex* described in the specification document.



<br>

## TransactionManager
We have a class, named *TransactionManager*, managing transactions and writing commit log. Each threads repeadtly call `bool transaction(int tid)`, which returns true until  *commit_id* <=*global_execution_order* 
<br>

### Random
A *TransactionManager* has a single random generator, named *Random*, to pick up three (or more if you want) distinct integers in [0, R-1].  It uses a bounded(linear) time complexity algorithm for safety. There is a mutex to avoid the data race.
<br>

### Record
It namely does read and write operation on the record. The proper lock should be acquired before calling any methods here.
<br>

### LockTable
A *TransactionManager* has a *Locktable*.  
Its constructor needs two arguments,  *N* and *R*. A set of linked-lists, corresponding to *Record*, shall contain acquired/pending locks.
All methods but *printLinkedList* for debugging returns a boolean value, which would be true if succeeded and false otherwise. The releasing methods should always true as long as the *LockTable* is well implemented. Hence, it would be safer to do assertion as calling them.  
<br>

i) `bool try{Shared, Exclusive}Lock(int tid, int idx)`  
Both functions shall try locking and it will return false if and only if any deadlocks are detected.  


ii) `bool release(int tid, int i, int j=-1, int k=-1)`  
It shall release the locks on i( ,  j, k) and wake up all sleeping threads to give a chance to acquire its pending lock.  


iii) `bool releaseAndWait(int tid, int i, int j=-1, int k=-1)`  
It will be called as any deadlocks are detected. Hence, it shall release all acquired/pending locks and will be blocked until the other locks are released except for the thread who releases the last one. (We checks by managing *size* of *LockTable*.) That will wake up all the other threads and retry the lock process. (hoping it would be done luckily in a way which would have no deadlocks for every steps)  



 
    

