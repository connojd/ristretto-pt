#ifndef RISTRETTO_TRACE_H
#define RISTRETTO_TRACE_H

#include <cyc.h>
#include <intel-pt.h>
#include <xed-interface.h>
#include <perf_event.h>

#define rmb()asm volatile("lfence":::"memory")

#define DEBUG_FILE_FLAGS O_RDWR | O_CREAT | O_TRUNC
#define DEBUG_FILE_MODE  S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH

typedef unsigned int u32;
typedef unsigned long int u64;
typedef long int s64;

struct flow_monitor {
    /* structs for reconstructing control flow */
    struct pt_insn_decoder *decoder;
    struct pt_config *config;
    struct pt_image *image;
    struct pt_image_section_cache *iscache;

    /* start & end addresses (as integers) for forward-only execution */
    unsigned long fwdstart;
    unsigned long fwdend;

    /* the actual address of the forward-only block */
    const char *addr;

    /* Data for the file backing the forward-only section */
    unsigned long foffset;
    unsigned long memfdsize;
    char *fname;
    int memfd; /* fd for code thats executing forward-only */
    int isid;
};

struct trace {
  struct perf_event_attr * event;
  struct perf_event_mmap_page * header;
  void * base;
  void * data;
  void * aux;
  int fd;
  int header_size;

  /* struct for enforcing forward-only */
  struct flow_monitor *monitor;
};

/**
 * Allocates and clears a struct flow_monitor. This should be
 * called before flow_monitor_start().
 */
struct flow_monitor* flow_monitor_alloc();

/**
 * Initializes the components of the struct flow_monitor referenced
 * by mtr.  mtr->codefd references the contents of
 * [*addr, *(addr + size - 1)].  This file is used by the
 * libipt image cache and probably isn't strictly necessary
 * but using it allows for less mods/more resuse of libipt.
 *
 * A zero return value implies all initializations succeeded
 * and that a thread has been created that will decode the instruction
 * flow and will kill the entire process if forward-only execution
 * is violated.
 */
int flow_monitor_start(struct trace *trace, char *addr, char *size);

/**
 * Frees a struct flow_monitor.
 */
void flow_monitor_free();

/**
 * Cleanup of the flow_monitor and all its owned memory and file.
 * The trace member is not touched, but rather should be cleaned up
 * with ristretto_trace_cleanup.
 */
void flow_monitor_cleanup(struct flow_monitor *mtr);

void * ristretto_trace_start(char *start, char *end);
int ristretto_trace_stop(void * tr);
int ristretto_trace_parse(void * tr);
int ristretto_trace_cleanup(void * tr);

int enforce_fwd_only(struct trace *tr);

#endif
