# vscp-fifo

Last updated: 2026-06-11

`vscp-fifo` provides a small ring-buffer queue for `vscpEvent*` pointers.

- Header: `common/vscp-fifo.h`
- Implementation: `common/vscp-fifo.c`

## Data structure

The queue state is stored in:

```c
typedef struct {
  vscp_event_t **buf;
  size_t head;
  size_t tail;
  size_t size;
} vscp_fifo_t;
```

Design notes:

- Circular buffer with one reserved slot to detect full vs empty.
- Effective capacity is `requested_size` (allocated size is `requested_size + 1`).
- Stores pointers only; events are not deep-copied on write/read.

## API summary

- `void vscp_fifo_init(vscp_fifo_t *f, uint16_t size)`
  - Initializes indices, allocates pointer array, zeros storage.
- `void vscp_fifo_clear(vscp_fifo_t *f)`
  - Resets indices and attempts to clear queued entries.
- `size_t vscp_fifo_write(vscp_fifo_t *f, vscp_event_t *pev)`
  - Enqueue one pointer. Returns `1` on success, `0` if full.
- `size_t vscp_fifo_read(vscp_fifo_t *f, vscp_event_t **pev)`
  - Dequeue one pointer. Returns `1` on success, `0` if empty.
- `size_t vscp_fifo_getFree(vscp_fifo_t *f)`
  - Returns number of free slots available for enqueue.
- `void vscp_fifo_deinit(vscp_fifo_t *f)`
  - Drains and destroys the FIFO: frees every event still queued inside it, then releases the internal pointer buffer. Safe to call on an already-empty FIFO. The `vscp_fifo_t` object must not be used again without a new `vscp_fifo_init()`.

## Usage pattern

```c
vscp_fifo_t q;
vscp_fifo_init(&q, 32);      // capacity = 32 events

if (!vscp_fifo_write(&q, pev)) {
  // queue full
}

vscp_event_t *out = NULL;
if (vscp_fifo_read(&q, &out)) {
  // process out
}
```

## Ownership and lifetime

- The FIFO stores raw `vscpEvent*` pointers.
- The caller controls allocation and release strategy for events.
- Call `vscp_fifo_deinit()` when the FIFO is no longer needed. It frees all queued events and releases the internal buffer. Example:

  ```c
  vscp_fifo_deinit(&g_vscp_rx_fifo);  // drains remaining events, frees buffer
  ```

## Concurrency model

The header comments describe a single-producer/single-consumer style lookahead ring-buffer approach. In practice:

- `head` is updated by writer path.
- `tail` is updated by reader path.

For multi-producer/multi-consumer access, protect operations with external synchronization.

## Behavior notes from current implementation

These notes describe current source behavior in `common/vscp-fifo.c`:

- `vscp_fifo_init` uses `malloc` for the pointer buffer and does not check allocation failure.
- `vscp_fifo_clear` resets `head`/`tail` and clears buffer entries.
- `vscp_fifo_getFree` returns free slots, not used slots.

If you build safety-critical paths, consider wrapping this module with:

- allocation checks,
- use of `vscp_fifo_deinit()` for deterministic teardown,
- clear ownership conventions for event memory.

Example skeleton: [examples/vscp-fifo-usage-skeleton.c](examples/vscp-fifo-usage-skeleton.c)
