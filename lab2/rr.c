#include <fcntl.h>
#include <stdbool.h>
#include <stdckdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/queue.h>
#include <sys/stat.h>
#include <unistd.h>

/* A process table entry.  */
struct process
{
    long pid;
    long arrival_time;
    long burst_time;

    TAILQ_ENTRY(process)
    pointers;

    /* Additional fields here */
    long time_left; 
    /* End of "Additional fields here" */
};

TAILQ_HEAD(process_list, process);

/* Skip past initial nondigits in *DATA, then scan an unsigned decimal
   integer and return its value.  Do not scan past DATA_END.  Return
   the integer’s value.  Report an error and exit if no integer is
   found, or if the integer overflows.  */
static long
next_int(char const **data, char const *data_end)
{
    long current = 0;
    bool int_start = false;
    char const *d;

    for (d = *data; d < data_end; d++)
    {
        char c = *d;
        if ('0' <= c && c <= '9')
        {
            int_start = true;
            if (ckd_mul(&current, current, 10) || ckd_add(&current, current, c - '0'))
            {
                fprintf(stderr, "integer overflow\n");
                exit(1);
            }
        }
        else if (int_start)
            break;
    }

    if (!int_start)
    {
        fprintf(stderr, "missing integer\n");
        exit(1);
    }

    *data = d;
    return current;
}

/* Return the first unsigned decimal integer scanned from DATA.
   Report an error and exit if no integer is found, or if it overflows.  */
static long
next_int_from_c_str(char const *data)
{
    return next_int(&data, strchr(data, 0));
}

/* A vector of processes of length NPROCESSES; the vector consists of
   PROCESS[0], ..., PROCESS[NPROCESSES - 1].  */
struct process_set
{
    long nprocesses;
    struct process *process;
};

/* Return a vector of processes scanned from the file named FILENAME.
   Report an error and exit on failure.  */
