# Fixed-Size Collections for Arduino

The goal of this library is to provide efficient implementations of some collection data structures which are suited for being used on the Andruino platform.

One key aspect is that they are implemented as fixed-size templates, so that the memory usage is known at compile time, and thus making the data structures more memory-efficient (no linked lists of dynamically allocated memory) and also avoiding the problems with fragmentation of the heap.

There are no dependencies other than the Arduino library (needed for String).

## Available Collections
### ArrayList

A fixed-size, array-based list.

### Deque

A double-ended queue implementation.

### BitSet

Basically just a memory-efficient and bounds-checked array of bits (booleans).

### HashSet

A set implementation which uses the given hash comparer.

### HashMap

A map (aka dictionary or associative array) implementation which uses the given hash comparer.

## MessageLoop

The MessageLoop is a special queue collection for implementing simple cooperative multitasking. It is basically a queue of callback functions, which can be configured with a delay before being invoked. The `MessageLoop::process()` method is designed to be called in the main `loop()` function.

Since the code will be executed in the context of the `process()` caller, the MessageLoop can also be used to queue and safely perform operations which are not allowed everywhere, such as writing to `Serial`.

```
int loopStatus() {
  Serial.println("(Message Loop is alive)");
  return 5000; // perform this again in 5s
}

int once() {
  Serial.println("Executed once");
  return 0; // do not perform again
}

int wifiStatus(bool& connected) {
  const bool connectedNew = WiFi.status() == WL_CONNECTED;
  if (connected != connectedNew) {
    connected = connectedNew;
    if (connected) {
      Serial.print("WiFi connected, IP address: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("WiFi disconnected");  
    }
  }
  return 500; // perform this again in 500ms
}

void setup() {
  messageLoop.post(loopStatus);
  messageLoop.post(once, 10000);
  WiFi.begin("SSID", "KEY");
  messageLoop.post(wifiStatus, false);
}

void loop() {
  messageLoop.process();
}
```

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