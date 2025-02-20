/*
	This code demonstrates the use of poll() to monitor input readiness on
	standard input (stdin) with a timeout of 2.5 seconds. It highlights the
	concept of blocking vs. non-blocking I/O.

	🚦 Context: Blocking vs. Non-Blocking I/O

	🔒 Blocking I/O
	Default behavior for most I/O operations.
	When you read from a file descriptor, the program waits ("blocks") until
	data is available.
	E.g., scanf() on standard input (stdin) blocks until the user presses ENTER.

	🚦 Non-Blocking I/O
	The program does not wait. Instead, it immediately returns, even if no data
	is available.
	Useful for real-time applications and event-driven architectures.
	poll() is a way to simulate non-blocking behavior by waiting only for a
	specific duration.
*/

#include <stdio.h>
#include <poll.h>

int main(void)
{
	struct pollfd pfds[1]; // More if you want to monitor more

	pfds[0].fd = 0;          // Standard input
	pfds[0].events = POLLIN; // Tell me when ready to read

	// If you needed to monitor other things, as well:
	//pfds[1].fd = some_socket; // Some socket descriptor
	//pfds[1].events = POLLIN;  // Tell me when ready to read

	printf("Hit RETURN or wait 2.5 seconds for timeout\n");

	int num_events = poll(pfds, 1, 2500); // 2.5 second timeout

	if (num_events == 0) {
		printf("Poll timed out!\n");
	} else {
		int pollin_happened = pfds[0].revents & POLLIN;

		if (pollin_happened) {
			printf("File descriptor %d is ready to read\n", pfds[0].fd);
		} else {
			printf("Unexpected event occurred: %d\n", pfds[0].revents);
		}
	}

	return 0;
}

/*
 * ============================
 * POLL() CODE ANALYSIS & CONTEXT
 * ============================
 *
 * This section of code demonstrates the use of poll() for non-blocking I/O.
 * The example monitors standard input (stdin) for readability with a timeout
 * of 2.5 seconds. It showcases the difference between blocking and non-blocking
 * behavior using the poll() function.
 *
 * ---------------------------------------------------
 * Key Concepts:
 * ---------------------------------------------------
 * 1. Blocking vs. Non-Blocking I/O:
 *    - Blocking I/O: The program waits ("blocks") until data is available.
 *      Example: Standard input (stdin) blocks until the user presses ENTER.
 *    - Non-Blocking I/O: The program does not wait. It immediately returns
 *      even if no data is available, useful for real-time applications.
 *    - poll() enables non-blocking behavior by waiting only for a specific
 *      duration (timeout).
 *
 * 2. poll() Function:
 *    - poll() is used to monitor multiple file descriptors simultaneously.
 *    - It can wait for specific events (e.g., data ready to read, error states).
 *    - pollfd structure:
 *       - fd: File descriptor to monitor (e.g., standard input, sockets).
 *       - events: Events to watch for (e.g., POLLIN for data ready to read).
 *       - revents: Stores the actual event that occurred.
 *
 * 3. Blocking vs. Non-Blocking Behavior with poll():
 *    - poll(pfds, 1, 2500) in this example:
 *      - Monitors one file descriptor (standard input, fd 0).
 *      - Waits for up to 2.5 seconds (2500 milliseconds).
 *      - Returns immediately if an event occurs or times out.
 *    - Timeout values:
 *      - 0: Non-blocking, returns immediately.
 *      - -1: Blocking, waits indefinitely until an event occurs.
 *      - >0: Waits for a specific time in milliseconds (2.5s in this case).
 *
 * 4. Practical Use:
 *    - Ideal for scenarios where the program needs to remain responsive.
 *    - Commonly used in network servers to monitor multiple sockets.
 *    - Allows mixing of user input and socket communication in event-driven systems.
 *
 * ---------------------------------------------------
 * Code Analysis - Line by Line:
 * ---------------------------------------------------
 *
 * struct pollfd pfds[1];
 * - Creates an array of pollfd structures.
 * - The array size is 1, but it can be extended to monitor multiple file descriptors.
 *
 * pfds[0].fd = 0; // Standard input
 * - Sets the file descriptor to monitor (0 = stdin).
 * - File descriptors: 0 (stdin), 1 (stdout), 2 (stderr).
 *
 * pfds[0].events = POLLIN;
 * - Specifies that we are interested in the POLLIN event.
 * - POLLIN triggers when data is available to read.
 *
 * printf("Hit RETURN or wait 2.5 seconds for timeout\n");
 * - Informs the user about the input options.
 *
 * int num_events = poll(pfds, 1, 2500);
 * - Calls poll() to monitor the file descriptors.
 * - First parameter: The array of pollfd structures.
 * - Second parameter: The number of items in the array (1 in this case).
 * - Third parameter: Timeout of 2500 milliseconds (2.5 seconds).
 *
 * if (num_events == 0) {
 *     printf("Poll timed out!\n");
 * }
 * - Checks if the poll() call timed out.
 * - If no events occurred within 2.5 seconds, this branch executes.
 *
 * int pollin_happened = pfds[0].revents & POLLIN;
 * - Evaluates whether the POLLIN event occurred.
 * - Uses bitwise AND (&) to check if the event matches POLLIN.
 *
 * if (pollin_happened) {
 *     printf("File descriptor %d is ready to read\n", pfds[0].fd);
 * }
 * - Executes if standard input (stdin) is ready to be read.
 * - The program will not block on read operations for this file descriptor.
 *
 * else {
 *     printf("Unexpected event occurred: %d\n", pfds[0].revents);
 * }
 * - Catches any unexpected events.
 * - This might include error events like POLLERR, POLLHUP, or POLLNVAL.
 *
 * return 0;
 * - The program exits cleanly.
 *
 * ---------------------------------------------------
 * Example Scenario:
 * ---------------------------------------------------
 * - The user is prompted to hit RETURN or wait 2.5 seconds.
 * - If input is detected within the timeout, the program notifies that stdin is ready.
 * - If no input is received, it notifies that the poll timed out.
 *
 */
