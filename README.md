## Trivia
The project is a Kahoot clone.<br>
You can create rooms and games, and play with multiple people to compete for the best score.

## Inner working
The project is separated into 2 subprojects.<br>
Trivia (C++): The server/backend handles all the users, games, and stores all the data needed.<br>
Client (C#): The client/frontend is the user-interface. It lets the user send data to the server and interpret data back in a nice GUI, basically a game.

## Disclamer
This is a reupload of an old project of mine from GitLab which I did for a course.<br>
So sometimes there are some weird design choices that were made.<br>
For example: Doing an API call for questions for the first time the Server creates the database.<br>
Which was the course requirement for the project.<br>
Also the project was made with VS22, and not a cross-platform build system like CMake.