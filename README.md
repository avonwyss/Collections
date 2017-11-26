# Fixed-Size Collections for Arduino

The goal of this library is to provide efficient implementations of some collection data structures which are suited for being used on the Andruino platform.

One key aspect is that they are implemented as fixed-size templates, so that the memory usage is known at compile time, and thus making the data structures more memory-efficient (no linked lists of dynamically allocated memory) and also avoiding the problems with fragmentation of the heap.

There are no dependencies other than the Arduino library (needed for String).

## Available Collections
### BitSet

Basically just a memory-efficient and bounds-checked array of bits (booleans).

### Deque

A double-ended queue implementation.

### HashSet

A set implementation which uses the given hash comparer.

### HashMap

A map (aka dictionary or associative array) implementation which uses the given hash comparer.

## Hash Comparer

For the hashed collections, a hash comparer is used to hash values and compare them for equality. The following are predefined:

* `GenericHashComparer`: Used by default; uses a cast to `unsigned int` for hashing and the `==` operator for equality comparison.
* `StringHashComparer`: Case-sensitive String hasher and comparer.
* `StringIgnoreCaseHashComparer`: Case-insensitive String hasher and comparer.

## Error Handling

The following types of errors can occur in the collections:

* Out of bound
* Out of space
* Key Not Found
* Duplicate Key
* Is Empty

The last template parameter is an optional function which handles error conditions. You can specify your own or use one of the predefined:

* `IgnoreCollectionErrorHandler`: Do nothing; errors will silently be ignored (but the requested operation will not take place)
* `LogFailCollectionErrorHandler`: Log error to Serial and go into infinite loop (may trigger watchdog reset)

When not specified, `IgnoreCollectionErrorHandler` will be used.