static struct process_set
init_processes(char const *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        perror("open");
        exit(1);
    }

    struct stat st;
    if (fstat(fd, &st) < 0)
    {
        perror("stat");
        exit(1);
    }

    size_t size;
    if (ckd_add(&size, st.st_size, 0))
    {
        fprintf(stderr, "%s: file size out of range\n", filename);
        exit(1);
    }

    char *data_start = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data_start == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    char const *data_end = data_start + size;
    char const *data = data_start;

    long nprocesses = next_int(&data, data_end);
    if (nprocesses <= 0)
    {
        fprintf(stderr, "no processes\n");
        exit(1);
    }

    struct process *process = calloc(sizeof *process, nprocesses);
    if (!process)
    {
        perror("calloc");
        exit(1);
    }

    for (long i = 0; i < nprocesses; i++)
    {
        process[i].pid = next_int(&data, data_end);
        process[i].arrival_time = next_int(&data, data_end);
        process[i].burst_time = next_int(&data, data_end);
        if (process[i].burst_time == 0)
        {
            fprintf(stderr, "process %ld has zero burst time\n",
                    process[i].pid);
            exit(1);
        }
    }

    if (munmap(data_start, size) < 0)
    {
        perror("munmap");
        exit(1);
    }
    if (close(fd) < 0)
    {
        perror("close");
        exit(1);
    }
    return (struct process_set){nprocesses, process};
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "%s: usage: %s file quantum\n", argv[0], argv[0]);
        return 1;
    }

    struct process_set ps = init_processes(argv[1]);
    long quantum_length = (strcmp(argv[2], "median") == 0 ? -1
                                                          : next_int_from_c_str(argv[2]));
    if (quantum_length == 0)
    {
        fprintf(stderr, "%s: zero quantum length\n", argv[0]);
        return 1;
    }

    struct process_list list;
    TAILQ_INIT(&list);

    long total_wait_time = 0;
    long total_response_time = 0;

    /* Your code here */

    //Sort process_set 

    for (long i = 1; i < ps.nprocesses; i++) {
        struct process key = ps.process[i];
        long j = i - 1;

        while (j >= 0 && ps.process[j].arrival_time > key.arrival_time) {
            ps.process[j + 1] = ps.process[j];
            j = j - 1;
        }
        ps.process[j + 1] = key;
    }

    for(long i = 0; i<ps.nprocesses; i++){
        ps.process[i].time_left = -1; 
    }

    long time = ps.process[0].arrival_time; 
    TAILQ_INSERT_TAIL(&list, &ps.process[0], pointers); 
    long cur_process = 1; 
    int list_size = 1; 
    bool median = quantum_length == -1; 
    struct process *temp = &ps.process[0]; 
    while(cur_process < ps.nprocesses && time >= ps.process[cur_process].arrival_time){
            TAILQ_INSERT_TAIL(&list, &ps.process[cur_process], pointers);
            cur_process++; 
            list_size++; 
    }
    while(!TAILQ_EMPTY(&list)){
       
        if(median){
            long median_arr[list_size]; 
            struct process* dummy; 
            int counter = 0; 
            TAILQ_FOREACH(dummy, &list, pointers){
                if(dummy->time_left != -1){
                    median_arr[counter] = dummy->burst_time - dummy->time_left; 
                } 
                else
                    median_arr[counter] = 0; 
                counter++;
            }

            for (long i = 1; i <  counter; i++) {
                long key = median_arr[i];
                long j = i - 1;

                while (j >= 0 && median_arr[j] > key) {
                    median_arr[j + 1] = median_arr[j];
                    j = j - 1;
                }
                median_arr[j + 1] = key;
            }

            if(counter == 0){
                quantum_length = 1; 
            }
            else if(counter % 2 == 0){
                double temp = (median_arr[counter/2] + median_arr[counter/2-1] )/2.0; 
                if((median_arr[counter/2] + median_arr[counter/2-1] ) % 2 == 1)
                    quantum_length = (long)temp % 2 == 0 ? (long)temp : (long)temp +1; 
                else quantum_length = (long)temp; 
            }
            else {
                quantum_length = median_arr[counter/2]; 
            }
            
            if(quantum_length == 0) quantum_length = 1; 
            
        }

        struct process* proc_cur = TAILQ_FIRST(&list); 
        if(proc_cur != temp) {
            time++; 
        }
            
        temp = proc_cur; 
        if(proc_cur->time_left == -1){
            total_response_time += time - proc_cur->arrival_time; 
            proc_cur->time_left = proc_cur->burst_time; 
        }
        time += quantum_length < proc_cur->time_left ? quantum_length : proc_cur->time_left; 
        while(cur_process < ps.nprocesses && time > ps.process[cur_process].arrival_time){
            TAILQ_INSERT_TAIL(&list, &ps.process[cur_process], pointers);
            list_size++; 
            cur_process++; 
        }
        proc_cur->time_left = 0 > proc_cur->time_left - quantum_length ? 0 : proc_cur->time_left - quantum_length; 

        if(proc_cur->time_left == 0){
            total_wait_time += time - proc_cur->arrival_time - proc_cur->burst_time; 
            TAILQ_REMOVE(&list, proc_cur, pointers); 
            list_size--; 
        }
        else{
            TAILQ_REMOVE(&list, proc_cur, pointers);
            TAILQ_INSERT_TAIL(&list, proc_cur, pointers); 
        }

        while(cur_process < ps.nprocesses && time == ps.process[cur_process].arrival_time){
            TAILQ_INSERT_TAIL(&list, &ps.process[cur_process], pointers);
            list_size++; 
            cur_process++; 
        }

        if(cur_process < ps.nprocesses && TAILQ_EMPTY(&list)){
            time = ps.process[cur_process].arrival_time; 
            //temp = &ps.process[cur_process]; 
            TAILQ_INSERT_TAIL(&list, &ps.process[cur_process], pointers); 
            cur_process++; 
            list_size++; 
        }
    }
    /* End of "Your code here" */

    printf("Average wait time: %.2f\n",
           total_wait_time / (double)ps.nprocesses);
    printf("Average response time: %.2f\n",
           total_response_time / (double)ps.nprocesses);

    if (fflush(stdout) < 0 || ferror(stdout))
    {
        perror("stdout");
        return 1;
    }

    free(ps.process);
    return 0;
}
