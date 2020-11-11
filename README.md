## Socket Programming

### Running the code
- Go to server directory and run `gcc server.c -o server` followed by `./server`
- Open client directory in another tab or another terminal window and run `gcc client.c -o client` followed by `./client`
This should show a connected message on both terminals. Additionally on client terminal, a prompt of the form `client>` should be displayed in cyan.

### Commands
On the client terminal, the following commands are supported:
1. `get <filename>`
	- If the file with the name specified exists in the server directory, it will be downloaded in the client directory. If the file doesn't exist error message will be shown.
	- Multiple files as arguments are also handled. eg: `get file1.txt file2.txt file3.txt`.
	- While a file is being downloaded, download percentage is shown.
	- Pictures can also be downloaded using this command.

2. `exit`
	This closes the connection with server and both client and server programs are terminated.

*Note:* Only regular files can be downloaded. Directories can't be downloaded. 
