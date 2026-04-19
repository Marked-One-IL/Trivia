static class Constants
{
	public static class Client
	{
		public static readonly byte SIGNUP = 1;
		public static readonly byte LOGIN = 2;
		public static readonly byte LOGOUT = 6;
		public static readonly byte GET_PLAYERS_IN_ROOM = 7;
		public static readonly byte JOIN_ROOM = 8;
		public static readonly byte GET_ROOMS = 9;
		public static readonly byte CREATE_ROOM = 10;
		public static readonly byte GET_HIGH_SCORE = 11;
		public static readonly byte GET_PERSONAL_STATS = 12;
		public static readonly byte CLOSE_ROOM = 13;
		public static readonly byte START_GAME = 14;
		public static readonly byte GET_ROOM_STATE = 15;
		public static readonly byte LEAVE_ROOM = 16;
		public static readonly byte LEAVE_GAME = 17;
		public static readonly byte GET_QUESTION = 18;
		public static readonly byte SUBMIT_ANSWER = 19;
        public static readonly byte GET_GAME_RESULTS = 20;
        public static readonly byte ARE_ALL_PLAYERS_DONE = 21;
    }

    public static class Server
	{
		public static readonly byte SIGNUP = 3;
		public static readonly byte LOGIN = 4;
		public static readonly byte FAILURE = 5;
		public static readonly byte LOGOUT = 6;
		public static readonly byte GET_PLAYERS_IN_ROOM = 7;
		public static readonly byte JOIN_ROOM = 8;
		public static readonly byte GET_ROOMS = 9;
		public static readonly byte CREATE_ROOM = 10;
		public static readonly byte GET_HIGH_SCORE = 11;
		public static readonly byte GET_PERSONAL_STATS = 12;
		public static readonly byte CLOSE_ROOM = 13;
		public static readonly byte START_GAME = 14;
		public static readonly byte GET_ROOM_STATE = 15;
		public static readonly byte LEAVE_ROOM = 16;
		public static readonly byte LEAVE_GAME = 17;
		public static readonly byte GET_QUESTION = 18;
		public static readonly byte SUBMIT_ANSWER = 19;
        public static readonly byte GET_GAME_RESULTS = 20;
        public static readonly byte ARE_ALL_PLAYERS_DONE = 21;
    }

    public static class Room_Status
	{
		public static readonly byte IN_GAME = 1;
		public static readonly byte OPEN = 2;
        public static readonly byte CLOSED = 3;
	}
}