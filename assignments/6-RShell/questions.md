1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

This is done by checking the RDSH_EOF_CHAR at the end of the server's response. It can handle partial reads by using a fixed size bugger to get data across multiple rcev call. 

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

It should explicitly define the boundaries and if it does not then there could be message merging or clients or servers could be waiting for missing data. 

3. Describe the general differences between stateful and stateless protocols.

Stateful will maintain sesion context while stateless each request is independent. The stateful protocols need session management and allow for complex interactions, while stateless protocols will scale better but don't have context. 

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

UDP are better when there is aneed for multicast, low latency, or when smaller more frequent messages are needed. 

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

The sockets API will prove the interface to use network communications.